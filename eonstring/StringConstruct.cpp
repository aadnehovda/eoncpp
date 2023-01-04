#include "String.h"
#include <eoninlinetest/InlineTest.h>
#include <cctype>
#include <regex>
#include <unordered_map>


namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// String Construction
	//

	class _ByteParser
	{
	public:
		inline _ByteParser( const char* input, index_t input_length ) noexcept {
			Beg = input; End = input + input_length; }

		inline operator bool() const noexcept { return Beg < End; }
		inline bool fillBytes() noexcept
		{
			for( int i = 0; i < 4 && *this; ++i, ++Beg )
			{
				if( !string_iterator::utf8Decode( State, CodePoint, static_cast<unsigned char>( *Beg ) ) )
				{
					++Beg;
					return true;
				}
				Bytes[ i ] = static_cast<unsigned char>( *Beg );
			}
			return false;
		}
		inline char_t utf8_char() const noexcept { return CodePoint; }
		inline void operator++() noexcept { ++Beg; }
	private:
		const char* Beg{ nullptr };
		const char* End{ nullptr };
		char_t CodePoint{ 0 }, State{ 0 };
		unsigned char Bytes[ 4 ]{ 0, 0, 0, 0 };
	};

	string::string( const char* input, index_t input_length, string non_utf8_substitution ) noexcept
	{
		_ByteParser parser( input, input_length );
		while( parser )
		{
			if( parser.fillBytes() )
				*this += parser.utf8_char();
			else
				*this += non_utf8_substitution;
		}
	}
	EON_TEST( string, cstr_constructor, empty,
		EON_EQ( string(), string( "", 0, "" ) ) );
	EON_TEST( string, cstr_constructor, legal,
		EON_EQ( "a", string( "a", 1, "" ) ) );
	EON_TEST_2STEP( string, cstr_constructor, illegal,
		string obj( "æ", 1, "X" ),
		EON_EQ( "X", obj ) );
	EON_TEST( string, cstr_constructor, legal_long,
		EON_EQ( "abcdefABCDEF", string( "abcdefABCDEF", 12, "" ) ) );
}
