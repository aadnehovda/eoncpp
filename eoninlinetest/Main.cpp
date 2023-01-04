#include "TestBase.h"
#include "Timer.h"



void usage( const std::filesystem::path& prog )
{
	eon::term << "Usage: " << eon::style::cyan << prog.c_str() << eon::style::normal << " ";

	eon::term << eon::style::weak << "[" << eon::style::yellow << "-c" << eon::style::normal << "="
		<< eon::style::yellow << "<class name>" << eon::style::weak << "]" << eon::style::normal;
	eon::term << eon::style::weak << "[" << eon::style::yellow << "-f" << eon::style::normal << "="
		<< eon::style::yellow << "<function name>" << eon::style::weak << "]" << eon::style::normal;
	eon::term << eon::style::weak << "[" << eon::style::yellow << "-t" << eon::style::normal << "="
		<< eon::style::yellow << "<test name>" << eon::style::weak << "]" << eon::style::normal;

	//eon::term << eon::style::weak << "[" << eon::style::yellow << "--class" << eon::style::normal << "="
	//	<< eon::style::yellow << "<name>" << eon::style::weak << "]" << eon::style::normal;
	//eon::term << " " << eon::style::weak << "[" << eon::style::yellow << "--method" << eon::style::normal << "="
	//	<< eon::style::yellow << "<name>" << eon::style::weak << "]" << eon::style::normal;
	//eon::term << " " << eon::style::weak << "[" << eon::style::yellow << "--test" << eon::style::normal << "="
	//	<< eon::style::yellow << "<name>" << eon::style::weak << "]" << eon::style::normal;
	//eon::term << "\n";

	eon::term << "\n";
	eon::term << eon::style::strong << "Options:" << eon::style::normal << "\n";
	eon::term << "  " << eon::style::yellow << "-c" << eon::style::normal
		<< "  : Name of class to run tests for. Default is to run for all classes!\n";
	eon::term << "  " << eon::style::yellow << "-f" << eon::style::normal
		<< " : Name of function/method to run tests for. Default is to run for all functions!\n";
	eon::term << "  " << eon::style::yellow << "-t" << eon::style::normal
		<< "   : Name of test to run (for specified class(es) and function(s)).\n";
	eon::term << "             Default is to run all tests!\n";

	//eon::term << "\n";
	//eon::term << eon::style::strong << "Options:" << eon::style::normal << "\n";
	//eon::term << "  " << eon::style::yellow << "--class" << eon::style::normal
	//	<< "  : Name of class to run tests for. Default is to run for all classes!\n";
	//eon::term << "  " << eon::style::yellow << "--method" << eon::style::normal
	//	<< " : Name of method to run tests for. Default is to run for all methods!\n";
	//eon::term << "  " << eon::style::yellow << "--test" << eon::style::normal
	//	<< "   : Name of test to run (for specified class(es) and method(s)).\n";
	//eon::term << "             Default is to run all tests!\n";
}

class Args
{
public:
	std::filesystem::path Exe;
	eon::string Cls, Func, Tst;

	eon::string Class;
	eon::string Method;
	eon::string Test;
	int Result = 0;
};

Args processArgs( int argc, const char* argv[] )
{
	if( argc == 0 )
		exit( 4 );

	Args args;
	args.Exe = argv[ 0 ];
	for( int i = 1; i < argc; ++i )
	{
		eon::string arg( argv[ i ] );
		if( arg == "--help" || arg == "/?" || arg == "-h" || arg == "-?" )
		{
			usage( args.Exe );
			args.Result = -1;
			break;
		}

		else if( arg.startsWith( "-c=" ) )
			args.Cls = arg.substr( arg.begin() + 3 );
		else if( arg.startsWith( "-f=" ) )
			args.Func = arg.substr( arg.begin() + 3 );
		else if( arg.startsWith( "-t=" ) )
			args.Tst = arg.substr( arg.begin() + 3 );

		//else if( arg.startsWith( "--class=" ) )
		//	args.Class = arg.substr( arg.begin() + 8 );
		//else if( arg.startsWith( "--method=" ) )
		//	args.Method = arg.substr( arg.begin() + 9 );
		//else if( arg.startsWith( "--test=" ) )
		//	args.Test = arg.substr( arg.begin() + 7 );

		else
		{
			eon::term << eon::style::error << "ERROR: Invalid argument!" << eon::style::normal << "\n";
			eon::term << "Argument: " << eon::style::red << arg << eon::style::normal << "\n";
			exit( 4 );
		}
	}

	return args;
}

