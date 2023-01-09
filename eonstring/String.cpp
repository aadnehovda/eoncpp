#include "String.h"
#include <eoninlinetest/InlineTest.h>
#include <cctype>
#include <regex>
#include <unordered_map>


namespace eon
{
	const string string::Empty;




	///////////////////////////////////////////////////////////////////////
	//
	// Basic Read-only Methods
	//

	EON_TEST( string, numChars, zero,
		EON_EQ( 0, string().numChars() ) );
	EON_TEST( string, numChars, ASCII,
		EON_EQ( 6, string( "abcdef" ).numChars() ) );
	EON_TEST( string, numChars, UTF8,
		EON_EQ( 6, string( u8"�bcd�f" ).numChars() ) );

	EON_TEST( string, length, zero,
		EON_EQ( 0, string().length() ) );
	EON_TEST( string, length, several,
		EON_EQ( 6, string( "abcdef" ).length() ) );
	EON_TEST( string, length, UTF8,
		EON_EQ( 6, string( u8"�bcd�f" ).length() ) );

	EON_TEST( string, numBytes, zero,
		EON_EQ( 0, string().numBytes() ) );
	EON_TEST( string, numBytes, ASCII,
		EON_EQ( 6, string( "abcdef" ).numBytes() ) );
	EON_TEST( string, numBytes, UTF8,
		EON_EQ( 8, string( u8"�bcd�f" ).numBytes() ) );

	EON_NO_TEST( string, bufferSize );

	EON_TEST( string, empty, empty,
		EON_TRUE( string().empty() ) );
	EON_TEST( string, empty, nonempty,
		EON_FALSE( string( "1" ).empty() ) );

	EON_NO_TEST( string, stdstr );
	EON_NO_TEST( string, c_str );

	EON_TEST( string, byte, ASCII,
		EON_EQ( 'b', string( "abcd" ).byte( 1 ) ) );
	EON_TEST( string, byte, UTF8,
		EON_EQ( -93, string( u8"���" ).byte( 1 ) ) );

	EON_TEST( string, isDoubleQuoted, is,
		EON_TRUE( string( "\"abc\"" ).isDoubleQuoted() ) );
	EON_TEST( string, isDoubleQuoted, partial_left,
		EON_FALSE( string( "\"abc" ).isDoubleQuoted() ) );
	EON_TEST( string, isDoubleQuoted, partial_right,
		EON_FALSE( string( "abc\"" ).isDoubleQuoted() ) );
	EON_TEST( string, isDoubleQuoted, not,
		EON_FALSE( string( "abc" ).isDoubleQuoted() ) );

	EON_TEST_2STEP( string, isDoubleQuoted, substring_is,
		string obj = "ab\"cd\"ef",
		EON_TRUE( obj.isDoubleQuoted( obj.substr( obj.begin() + 2, obj.end() - 2 ) ) ) );
	EON_TEST_2STEP( string, isDoubleQuoted, substring_not,
		string obj = "ab\"cd\"ef",
		EON_FALSE( obj.isDoubleQuoted( obj.substr( obj.begin() + 2, obj.end() - 3 ) ) ) );

	EON_TEST_2STEP( string, isSingleQuoted, substring_is,
		string obj = "ab'cd'ef",
		EON_TRUE( obj.isSingleQuoted( obj.substr( obj.begin() + 2, obj.end() - 2 ) ) ) );
	EON_TEST_2STEP( string, isSingleQuoted, substring_not,
		string obj = "ab'cd'ef",
		EON_FALSE( obj.isSingleQuoted( obj.substr( obj.begin() + 2, obj.end() - 3 ) ) ) );

	EON_TEST( string, blank, blank,
		EON_TRUE( string( "  " ).blank() ) );
	EON_TEST( string, blank, not_blank,
		EON_FALSE( string( " l " ).blank() ) );

	EON_TEST( string, indentationLevel, zero,
		EON_EQ( 0, string( "abc" ).indentationLevel() ) );
	EON_TEST( string, indentationLevel, not_sero,
		EON_EQ( 2, string( "\t\tabc" ).indentationLevel() ) );

	EON_TEST_2STEP( string, characters, ASCII,
		std::vector<char_t> exp EON_CSC( 'a', 'b', 'c' ),
		EON_EQ( exp, string( "abc" ).characters<std::vector<char_t>>() ) );
	EON_TEST_2STEP( string, characters, UTF8,
		std::vector<char_t> exp EON_CSC( 'a', char_t( 0xFF ), 'c' ),
		EON_EQ( exp, string( u8"a�c" ).characters<std::vector<char_t>>() ) );

	EON_TEST_3STEP( string, characters, substr_ASCII,
		std::vector<char_t> exp EON_CSC( 'b', 'c' ),
		string obj = "abcd",
		EON_EQ( exp, obj.characters<std::vector<char_t>>( obj.substr( obj.begin() + 1, obj.end() - 1 ) ) ) );
	EON_TEST_3STEP( string, characters, substr_HighToLow,
		std::vector<char_t> exp EON_CSC( 'c', 'b' ),
		string obj = "abcd",
		EON_EQ( exp, obj.characters<std::vector<char_t>>( obj.substr( obj.begin() + 1, obj.end() - 1 ).highToLow() ) ) );




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
		EON_EQ( u8"���", string( std::wstring( L"���" ) ) ) );

	EON_TEST( string, string, wchar_empty,
		EON_EQ( string(), string( L"" ) ) );
	EON_TEST( string, string, wchar_ascii,
		EON_EQ( "aB1", string( L"aB1" ) ) );
	EON_TEST( string, string, wchar_unicode,
		EON_EQ( u8"���", string( L"���" ) ) );

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
		EON_EQ( u8"���", string() = std::wstring( L"���" ) ) );

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
		EON_EQ( u8"���", string() = L"���" ) );

	EON_TEST( string, operator_wstring, empty,
		EON_EQ( L"", static_cast<std::wstring>( string() ) ) );
	EON_TEST( string, operator_wstring, ascii,
		EON_EQ( L"aB1", static_cast<std::wstring>( string( "aB1" ) ) ) );
	EON_TEST( string, operator_wstring, unicode,
		EON_EQ( L"���", static_cast<std::wstring>( string( u8"���" ) ) ) );

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
		EON_EQ( L"���", string( u8"���" ).stdwstr() ) );

	EON_TEST( string, operator_plsasgn, wstring_empty_empty,
		EON_EQ( string(), string( ) += std::wstring() ) );
	EON_TEST( string, operator_plsasgn, wstring_empty,
		EON_EQ( "abc", string( "abc" ) += std::wstring() ) );
	EON_TEST( string, operator_plsasgn, wstring_ascii,
		EON_EQ( "abcdef", string( "abc" ) += std::wstring( L"def" ) ) );
	EON_TEST( string, operator_plsasgn, wstring_unicode,
		EON_EQ( u8"abc���", string( "abc" ) += std::wstring( L"���" ) ) );




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
		auto elements = substr().splitNonSequential<std::set<substring>>( separator.substr() );
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
