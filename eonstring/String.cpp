#include "String.h"
#include <eoninlinetest/InlineTest.h>
#include <cctype>
#include <regex>
#include <unordered_map>


namespace eon
{
	const string string::Empty;




	///////////////////////////////////////////////////////////////////////////
	//
	// String hashing
	//

	EON_NO_TEST( string, hash32 );
	EON_NO_TEST( string, hash64 );
	EON_NO_TEST( string, hash );




	///////////////////////////////////////////////////////////////////////////
	//
	// Comparing
	//

	EON_TEST( string, startsWith, string_empty,
		EON_FALSE( string( "" ).startsWith( "abc" ) ) );
	EON_TEST( string, startsWith, string_true,
		EON_TRUE( string( "abcdef" ).startsWith( "abc" ) ) );
	EON_TEST( string, startsWith, string_false,
		EON_FALSE( string( "abcdef" ).startsWith( "abd" ) ) );

	EON_TEST( string, startsWith, char_empty,
		EON_FALSE( string( "" ).startsWith( 'a' ) ) );
	EON_TEST( string, startsWith, char_true,
		EON_TRUE( string( "abcdef" ).startsWith( 'a' ) ) );
	EON_TEST( string, startsWith, char_false,
		EON_FALSE( string( "abcdef" ).startsWith( 'b' ) ) );

	EON_TEST( string, endsWith, string_empty,
		EON_FALSE( string( "" ).endsWith( "def" ) ) );
	EON_TEST( string, endsWith, string_true,
		EON_TRUE( string( "abcdef" ).endsWith( "def" ) ) );
	EON_TEST( string, endsWith, string_false,
		EON_FALSE( string( "abcdef" ).endsWith( "dee" ) ) );

	EON_TEST( string, endsWith, char_empty,
		EON_FALSE( string( "" ).endsWith( 'f' ) ) );
	EON_TEST( string, endsWith, char_true,
		EON_TRUE( string( "abcdef" ).endsWith( 'f' ) ) );
	EON_TEST( string, endsWith, char_false,
		EON_FALSE( string( "abcdef" ).endsWith( 'e' ) ) );

	EON_TEST( string, compare, string_empty_empty,
		EON_EQ( 0, string().compare( string() ) ) );
	EON_TEST( string, compare, string_equal,
		EON_EQ( 0, string( "abcdef" ).compare( string( "abcdef" ) ) ) );
	EON_TEST( string, compare, string_less_faster,
		EON_EQ( -1, string( "abccef" ).compare( string( "abcdef" ) ) ) );
	EON_TEST( string, compare, string_greater_faster,
		EON_EQ( 1, string( "abcdef" ).compare( string( "abccef" ) ) ) );

	EON_NO_TEST( string, operator_lt );
	EON_NO_TEST( string, operator_le );
	EON_NO_TEST( string, operator_gt );
	EON_NO_TEST( string, operator_ge );
	EON_NO_TEST( string, operator_eq );
	EON_NO_TEST( string, operator_ne );




	///////////////////////////////////////////////////////////////////////////
	//
	// Wide-string Support
	//

	EON_TEST( string, string, wstring_empty,
		EON_EQ( string(), string( std::wstring() ) ) );
	EON_TEST( string, string, wstring_ascii,
		EON_EQ( "aB1", string( std::wstring( L"aB1" ) ) ) );
	EON_TEST( string, string, wstring_unicode,
		EON_EQ( u8"€Øá", string( std::wstring( L"€Øá" ) ) ) );

	EON_TEST( string, string, wchar_empty,
		EON_EQ( string(), string( L"" ) ) );
	EON_TEST( string, string, wchar_ascii,
		EON_EQ( "aB1", string( L"aB1" ) ) );
	EON_TEST( string, string, wchar_unicode,
		EON_EQ( u8"€Øá", string( L"€Øá" ) ) );