eonitest::TestPtr create( eonitest::TestBase::TestRef& test ) noexcept
{
	try
	{
		return test.Factory->create();
	}
	catch( ... )
	{
		return eonitest::TestPtr();
	}
}

//eonitest::__EonTestPtr createTest( eonitest::__EonTestBase::TestRef& test ) noexcept
//{
//	try
//	{
//		return test.Factory->createTest();
//	}
//	catch( ... )
//	{
//		return eonitest::__EonTestPtr();
//	}
//}


eonitest::TestPtr run( eonitest::TestBase::TestRef& test ) noexcept
{
	eonitest::TestPtr test_obj;
	try
	{
		test_obj = test.Factory->create();
	}
	catch( ... ) {}
	if( !test_obj )
		return test_obj;

	try
	{
		eon::locale base_eon_locale = eon::locale::get();
		std::locale base_std_locale;
		bool success = test_obj->runTest();
		std::locale::global( base_std_locale );
		eon::locale::set( base_eon_locale );
	}
	catch( eon::exception& e )
	{
		test_obj->Details.Success = false;
		test_obj->Details.Log << eonitest::style::error << "ERROR!" << " Got an eon::exception!" << '\n';
		test_obj->Details.Log << "  " << eonitest::style::key << "Name"
			<< eonitest::style::value << eon::str( e.name() ) << '\n';
		test_obj->Details.Log << "  " << eonitest::style::key << "Info" << eonitest::style::value << e.info() << '\n';
	}
	catch( std::exception& e )
	{
		test_obj->Details.Success = false;
		test_obj->Details.Log << eonitest::style::error << "ERROR!" << " Got an std::exception!" << '\n';
		eon::string type = typeid( e ).name();
		eon::string what = eon::string( e.what() ).trim();
		test_obj->Details.Log << "  " << eonitest::style::key << "Type"
			<< eonitest::style::value << type << '\n';
		if( !what.empty() )
			test_obj->Details.Log << "  " << eonitest::style::key << "What" << eonitest::style::value << what << '\n';
	}
	catch( ... )
	{
		test_obj->Details.Success = false;
		test_obj->Details.Log << eonitest::style::error << "ERROR!" << " Got an unknown exception!" << '\n';
	}
	return test_obj;
}

//eonitest::__Result runTest( eonitest::__EonTestBase::TestRef& test ) noexcept
//{
//	eonitest::__EonTestPtr test_obj;
//	try {
//		test_obj = test.Factory->createTest(); }
//	catch( ... ) { }
//	if( !test_obj )
//		return eonitest::__Result::failedToConstruct();
//
//	try
//	{
//		eon::locale base_eon_locale = eon::locale::get();
//		std::locale base_std_locale;
// 		auto result = test_obj->_runEonTest_();
//		std::locale::global( base_std_locale );
//		eon::locale::set( base_eon_locale );
//		return result;
//	}
//	catch( eon::exception& e )
//	{
//		return eonitest::__Result::failureByException( "eon::exception", e.details() );
//	}
//	catch( std::exception& e )
//	{
//		eon::string details = typeid( e ).name();
//		eon::string what = e.what();
//		details += what.trim();
//		return eonitest::__Result::failureByException( "std::exception", details );
//	}
//	catch( ... )
//	{
//		return eonitest::__Result::failureByException( "exception", "Unknown" );
//	}
//}


void reportTestStart( const eonitest::TestName& name )
{
	eon::term << eon::style::weak << "Class=" << eon::style::strong << name.ClsName << eon::style::normal;
	eon::term << eon::style::weak << " Func=" << eon::style::strong << name.FuncName << eon::style::normal;
	eon::term << eon::style::weak << " Test=" << eon::style::strong << name.TstName << eon::style::normal;
	if( name.length() < 50 )
		eon::term << eon::string().padRight( 50 - name.length() );
}

