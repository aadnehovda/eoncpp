#include "String.h"
#include <eoninlinetest/InlineTest.h>
#include <cctype>
#include <regex>
#include <unordered_map>


namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// String Assignment Metods
	//

	string& string::assign( const char_t* input, index_t input_length )
	{
		clear();
		reserve( input_length );
		for( index_t i = 0; i < input_length; ++i )
			*this += input[ i ];
		return *this;
	}
	EON_TEST_2STEP( string, utf8str_assign, empty,
		char_t inp[ 1 ],
		EON_EQ( "\0", string().assign( inp, 0 ) ) );
	EON_TEST_3STEP( string, utf8str_assign, nonempty,
		char_t inp[ 1 ],
		inp[ 0 ] = char_t( 'a' ),
		EON_EQ( "a", string().assign( "a", 1 ) ) );

	string& string::assign( const char* input, index_t input_length )
	{
		iterator i( input, input_length );	// Using iterator to scan the raw string for us!
		_assertValidUtf8( i );
		NumChars = i.numSourceChars();
		Bytes.assign( input, input_length );
		return *this;
	}
	EON_TEST( string, cstr_assign, empty,
		EON_EQ( string(), string().assign( "", 0 ) ) );
	EON_TEST( string, cstr_assign, nonempty,
		EON_EQ( "abc", string().assign( "abc", 3 ) ) );

	string& string::assign( index_t copies, char_t input )
	{
		uint32_t bytes;
		auto size = iterator::unicodeToBytes( input, bytes );
		Bytes.reserve( size * copies );
		for( index_t i = 0; i < copies; ++i )
			Bytes.append( (const char*)&bytes, size );
		NumChars = copies;
		return *this;
	}
	EON_TEST( string, charcopies_assign, none,
		EON_EQ( string(), string().assign( static_cast<index_t>( 0 ), static_cast<char_t>( 'a' ) ) ) );
	EON_TEST( string, charcopies_assign, one,
		EON_EQ( "a", string().assign( static_cast<index_t>( 1 ), static_cast<char_t>( 'a' ) ) ) );
	EON_TEST( string, charcopies_assign, many,
		EON_EQ( "aaaaaaaaaa", string().assign( static_cast<index_t>( 10 ), static_cast<char_t>( 'a' ) ) ) );

	string& string::assign( index_t copies, const string& other )
	{
		// Make sure 'other' and 'this' are not the same!
		if( &other.Bytes != &Bytes )
		{
			Bytes.reserve( other.numBytes() * copies );
			for( index_t i = 0; i < copies; ++i )
				Bytes.append( other.Bytes );
			NumChars = other.NumChars * copies;
			return *this;
		}
		else
			return assign( copies, string( other ) );
	}
	EON_TEST( string, stringcopies_assign, none,
		EON_EQ( string(), string().assign( static_cast<index_t>( 0 ), string( "ab" ) ) ) );
	EON_TEST( string, stringcopies_assign, one,
		EON_EQ( "ab", string().assign( static_cast<index_t>( 1 ), string( "ab" ) ) ) );
	EON_TEST( string, stringcopies_assign, many,
		EON_EQ( "ababababab", string().assign( static_cast<index_t>( 5 ), string( "ab" ) ) ) );

	string& string::assign( index_t copies, const std::string& input )
	{
		// Make sure 'other' is not our own buffer!
		if( &input != &Bytes )
		{
			iterator i( input );
			_assertValidUtf8( i );
			Bytes.reserve( input.size() * copies );
			for( index_t i = 0; i < copies; ++i )
				Bytes.append( input );
			NumChars = i.numChar() * copies;
			return *this;
		}
		else
			return assign( copies, std::string( input ) );
	}
	EON_TEST( string, stdstringcopies_assign, none,
		EON_EQ( string(), string().assign( static_cast<index_t>( 0 ), std::string( "ab" ) ) ) );
	EON_TEST( string, stdstringcopies_assign, one,
		EON_EQ( "ab", string().assign( static_cast<index_t>( 1 ), std::string( "ab" ) ) ) );
	EON_TEST( string, stdstringcopies_assign, many,
		EON_EQ( "ababababab", string().assign( static_cast<index_t>( 5 ), std::string( "ab" ) ) ) );

	string& string::assign( index_t copies, const substring& input )
	{
		// Make sure 'input' is not from 'this'!
		if( !input.sameSource( Bytes.c_str() ) )
		{
			_assertValidUtf8( input );
			clear();
			Bytes.reserve( input.numBytes() * copies );
			for( index_t i = 0; i < copies; ++i )
				*this += input;
			return *this;
		}
		else
			return assign( copies, string( input ) );
	}
	EON_TEST( string, assign, substringcopies_none,
		EON_EQ( string(), string().assign( static_cast<index_t>( 0 ), substring( "ab" ) ) ) );
	EON_TEST( string, assign, substringcopies_one,
		EON_EQ( "ab", string().assign( static_cast<index_t>( 1 ), substring( "ab" ) ) ) );
	EON_TEST( string, assign, substringcopies_many,
		EON_EQ( "ababababab", string().assign( static_cast<index_t>( 5 ), substring( "ab" ) ) ) );

	string& string::operator=( const substring& input )
	{
		_assertValidUtf8( input );
		return input.isLowToHigh() ? _assignLowToHigh( input ) : _assignHighToLow( input );
	}
	EON_TEST( string, substring_asgn, empty,
		EON_EQ( string(), string() = substring() ) );
	EON_TEST( string, substring_asgn, low2high,
		EON_EQ( "abc", string() = substring( "abc" ) ) );
	EON_TEST_2STEP( string, substring_asgn, high2low,
		auto inp = substring( "cba" ).highToLow(),
		EON_EQ( "abc", string() = inp ) );

	string& string::_assignHighToLow( const substring& input )
	{
		clear();
		for( auto i = input.begin(); i != input.end(); --i )
			*this += *i;
		return *this;
	}
	EON_NO_TEST( string, _assignHighToLow );

	string& string::_assignLowToHigh( const substring& input )
	{
		NumChars = input.numChars();
		Bytes.assign( input.begin().byteData(), input.numBytes() );
		return *this;
	}
	EON_NO_TEST( string, _assignLowToHigh );

	string& string::operator=( std::string&& input )
	{
		substring sub( input );
		_assertValidUtf8( sub );
		NumChars = sub.numChars();
		Bytes = std::move( input );
		return *this;
	}
	EON_TEST_2STEP( string, move_asgn, basic,
		string inp( "value" ),
		EON_EQ( "value", string() = std::move( inp ) ) );
	EON_TEST_3STEP( string, move_assign, moved,
		string inp( "value" ),
		string dummy = std::move( inp ),
		EON_TRUE( inp.empty() ) );

	string& string::operator=( const std::initializer_list<char_t>& input )
	{
		clear();
		reserve( input.size() );
		for( auto& c : input )
			*this += c;
		return *this;
	}
	EON_TEST( string, charinit_asgn, basic,
		EON_EQ( "abc", string() = EON_CSC( char_t( 'a' ), char_t( 'b' ), char_t( 'c' ) ) ) );

	string& string::operator=( const std::vector<char_t>& input )
	{
		clear();
		reserve( input.size() );
		for( auto& c : input )
			*this += c;
		return *this;
	}
	EON_TEST( string, charvecinit_asgn, basic,
		EON_EQ( "abc", string() = std::vector<char_t>EON_CSC( char_t( 'a' ), char_t( 'b' ), char_t( 'c' ) ) ) );

	string& string::operator=( const std::initializer_list<char>& input )
	{
		clear();
		reserve( input.size() );
		for( auto& chr : input )
			*this += static_cast<byte_t>( chr );
		return *this;
	}
	EON_TEST( string, chrinit_asgn, basic,
		EON_EQ( "abc", string() = EON_CSC( 'a', 'b', 'c' ) ) );

	string& string::operator=( const std::initializer_list<unsigned char>& input )
	{
		clear();
		reserve( input.size() );
		for( auto& chr : input )
			*this += static_cast<byte_t>( chr );
		return *this;
	}
	EON_TEST( string, uchrinit_asgn, basic,
		EON_EQ( "abc", string() = EON_CSC( unsigned char( 'a' ), unsigned char( 'b' ), unsigned char( 'c' ) ) ) );
}