	string& string::operator=( const std::wstring& stdwstr )
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return *this = conv.to_bytes( stdwstr );
	}
	EON_TEST( string, operator_asgn, wstring_empty,
		EON_EQ( string(), string() = std::wstring( L"" ) ) );
	EON_TEST( string, operator_asgn, wstring_ascii,
		EON_EQ( "aB1", string() = std::wstring( L"aB1" ) ) );
	EON_TEST( string, operator_asgn, wstring_unicode,
		EON_EQ( u8"€Øá", string() = std::wstring( L"€Øá" ) ) );

	string& string::operator=( const wchar_t* cstr )
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return *this = conv.to_bytes( cstr );
	}
	EON_TEST( string, operator_asgn, whar_empty,
		EON_EQ( string(), string() = L"" ) );
	EON_TEST( string, operator_asgn, wchar_ascii,
		EON_EQ( "aB1", string() = L"aB1" ) );
	EON_TEST( string, operator_asgn, wchar_unicode,
		EON_EQ( u8"€Øá", string() = L"€Øá" ) );

	EON_TEST( string, operator_wstring, empty,
		EON_EQ( L"", static_cast<std::wstring>( string() ) ) );
	EON_TEST( string, operator_wstring, ascii,
		EON_EQ( L"aB1", static_cast<std::wstring>( string( "aB1" ) ) ) );
	EON_TEST( string, operator_wstring, unicode,
		EON_EQ( L"€Øá", static_cast<std::wstring>( string( u8"€Øá" ) ) ) );

	std::wstring string::stdwstr() const
	{
		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
		return conv.from_bytes( Bytes );
	}
	EON_TEST( string, stdwstr, empty,
		EON_EQ( L"", string().stdwstr() ) );
	EON_TEST( string, stdwstr, ascii,
		EON_EQ( L"aB1", string( "aB1" ).stdwstr() ) );
	EON_TEST( string, stdwstr, unicode,
		EON_EQ( L"€Øá", string( u8"€Øá" ).stdwstr() ) );

	EON_TEST( string, operator_plsasgn, wstring_empty_empty,
		EON_EQ( string(), string( ) += std::wstring() ) );
	EON_TEST( string, operator_plsasgn, wstring_empty,
		EON_EQ( "abc", string( "abc" ) += std::wstring() ) );
	EON_TEST( string, operator_plsasgn, wstring_ascii,
		EON_EQ( "abcdef", string( "abc" ) += std::wstring( L"def" ) ) );
	EON_TEST( string, operator_plsasgn, wstring_unicode,
		EON_EQ( u8"abc€Øá", string( "abc" ) += std::wstring( L"€Øá" ) ) );




	///////////////////////////////////////////////////////////////////////////
	//
	// String as List
	//

	string& string::push_back( const string& element, const string& separator )
	{
		if( !empty() )
			*this += separator;
		*this += element;
		return *this;
	}
	EON_TEST( string, push_back, empty_empty,
		EON_EQ( "", string().push_back( "" ) ) );
	EON_TEST( string, push_back, empty_element,
		EON_EQ( "abc;", string( "abc" ).push_back( "" ) ) );
	EON_TEST( string, push_back, empty_list,
		EON_EQ( "abc", string( "" ).push_back( "abc" ) ) );
	EON_TEST( string, push_back, basic,
		EON_EQ( "abc;def", string( "abc" ).push_back( "def" ) ) );
	EON_TEST( string, push_back, custom_separator,
		EON_EQ( "abc:-:def", string( "abc" ).push_back( "def", ":-:" ) ) );

	string& string::push_front( const string& element, const string& separator )
	{
		if( !empty() )
			*this = element + separator + *this;
		else
			*this = element;
		return *this;
	}
	EON_TEST( string, push_front, empty_empty,
		EON_EQ( "", string().push_front( "" ) ) );
	EON_TEST( string, push_front, empty_element,
		EON_EQ( ";abc", string( "abc" ).push_front( "" ) ) );
	EON_TEST( string, push_front, empty_list,
		EON_EQ( "abc", string( "" ).push_front( "abc" ) ) );
	EON_TEST( string, push_front, basic,
		EON_EQ( "def;abc", string( "abc" ).push_front( "def" ) ) );
	EON_TEST( string, push_front, custom_separator,
		EON_EQ( "def:-:abc", string( "abc" ).push_front( "def", ":-:" ) ) );

	string& string::orderList( const string& separator )
	{
		auto elements = substr().splitNonsequential<std::set<substring>>( separator.substr() );
		return *this = separator.join( elements );
	}
	EON_TEST( string, orderList, empty,
		EON_EQ( "", string().orderList() ) );
	EON_TEST( string, orderList, singletons,
		EON_EQ( "a;b;c;d", string( "b;d;a;c" ).orderList() ) );
	EON_TEST( string, orderList, duplicates,
		EON_EQ( "a;b;c;d", string( "b;a;d;a;c;a;d" ).orderList() ) );




	///////////////////////////////////////////////////////////////////////////
	//
	// Helpers
	//

	EON_TEST_2STEP( string, _findDecimalSeparator, none,
		std::vector<char_t> digits = EON_CSC( char_t( '1' ), char_t( '2' ), char_t( '3' ), char_t( '4' ), char_t( '5' ) ),
		EON_EQ( 5, string()._findDecimalSeparator( digits, '.' ) ) );
	EON_TEST_2STEP( string, _findDecimalSeparator, first,
		std::vector<char_t> digits = EON_CSC( char_t( '.' ), char_t( '2' ), char_t( '3' ), char_t( '4' ), char_t( '5' ) ),
		EON_EQ( 0, string()._findDecimalSeparator( digits, '.' ) ) );
	EON_TEST_2STEP( string, _findDecimalSeparator, last,
		std::vector<char_t> digits = EON_CSC( char_t( '1' ), char_t( '2' ), char_t( '3' ), char_t( '4' ), char_t( '.' ) ),
		EON_EQ( 4, string()._findDecimalSeparator( digits, '.' ) ) );
	EON_TEST_2STEP( string, _findDecimalSeparator, mid,
		std::vector<char_t> digits = EON_CSC( char_t( '1' ), char_t( '2' ), char_t( '.' ), char_t( '4' ), char_t( '5' ) ),
		EON_EQ( 2, string()._findDecimalSeparator( digits, '.' ) ) );

	//EON_TEST_2STEP( string, _zeroIndexRoundUp, non_zero_index,
	//	std::vector<char_t> digits = EON_CSC( char_t( '1' ), char_t( '2' ), char_t( '3' ), char_t( '4' ), char_t( '5' ) ),
	//	EON_FALSE( _zeroIndexRoundUp( digits, 1 ) );
	//EON_TEST_2STEP( string, _zeroIndexRoundUp, bool,
	//	std::vector<char_t> digits = EON_CSC( char_t( '1' ), char_t( '2' ), char_t( '3' ), char_t( '4' ), char_t( '5' ) ),
	//	EON_TRUE( _zeroIndexRoundUp( digits, 0 ) );
	//EON_CMPTEST_3STEP( string, _zeroIndexRoundUp, str,
	//	std::vector<char_t> digits = EON_CSC( char_t( '1' ), char_t( '2' ), char_t( '3' ), char_t( '4' ), char_t( '5' ) ),
	//	_zeroIndexRoundUp( digits, 0 ),
	//	"192345", string().join( digits ) );
}