//void reportTestStart( const eonitest::__EonTestName& name )
//{
//	eon::term << "Class=" << eon::style::green << name.className() << eon::style::normal;
//	eon::term << " Method=" << eon::style::blue << name.methodName() << eon::style::normal;
//	eon::term << " Test=" << eon::style::yellow << name.testName() << eon::style::normal;
//	if( name.length() < 50 )
//		eon::term << eon::string().padRight( 50 - name.length() );
//}

void reportTestOK()
{
	eon::term << eon::style::success << " OK " << eon::style::normal << "\n";
}

void reportTestFail( const eonitest::TestDetails& details, const eonitest::TestLocation& location )
{
	eon::term << eon::style::error << "FAIL" << eon::style::normal << "\n";
	eon::term << "  " << eon::style::strong << "Location  : " << eon::style::yellow << location.File << eon::style::normal
		<< ":" << eon::style::magenta << location.NumLine << eon::style::normal << "\n";
	for( auto& line : details.Log.Lines )
	{
		for( auto& item : line.Items )
			eon::term << item.Style << item.Item;
		eon::term << eon::style::normal << "\n";
	}
}

size_t runTests( eon::string cls_name, eon::string func_name, eon::string tst_name, std::list<eonitest::TestName>& failed )
{
	if( !eonitest::TestBase::TestsList || eonitest::TestBase::TestsList->empty() )
		return 0;

	size_t total = 0;
	for( auto& test : *eonitest::TestBase::TestsList )
	{
		if( !cls_name.empty() && cls_name.compare( test.Name.ClsName, eon::substring::ICaseCompare ) != 0 )
			continue;
		if( !func_name.empty() && func_name.compare( test.Name.FuncName, eon::substring::ICaseCompare ) != 0 )
			continue;
		if( !tst_name.empty() && tst_name.compare( test.Name.TstName, eon::substring::ICaseCompare ) != 0 )
			continue;

		++total;
		reportTestStart( test.Name );
		eon::string error;
		eonitest::Timer timer;
		auto test_ptr = run( test );
		if( !test_ptr )
			continue;
		eon::term << " " << eon::style::magenta << timer.elapsed() << eon::style::normal << " ";
		if( test_ptr->Details.Success )
			reportTestOK();
		else
		{
			reportTestFail( test_ptr->Details, test.Location );
			failed.push_back( test.Name );
		}
	}
	eon::term << "\n";
	return total;
}



//void reportTestFail( const ::eonitest::__Result& result, const ::eonitest::__EonLocation& location )
//{
//	eon::term << eon::style::error << "FAIL" << eon::style::normal << "\n";
//	eon::term << "  Location : " << eon::style::yellow << location.file() << eon::style::normal << ":"
//		<< eon::style::magenta << location.line() << eon::style::normal << "\n";
//	if( result.constructError() )
//		eon::term << "  Failed to construct test!\n";
//	else if( result.exceptionError() )
//	{
//		eon::term << "  Got unexpected exception!\n";
//		eon::term << "  Type   : " << eon::style::cyan << result.ExceptionType << eon::style::normal << "\n";
//		eon::term << "  Details: " << eon::style::code << result.ExceptionDetails << eon::style::normal << "\n";
//		if( !result.Details.empty() )
//			eon::term << "  " << result.Details << "\n";
//	}
//	else if( result.noThrowError() )
//	{
//		eon::term << "  Expected exception but none were thrown!\n";
//		eon::term << "  Expected : " << eon::style::cyan << result.ExceptionDetails << eon::style::normal << "\n";
//		if( !result.Details.empty() )
//			eon::term << "  " << result.Details << "\n";
//	}
//	else if( result.trewError() )
//	{
//		eon::term << "  Expected exception not to be thrown but it was!\n";
//		eon::term << "  Exception: " << eon::style::cyan << result.ExceptionDetails << eon::style::normal << "\n";
//	}
//	else if( result.missingOpError() )
//		eon::term << "  Missing valid test operation!";
//	else
//	{
//		eon::term << "  Operation: " << eon::style::key_1 << "<expected result>" << eon::style::normal
//			<< " " << eon::style::strong << result.OperatorName << eon::style::normal << " "
//			<< eon::style::key_2 << "<actual result>" << eon::style::normal << "\n";
//		eon::term << "  Expected: " << eon::style::key_1 << result.ExpectedExpr << eon::style::normal << "\n";
//		eon::term << "     Value: " << eon::style::value_1 << result.ExpectedVal << eon::style::normal << "\n";
//		eon::term << "  Actual  : " << eon::style::key_2 << result.ActualExpr << eon::style::normal << "\n";
//		eon::term << "     Value: " << eon::style::value_2 << result.ActualVal << eon::style::normal << "\n";
//		if( !result.Details.empty() )
//			eon::term << "  " << result.Details << "\n";
//	}
//}

//size_t runTests(
//	eon::string classname, eon::string methodname, eon::string testname, std::list<eonitest::__EonTestName>& failed )
//{
//	if( !eonitest::__EonTestBase::TestsList || eonitest::__EonTestBase::TestsList->empty() )
//		return 0;
//
//	size_t total = 0;
//	for( auto& test : *eonitest::__EonTestBase::TestsList )
//	{
//		if( !classname.empty() && classname.compare( test.Name.className(), eon::substring::ICaseCompare ) != 0 )
//			continue;
//		if( !methodname.empty() && methodname.compare( test.Name.methodName(), eon::substring::ICaseCompare ) != 0 )
//			continue;
//		if( !testname.empty() && testname.compare( test.Name.testName(), eon::substring::ICaseCompare ) != 0 )
//			continue;
//
//		++total;
//		reportTestStart( test.Name );
//		eon::string error;
//		eonitest::Timer timer;
//		auto result = runTest( test );
//		eon::term << " " << eon::style::magenta << timer.elapsed() << eon::style::normal << " ";
//		if( result )
//			reportTestOK();
//		else
//		{
//			reportTestFail( result, test.Location );
//			failed.push_back( test.Name );
//		}
//	}
//	eon::term << "\n";
//	return total;
//}


int main( int argc, const char* argv[] )
{
	auto args = processArgs( argc, argv );
	if( args.Result != 0 )
		return args.Result < 0 ? 0 : args.Result;

//	std::list<eonitest::__EonTestName> failed;
	std::list<eonitest::TestName> failed;
	eonitest::Timer timer;
	auto total = runTests( args.Cls, args.Func, args.Tst, failed );
//	auto total = runTests( args.Class, args.Method, args.Test, failed );
	auto duration = timer.elapsed();
	if( total > 0 )
		eon::term << "Running " << eon::style::magenta << total << eon::style::normal << " test"
		<< ( total > 1 ? "s" : "" ) << " tests took: " << eon::style::magenta << duration << eon::style::normal << "\n";

	if( total == 0 )
	{
		eon::term << eon::style::error << " No tests have been defined! " << eon::style::normal << "\n";
		return 4;
	}
	else if( failed.empty() )
	{
		eon::term << eon::style::success << " All tests passed! " << eon::style::normal << "\n";
		return 0;
	}
	else
	{
		eon::term << "\n" << eon::style::error << "The following tests failed:" << eon::style::normal << "\n";
		for( auto& name : failed )
			eon::term << eon::style::red << "  - " << eon::style::normal
			//<< "Class=" << eon::style::cyan << name.className() << eon::style::normal
			//<< " Method=" << eon::style::cyan << name.methodName() << eon::style::normal
			//<< " Test=" << eon::style::cyan << name.testName() << eon::style::normal
			<< eon::style::weak << "Class=" << eon::style::strong << name.ClsName << eon::style::normal
			<< eon::style::weak << " Function=" << eon::style::strong << name.FuncName << eon::style::normal
			<< eon::style::weak << " Test=" << eon::style::strong << name.TstName << eon::style::normal
			<< "\n";
		eon::term << "\n";
		return 4;
	}
}
