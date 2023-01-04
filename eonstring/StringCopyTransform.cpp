#include "String.h"
#include <eoninlinetest/InlineTest.h>
#include <cctype>
#include <regex>
#include <unordered_map>


namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// Copy and Transform
	//

	EON_TEST_2STEP( string, upper, empty,
		string obj,
		EON_EQ( "", obj.upper( obj.substr() ) ) );
	EON_TEST_2STEP( string, upper, all,
		string obj( "aBcDeFgH" ),
		EON_EQ( "ABCDEFGH", obj.upper( obj.substr() ) ) );
	EON_TEST_2STEP( string, upper, substr,
		string obj( "aBcDeFgH" ),
		EON_EQ( "aBcDEFgH", obj.upper( obj.substr( obj.begin() + 3, obj.end() - 3 ) ) ) );
	EON_TEST_3STEP( string, upper, utf8,
		eon::locale loc( "nb_NO.utf8" ),
		string obj( EON_CSC( char_t( 230 ), char_t( 248 ), char_t( 229 ), char_t( 198 ), char_t( 216 ) ) ),
		EON_EQ( string( EON_CSC( char_t( 198 ), char_t( 216 ), char_t( 197 ), char_t( 198 ), char_t( 216 ) ) ),
			obj.upper( obj.substr(), &loc ) ) );

	EON_TEST_2STEP( string, lower, empty,
		string obj,
		EON_EQ( "", obj.lower( obj.substr() ) ) );
	EON_TEST_2STEP( string, lower, all,
		string obj( "aBcDeFgH" ),
		EON_EQ( "abcdefgh", obj.lower( obj.substr() ) ) );
	EON_TEST_2STEP( string, lower, substr,
		string obj( "aBcDeFgH" ),
		EON_EQ( "aBcdeFgH", obj.lower( obj.substr( obj.begin() + 3, obj.end() - 3 ) ) ) );
	EON_TEST_3STEP( string, lower, utf8,
		eon::locale loc( "nb_NO.utf8" ),
		string obj( EON_CSC( char_t( 230 ), char_t( 248 ), char_t( 229 ), char_t( 198 ), char_t( 216 ) ) ),
		EON_EQ( string( EON_CSC( char_t( 230 ), char_t( 248 ), char_t( 229 ), char_t( 230 ), char_t( 248 ) ) ),
			obj.lower( obj.substr(), &loc ) ) );

	string string::ucFirst( const substring& sub, const eon::locale* custom_locale ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		if( area.numBytes() == 1 )
			return upper( area, custom_locale );
		else
		{
			return string( substr( begin(), area.begin() ) )
				+ string( substr( area.begin(), area.begin() + 1 ) ).upper( custom_locale )
				+ string( substr( area.begin() + 1 ) );
		}
	}
	EON_TEST_2STEP( string, ucFirst, empty,
		string obj,
		EON_EQ( "", obj.ucFirst( obj.substr() ) ) );
	EON_TEST_2STEP( string, ucFirst, all,
		string obj( "abcde" ),
		EON_EQ( "Abcde", obj.ucFirst( obj.substr() ) ) );
	EON_TEST_2STEP( string, ucFirst, substr,
		string obj( "abcde" ),
		EON_EQ( "aBcde", obj.ucFirst( obj.substr( obj.begin() + 1 ) ) ) );

	EON_TEST_2STEP( string, ucWords, empty,
		string obj,
		EON_EQ( "", obj.ucWords( obj.substr() ) ) );
	EON_TEST_2STEP( string, ucWords, all,
		string obj( "abc def ghi jkl" ),
		EON_EQ( "Abc Def Ghi Jkl", obj.ucWords( obj.substr() ) ) );
	EON_TEST_2STEP( string, ucWords, substr,
		string obj( "abc def ghi jkl" ),
		EON_EQ( "abc Def Ghi jkl", obj.ucWords( obj.substr( obj.begin() + 3, obj.end() - 3 ) ) ) );

	EON_TEST_2STEP( string, ucSentences, empty,
		string obj,
		EON_EQ( "", obj.ucSentences( obj.substr() ) ) );
	EON_TEST_2STEP( string, ucSentences, all,
		string obj( "abc def. ghi jkl: mno" ),
		EON_EQ( "Abc def. Ghi jkl: Mno", obj.ucSentences( obj.substr() ) ) );
	EON_TEST_2STEP( string, ucSentences, substr,
		string obj( "abc def. ghi jkl: mno" ),
		EON_EQ( "abc def. Ghi jkl: mno", obj.ucSentences( obj.substr( obj.begin() + 3, obj.end() - 3 ) ) ) );

	EON_TEST_2STEP( string, substr, empty,
		string obj,
		EON_EQ( substring(), obj.substr() ) );
	EON_TEST_2STEP( string, substr, all,
		string obj( "abcde" ),
		EON_EQ( substring( "abcde" ), obj.substr() ) );
	EON_TEST_2STEP( string, substr, start,
		string obj( "abcde" ),
		EON_EQ( substring( "cde" ), obj.substr( obj.begin() + 2 ) ) );
	EON_TEST_2STEP( string, substr, start_end,
		string obj( "abcde" ),
		EON_EQ( substring( "bc" ), obj.substr( obj.begin() + 1, obj.end() - 2 ) ) );
	EON_TEST_2STEP( string, substr, reverse,
		string obj( "abcde" ),
		EON_EQ( "dc", string( obj.substr( obj.end() - 2, obj.begin() + 1 ) ) ) );

	EON_TEST_2STEP( string, substr_indices, basic,
		string obj( "abcde" ),
		EON_EQ( substring( "cde" ), obj.substr( 2, 3 ) ) );


	substring string::slice( int64_t start, int64_t end ) const
	{
		auto s = static_cast<index_t>( start >= 0 ? start : NumChars + start );
		auto e = static_cast<index_t>( end >= 0 ? end : NumChars + end );
		if( s <= e )
			++e;
		else
			--e;
		auto sub = substr();
		return substring( sub.iterator( s ), sub.iterator( e ) );
	}
	EON_TEST_2STEP( string, slice, empty,
		string obj,
		EON_EQ( substring(), obj.slice( 0, -1 ) ) );
	EON_TEST_2STEP( string, slice, basic,
		string obj( "abcde" ),
		EON_EQ( substring( "cd" ), obj.slice( 2, -2 ) ) );
	EON_TEST_2STEP( string, slice, reverse,
		string obj( "abcde" ),
		EON_EQ( "dc", string( obj.slice( -2, 2 ) ) ) );


	EON_TEST( string, beforeFirst, str_none,
		EON_EQ( substring(), string( "abcdefg" ).beforeFirst( "ge" ) ) );
	EON_TEST( string, beforeFirst, str_single,
		EON_EQ( "abc", string( string( "abcdefg" ).beforeFirst( "de" ) ) ) );
	EON_TEST( string, beforeFirst, str_multiple,
		EON_EQ( "abc", string( string( "abcdefdegh" ).beforeFirst( "de" ) ) ) );

	EON_TEST( string, beforeFirst, chr_none,
		EON_EQ( substring(), string( "abcdefg" ).beforeFirst( 'h' ) ) );
	EON_TEST( string, beforeFirst, chr_single,
		EON_EQ( "abc", string( string( "abcdefg" ).beforeFirst( 'd' ) ) ) );
	EON_TEST( string, beforeFirst, chr_multiple,
		EON_EQ( "abc", string( string( "abcdefdgh" ).beforeFirst( 'd' ) ) ) );

	EON_TEST( string, beforeLast, str_none,
		EON_EQ( substring(), string( "abcdefg" ).beforeLast( "ge" ) ) );
	EON_TEST( string, beforeLast, str_single,
		EON_EQ( "abc", string( string( "abcdefg" ).beforeLast( "de" ) ) ) );
	EON_TEST( string, beforeLast, str_multiple,
		EON_EQ( "abcdef", string( string( "abcdefdegh" ).beforeLast( "de" ) ) ) );

	EON_TEST( string, beforeLast, chr_none,
		EON_EQ( substring(), string( "abcdef" ).beforeLast( 'g' ) ) );
	EON_TEST( string, beforeLast, chr_single,
		EON_EQ( "abc", string( string( "abcdef" ).beforeLast( 'd' ) ) ) );
	EON_TEST( string, beforeLast, chr_multiple,
		EON_EQ( "abcdef", string( string( "abcdefdgh" ).beforeLast( 'd' ) ) ) );

	EON_TEST( string, afterFirst, str_single,
		EON_EQ( substring( "fg" ), string( "abcdefg" ).afterFirst( "de" ) ) );
	EON_TEST( string, afterFirst, str_multiple,
		EON_EQ( substring( "fdegh" ), string( "abcdefdegh" ).afterFirst( "de" ) ) );

	EON_TEST( string, afterFirst, chr_none,
		EON_EQ( substring(), string( "abcdefg" ).afterFirst( 'h' ) ) );
	EON_TEST( string, afterFirst, chr_single,
		EON_EQ( substring( "efg" ), string( "abcdefg" ).afterFirst( 'd' ) ) );
	EON_TEST( string, afterFirst, chr_multiple,
		EON_EQ( substring( "efdgh" ), string( "abcdefdgh" ).afterFirst( 'd' ) ) );

	EON_TEST( string, afterLast, str_none,
		EON_EQ( substring(), string( "abcdefg" ).afterLast( "ge" ) ) );
	EON_TEST( string, afterLast, str_single,
		EON_EQ( substring( "fg" ), string( "abcdefg" ).afterLast( "de" ) ) );
	EON_TEST( string, afterLast, str_multiple,
		EON_EQ( substring( "gh" ), string( "abcdefdegh" ).afterLast( "de" ) ) );

	EON_TEST( string, afterLast, chr_none,
		EON_EQ( substring(), string( "abcdef" ).afterLast( 'g' ) ) );
	EON_TEST( string, afterLast, chr_single,
		EON_EQ( substring( "ef" ), string( "abcdef" ).afterLast( 'd' ) ) );
	EON_TEST( string, afterLast, chr_multiple,
		EON_EQ( substring( "gh" ), string( "abcdefdgh" ).afterLast( 'd' ) ) );


	EON_TEST( string, replace, str_empty,
		EON_EQ( "", string().replace( "", "" ) ) );
	EON_TEST( string, replace, str_nothing,
		EON_EQ( "abcdef", string( "abcdef" ).replace( "", "x" ) ) );
	EON_TEST( string, replace, str_single,
		EON_EQ( "abxyef", string( "abcdef" ).replace( "cd", "xy" ) ) );
	EON_TEST( string, replace, str_multiple,
		EON_EQ( "abxyefxyghxyij", string( "abcdefcdghcdij" ).replace( "cd", "xy" ) ) );
	EON_TEST( string, replace, str_blank,
		EON_EQ( "abef", string( "abcdef" ).replace( "cd", "" ) ) );
	EON_TEST_2STEP( string, replace, str_sub,
		string obj( "abcdefcdghcdij" ),
		EON_EQ( "abcdefxyghcdij", obj.replace( "cd", "xy", obj.substr( obj.begin() + 3, obj.end() - 3 ) ) ) );

	EON_TEST( string, replace, chr_empty,
		EON_EQ( "", string().replace( ' ', ' ' ) ) );
	EON_TEST( string, replace, chr_nothing,
		EON_EQ( "abcdef", string( "abcdef" ).replace( ' ', 'x' ) ) );
	EON_TEST( string, replace, chr_single,
		EON_EQ( "abxdef", string( "abcdef" ).replace( 'c', 'x' ) ) );
	EON_TEST( string, replace, chr_multiple,
		EON_EQ( "abxdefxghxij", string( "abcdefcghcij" ).replace( 'c', 'x' ) ) );
	EON_TEST_2STEP( string, replace, chr_sub,
		string obj( "abcdefcghcij" ),
		EON_EQ( "abcdefxghcij", obj.replace( 'c', 'x', obj.substr( obj.begin() + 3, obj.end() - 3 ) ) ) );

	string string::replace( const std::map<string, string>& find_replace, const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto output = _prepOutput( area );
		for( iterator i = area.begin(); i != area.end(); ++i )
		{
			for( auto& elm : find_replace )
			{
				if( substr( i, area.end() ).compareSub( elm.first.substr(), elm.first.numChars() ) == 0 )
				{
					output += elm.second;
					i += elm.first.numChars() - 1;
					goto done;
				}
			}
			output += *i;
		done:
			;
		}
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST( string, replace, str_map_empty,
		EON_EQ( "", string().replace( std::map<string, string>() ) ) );
	EON_TEST( string, replace, str_map_nothing,
		EON_EQ( "abcdabefabgh", string( "abcdabefabgh" ).replace( std::map<string, string>EON_CSC(
			EON_CSC( "ac", "dc" ), EON_CSC( "r2", "d2" ) ) ) ) );
	EON_TEST( string, replace, str_map_single,
		EON_EQ( "abcdagoodfabgh", string( "abcdabefabgh" ).replace( std::map<string, string>EON_CSC(
			EON_CSC( "ac", "dc" ), EON_CSC( "be", "good" ) ) ) ) );
	EON_TEST( string, replace, str_map_multiple,
		EON_EQ( "dccddctadcgh", string( "abcdabefabgh" ).replace( std::map<string, string>EON_CSC(
			EON_CSC( "ab", "dc" ), EON_CSC( "ef", "ta" ) ) ) ) );
	EON_TEST( string, replace, str_map_blank,
		EON_EQ( "cdefgh", string( "abcdabefabgh" ).replace( std::map<string, string>EON_CSC(
			EON_CSC( "ab", "" ), EON_CSC( "r2", "d2" ) ) ) ) );
	EON_TEST_2STEP( string, replace, str_map_sub,
		string obj( "abcdabefabgh" ),
		EON_EQ( "abcddcefabgh", obj.replace( std::map<string, string>EON_CSC( EON_CSC( "ab", "dc" ) ), obj.substr(
			obj.begin() + 3, obj.end() - 3 ) ) ) );

	string string::replace( const std::map<char_t, char_t>& find_replace, const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto output = _prepOutput( area );
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			auto found = find_replace.find( *i );
			if( found != find_replace.end() )
				output += found->second;
			else
				output += *i;
		}
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST( string, replace, chr_map_empty,
		EON_EQ( "", string().replace( std::map<char_t, char_t>() ) ) );
	EON_TEST( string, replace, chr_map_nothing,
		EON_EQ( "abcdaefagh", string( "abcdaefagh" ).replace( std::map<char_t, char_t>EON_CSC(
			EON_CSC( 'r', 's' ), EON_CSC( 'x', 'y' ) ) ) ) );
	EON_TEST( string, replace, chr_map_single,
		EON_EQ( "xbcdxefxgh", string( "abcdaefagh" ).replace( std::map<char_t, char_t>EON_CSC(
			EON_CSC( 'a', 'x' ), EON_CSC( 'x', 'y' ) ) ) ) );
	EON_TEST( string, replace, chr_map_multiple,
		EON_EQ( "xbcdxyfxgh", string( "abcdaefagh" ).replace( std::map<char_t, char_t>EON_CSC(
			EON_CSC( 'a', 'x' ), EON_CSC( 'e', 'y' ) ) ) ) );
	EON_TEST_2STEP( string, replace, chr_map_sub,
		string obj( "abcdaefagh" ),
		EON_EQ( "abcdxefagh", obj.replace( std::map<char_t, char_t>EON_CSC( EON_CSC( 'a', 'x' ) ), obj.substr(
			obj.begin() + 3, obj.end() - 3 ) ) ) );


	string string::escape( const substring& sub ) const
	{
		static std::unordered_map<char_t, char_t> singletons{
			{ '\'', nochar }, { '"', '"' }, { '?', nochar }, { '\\', '\\' },
			{ '\a', 'a' }, { '\b', 'b' }, { '\f', 'f' }, { '\n', 'n' },
			{ '\r', 'r' }, { '\t', nochar }, { '\v', 'v' } };
		return _escape( sub, singletons );
	}
	EON_TEST_3STEP( string, escape, mix,
		string input( u8"a\tb\nc?d\u0600e\"f\'g\\h" ),
		string output = input.escape(),
		EON_EQ( "a\tb\\nc?d\\u0600;e\\\"f'g\\\\h", output ) );

	string string::_escape( const substring& sub, const std::unordered_map<char_t, char_t>& singletons ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		return _escape( area, _prepOutput( area ), singletons );
	}
	EON_TEST_3STEP( string, _escape, lowToHigh,
		string input( "abac" ),
		string output = input._escape( input.substr(), EON_CSC( EON_CSC( char_t( 'a' ), char_t( 'x' ) ) ) ),
		EON_EQ( "\\xb\\xc", output ) );
	EON_TEST_3STEP( string, _escape, highToLow,
		string input( "abac" ),
		string output = input._escape( input.substr().highToLow(), EON_CSC( EON_CSC( char_t( 'a' ), char_t( 'x' ) ) ) ),
		EON_EQ( "\\xb\\xc", output ) );

	inline void _escapeFound( string& output, char_t in, char_t out )
	{
		if( out != nochar )
			output << BackSlashChr << out;
		else
			output << in;
	}
	EON_TEST_3STEP( string, _escapeFound, nochar,
		string output,
		_escapeFound( output, 'x', nochar ),
		EON_EQ( "x", output ) );
	EON_TEST_3STEP( string, _escapeFound, basic,
		string output,
		_escapeFound( output, 'x', 'y' ),
		EON_EQ( "\\y", output ) );

	inline void _escapeNonprintable( string& output, const string::iterator& in, const substring& area )
	{
		static const char_t divisor[]{ 0x01000000, 0x00010000, 0x00000100, 0x00000001 };
		char_t remainder = *in;
		output << BackSlashChr;
		if( *in == 0 )	// Null gets escaped as octal!
			output << std::to_string( static_cast<int>( *in ) );
		else
		{
			output << "u";
			bool got_something = false;
			for( int i = 0; i < 4; ++i )
			{
				auto byte = static_cast<byte_t>( remainder / divisor[ i ] );
				remainder %= divisor[ i ];
				if( !got_something && byte != 0 )
					got_something = true;
				if( got_something )
					output << byteToHex( byte );
			}
			if( in < area.last() )
				output << ";";
		}
	}
	EON_TEST_3STEP( string, _escapeNonprintable, single_byte,
		EON_DECLARE( string, output, input( "a" ) ),
		_escapeNonprintable( output, input.begin(), input.substr() ),
		EON_EQ( "\\u61", output ) );
	EON_TEST_3STEP( string, _escapeNonprintable, four_bytes,
		EON_DECLARE( string, output, input( char_t( 0xE007F ) ) ),
		_escapeNonprintable( output, input.begin(), input.substr() ),
		EON_EQ( "\\u0E007F", output ) );

	inline void _escapeChar(
		string::iterator& c, const substring& area, string& output, const std::unordered_map<char_t, char_t>& singletons )
	{
		auto& chars = Characters::get();
		auto found = singletons.find( *c );
		if( found != singletons.end() )
			_escapeFound( output, *c, found->second );
		else if( chars.isOther( *c ) )	// Non-printable gets output as Unicode escapes
			_escapeNonprintable( output, c, area );

		else
			output += *c;
	}
	EON_TEST_3STEP( string, _escapeChar, basic,
		EON_DECLARE( string, output, input( "a" ) ),
		_escapeChar( input.begin(), input.substr(), output, EON_CSC( EON_CSC( 'a', 'x' ) ) ),
		EON_EQ( "\\x", output ) );
	EON_TEST_3STEP( string, _escapeChar, non_printable,
		EON_DECLARE( string, output, input( u8"\u0600" ) ),
		_escapeChar( input.begin(), input.substr(), output, EON_CSC( EON_CSC( 'a', 'x' ) ) ),
		EON_EQ( "\\u0600", output ) );
	EON_TEST_3STEP( string, _escapeChar, no_escape,
		EON_DECLARE( string, output, input( "b" ) ),
		_escapeChar( input.begin(), input.substr(), output, EON_CSC( EON_CSC( 'a', 'x' ) ) ),
		EON_EQ( "b", output ) );

	string string::_escape(
		const substring& area, string output, const std::unordered_map<char_t, char_t>& singletons ) const
	{
		for( string::iterator c = area.begin(); c != area.end(); ++c )
			_escapeChar( c, area, output, singletons );
		if( area.end() < end() )
			output += substr( area.end() );
		return output;
	}
	EON_TEST_3STEP( string, _escape, empty,
		string input,
		auto output = input._escape( input.substr(), "", EON_CSC( EON_CSC( 'a', 'x' ) ) ),
		EON_EQ( "", output ) );
	EON_TEST_3STEP( string, _escape, single_plain,
		string input( "b" ),
		auto output = input._escape( input.substr(), "", EON_CSC( EON_CSC( 'a', 'x' ) ) ),
		EON_EQ( "b", output ) );
	EON_TEST_3STEP( string, _escape, single_escape,
		string input( "a" ),
		auto output = input._escape( input.substr(), "", EON_CSC( EON_CSC( 'a', 'x' ) ) ),
		EON_EQ( "\\x", output ) );
	EON_TEST_3STEP( string, _escape, multiple_mix,
		string input( "abacad" ),
		auto output = input._escape( input.substr(), "", EON_CSC( EON_CSC( 'a', 'x' ) ) ),
		EON_EQ( "\\xb\\xc\\xd", output ) );


	string string::escapeAll( const substring& sub ) const
	{
		static std::unordered_map<char_t, char_t> singletons{
			{ '\'', '\'' }, { '"', '"' }, { '?', '?' }, { '\\', '\\' },
			{ '\a', 'a' }, { '\b', 'b' }, { '\f', 'f' }, { '\n', 'n' },
			{ '\r', 'r' }, { '\t', 't' }, { '\v', 'v' } };
		return _escape( sub, singletons );
	}
	EON_TEST_3STEP( string, escapeAll, mix,
		string input( u8"a\tb\nc?d\u0600e\"f\'g\\h" ),
		string output = input.escapeAll(),
		EON_EQ( "a\\tb\\nc\\?d\\u0600;e\\\"f\\'g\\\\h", output ) );

	string string::unescape( const substring& sub ) const
	{
		static std::unordered_map<char_t, char_t> singletons{
			{ '\'', '\'' }, { '"', '"' }, { '?', '?' }, { '\\', '\\' },
			{ 'a', '\a' }, { 'b', '\b' }, { 'f', '\f' }, { 'n', '\n' },
			{ 'r', '\r' }, { 't', '\t' }, { 'v', '\v' } };
		return _unescape( sub, singletons );
	}
	EON_TEST_3STEP( string, unescape, mix,
		string input( "a\tb\\nc?d\\u0600;e\\\"f'g\\\\h" ),
		string output = input.unescape(),
		EON_EQ( string( u8"a\tb\nc?d\u0600e\"f\'g\\h" ), output ) );
	EON_TEST_3STEP( string, unescape, all,
		string input( "a\\tb\\nc\\?d\\u0600;e\\\"f\\'g\\\\h" ),
		string output = input.unescape(),
		EON_EQ( string( u8"a\tb\nc?d\u0600e\"f\'g\\h" ), output ) );

	string string::_unescape( const substring& sub, const std::unordered_map<char_t, char_t>& singletons ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		return _unescape( area, _prepOutput( area ), singletons );
	}

	inline bool _unescapeSingleton(
		string::iterator& c, string& output, const std::unordered_map<char_t, char_t>& singletons )
	{
		auto found = singletons.find( *c );
		if( found == singletons.end() )
			return false;
		output += found->second;
		return true;
	}
	EON_TEST_2STEP( string, _unescapeSingleton, nfound_bool,
		EON_DECLARE( string, output, input( "a" ) ),
		EON_FALSE( _unescapeSingleton( input.begin(), output, EON_CSC( EON_CSC( 'x', 'a' ) ) ) ) );
	EON_TEST_3STEP( string, _unescapeSingleton, nfound_output,
		EON_DECLARE( string, output, input( "a" ) ),
		_unescapeSingleton( input.begin(), output, EON_CSC( EON_CSC( 'x', 'a' ) ) ),
		EON_EQ( "", output ) );
	EON_TEST_2STEP( string, _unescapeSingleton, found_bool,
		EON_DECLARE( string, output, input( "x" ) ),
		EON_TRUE( _unescapeSingleton( input.begin(), output, EON_CSC( EON_CSC( 'x', 'a' ) ) ) ) );
	EON_TEST_3STEP( string, _unescapeSingleton, found_output,
		EON_DECLARE( string, output, input( "x" ) ),
		_unescapeSingleton( input.begin(), output, EON_CSC( EON_CSC( 'x', 'a' ) ) ),
		EON_EQ( "a", output ) );

	inline bool _unescapeUnicode( string::iterator& c, string& output )
	{
		if( ( *c != 'u' && *c != 'U' ) || c + 1 == c.getEnd() || !isHexDigit( *( c + 1 ) ) )
			return false;
		char_t val{ 0 };
		for( int i = 0; ; ++i )
		{
			val <<= 4;
			val += hexToNum( static_cast<byte_t>( *++c ) );
			if( c + 1 == c.getEnd() )
				break;
			else
			{
				if( *( c + 1 ) == ';' )
				{
					++c;
					break;
				}
				else if( !isHexDigit( *( c + 1 ) ) || i == 8 )
					break;
			}
		}
		output += val;
		return true;
	}
	EON_TEST_2STEP( string, _unescapeUnicode, not_unicode_bool,
		EON_DECLARE( string, output, input( "a" ) ),
		EON_FALSE( _unescapeUnicode( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeUnicode, not_unicode_str,
		EON_DECLARE( string, output, input( "a" ) ),
		_unescapeUnicode( input.begin(), output ),
		EON_EQ( "", output ) );
	EON_TEST_2STEP( string, _unescapeUnicode, unicode_bool,
		EON_DECLARE( string, output, input( "u0600" ) ),
		EON_TRUE( _unescapeUnicode( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeUnicode, unicode_str,
		EON_DECLARE( string, output, input( "u0600" ) ),
		_unescapeUnicode( input.begin(), output ),
		EON_EQ( string( u8"\u0600" ), output ) );
	EON_TEST_2STEP( string, _unescapeUnicode, largeunicode_bool,
		EON_DECLARE( string, output, input( "u01D78D" ) ),
		EON_TRUE( _unescapeUnicode( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeUnicode, largeunicode_str,
		EON_DECLARE( string, output, input( "u01D78D" ) ),
		_unescapeUnicode( input.begin(), output ),
		EON_EQ( string( char_t( 120717 ) ), output ) );

	inline int _parseUnescapeHex( string::iterator& c, char_t& parsed )
	{
		int num_bytes = 0;
		for( int i = 0; ; ++i )
		{
			parsed <<= 4;
			parsed += hexToNum( static_cast<byte_t>( *++c ) );
			if( i % 2 == 1 )
				++num_bytes;
			if( c + 1 == c.getEnd() )
				break;
			else
			{
				if( *( c + 1 ) == ';' )
				{
					++c;
					break;
				}
				else if( !isHexDigit( *( c + 1 ) ) || i == 8 )
					break;
			}
		}
		return num_bytes;
	}
	inline char_t _decodeUnescapeHex( char_t parsed, int num_bytes )
	{
		char_t decoded{ 0 };
		static const byte_t first_byte_mask[ 4 ]{ 0b01111111, 0b00011111, 0b00001111, 0b00000111 };
		byte_t* bytes = (byte_t*)&parsed;
		decoded = ( bytes[ num_bytes - 1 ] & first_byte_mask[ num_bytes - 1 ] ) << ( ( num_bytes - 1 ) * 6 );
		for( --num_bytes; num_bytes > 0; --num_bytes )
			decoded += ( bytes[ num_bytes - 1 ] & 0b00111111 ) << ( ( num_bytes - 1 ) * 6 );
		return decoded;
	}
	inline bool _unescapeHex( string::iterator& c, string& output )
	{
		if( ( *c != 'x' && *c != 'X' ) || c + 1 == c.getEnd() || !isHexDigit( *( c + 1 ) ) )
			return false;
		char_t parsed{ 0 };
		auto num_bytes = _parseUnescapeHex( c, parsed );
		output += _decodeUnescapeHex( parsed, num_bytes );
		return true;
	}
	EON_TEST_2STEP( string, _unescapeHex, not_hex_bool,
		EON_DECLARE( string, output, input( "a" ) ),
		EON_FALSE( _unescapeHex( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeHex, not_hex_str,
		EON_DECLARE( string, output, input( "a" ) ),
		_unescapeHex( input.begin(), output ),
		EON_EQ( "", output ) );
	EON_TEST_2STEP( string, _unescapeHex, hex_bool,
		EON_DECLARE( string, output, input( "x6B" ) ),
		EON_TRUE( _unescapeHex( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeHex, hex_str,
		EON_DECLARE( string, output, input( "x6B" ) ),
		_unescapeHex( input.begin(), output ),
		EON_EQ( "k", output ) );
	EON_TEST_2STEP( string, _unescapeHex, unicode_bool,
		EON_DECLARE( string, output, input( "xD880" ) ),
		EON_TRUE( _unescapeHex( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeHex, unicode_str,
		EON_DECLARE( string, output, input( "xD880" ) ),
		_unescapeHex( input.begin(), output ),
		EON_EQ( string( u8"\u0600" ), output ) );
	EON_TEST_2STEP( string, _unescapeHex, largehex_bool,
		EON_DECLARE( string, output, input( "xF09D9E8D" ) ),
		EON_TRUE( _unescapeHex( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeHex, largehex_str,
		EON_DECLARE( string, output, input( "xF09D9E8D" ) ),
		_unescapeHex( input.begin(), output ),
		EON_EQ( string( char_t( 120717 ) ), output ) );

	inline bool _unescapeOctal( string::iterator& c, string& output )
	{
		if( !isOctalDigit( *c ) )
			return false;
		uint32_t val{ 0 };
		bool have_val = false;
		while( true )
		{
			val <<= 3;
			val += *c - '0';
			have_val = true;
			if( c + 1 == c.getEnd() || !isOctalDigit( *( c + 1 ) ) )
			{
				if( have_val )
				{
					output += static_cast<char_t>( val );
					have_val = false;
				}
				break;
			}
			++c;
		}
		return true;
	}
	EON_TEST_2STEP( string, _unescapeOctal, not_octal_bool,
		EON_DECLARE( string, output, input( "a" ) ),
		EON_FALSE( _unescapeOctal( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeOctal, not_octal_str,
		EON_DECLARE( string, output, input( "a" ) ),
		_unescapeOctal( input.begin(), output ),
		EON_EQ( "", output ) );
	EON_TEST_2STEP( string, _unescapeOctal, octal_bool,
		EON_DECLARE( string, output, input( "141" ) ),
		EON_TRUE( _unescapeOctal( input.begin(), output ) ) );
	EON_TEST_3STEP( string, _unescapeOctal, octal_str,
		EON_DECLARE( string, output, input( "141" ) ),
		_unescapeOctal( input.begin(), output ),
		EON_EQ( "a", output ) );

	inline void _unescapeChar( string::iterator& c, string& output, const std::unordered_map<char_t, char_t>& singletons )
	{
		auto c1 = c + 1;
		if( *c == BackSlashChr && c1 != c.getEnd() )
		{
			if( _unescapeSingleton( c1, output, singletons )
				|| _unescapeUnicode( c1, output )
				|| _unescapeHex( c1, output )
				|| _unescapeOctal( c1, output ) )
			{
				c = c1;
				return;
			}
		}
		output += *c;
	}

	string string::_unescape(
		const substring& area, string output, const std::unordered_map<char_t, char_t>& singletons ) const
	{
		for( string::iterator c = area.begin(); c != area.end(); ++c )
			_unescapeChar( c, output, singletons );
		return output;
	}
	EON_TEST_3STEP( string, _unescape, empty,
		EON_DECLARE( string, output, input ),
		output = input._unescape( input.substr(), output, EON_CSC( EON_CSC( 'z', 'a' ) ) ),
		EON_EQ( "", output ) );
	EON_TEST_3STEP( string, _unescape, not_escaped,
		EON_DECLARE( string, output, input( "abcdef" ) ),
		output = input._unescape( input.substr(), output, EON_CSC( EON_CSC( 'z', 'a' ) ) ),
		EON_EQ( "abcdef", output ) );
	EON_TEST_3STEP( string, _unescape, singleton,
		EON_DECLARE( string, output, input( "\\zbcdef" ) ),
		output = input._unescape( input.substr(), output, EON_CSC( EON_CSC( 'z', 'a' ) ) ),
		EON_EQ( "abcdef", output ) );
	EON_TEST_3STEP( string, _unescape, unicode,
		EON_DECLARE( string, output, input( "\\u61;bcdef" ) ),
		output = input._unescape( input.substr(), output, EON_CSC( EON_CSC( 'z', 'a' ) ) ),
		EON_EQ( "abcdef", output ) );
	EON_TEST_3STEP( string, _unescape, hex,
		EON_DECLARE( string, output, input( "\\x61;bcdef" ) ),
		output = input._unescape( input.substr(), output, EON_CSC( EON_CSC( 'z', 'a' ) ) ),
		EON_EQ( "abcdef", output ) );
	EON_TEST_3STEP( string, _unescape, octal,
		EON_DECLARE( string, output, input( "\\141bcdef" ) ),
		output = input._unescape( input.substr(), output, EON_CSC( EON_CSC( 'z', 'a' ) ) ),
		EON_EQ( "abcdef", output ) );


	EON_TEST_2STEP( string, toHtml, all_ascii_only,
		string input( "abc" ),
		EON_EQ( "abc", input.toHtml() ) );
	EON_TEST_2STEP( string, toHtml, all_mix,
		string input( u8"a\u00E6b\tc" ),
		EON_EQ( "a&#230;b&#9;c", input.toHtml() ) );

	inline bool _isPrintableAscii( char_t c ) noexcept {
		return c == '\n' || ( c >= ' ' && c < 126 ); }
	EON_NO_TEST( string, _isPrintableAscii );

	inline void _toHtml( char_t c, string& output )	{
		output += "&#" + string( static_cast<int32_t>( c ) ) + SemiColonChr; }
	EON_NO_TEST( string, _toHtml );

	string string::toHtml( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto output = _prepOutput( area );
		for( auto c : area )
		{
			if( _isPrintableAscii( c ) )
				output += c;
			else
				_toHtml( c, output );
		}
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST_2STEP( string, toHtml, sub_ascii_only,
		string input( "abcdef" ),
		EON_EQ( "abcdef", input.toHtml( input.substr( input.begin() + 2, input.last() - 1 ) ) ) );
	EON_TEST_2STEP( string, toHtml, sub_mix,
		string input( u8"a\u00E6b\tc\vd\be\rf" ),
		EON_EQ( string( u8"a\u00E6b&#9;c&#11;d&#8;e\rf" ),
			input.toHtml( input.substr( input.begin() + 2, input.last() - 1 ) ) ) );


	EON_TEST_2STEP( string, fromHtml, all_ascii_only,
		string input( "abc" ),
		EON_EQ( "abc", input.fromHtml() ) );
	EON_TEST_2STEP( string, fromHtml, all_mix,
		string input( "a&#230;b&#9;c" ),
		EON_EQ( string( u8"a\u00E6b\tc" ), input.fromHtml() ) );

	inline bool _canBeHtml( string::iterator& i, const substring& area ) noexcept {
		return *i == '&' && ( i + 1 ) != area.end() && *( i + 1 ) == '#'; }
	EON_NO_TEST( string, _canBeHtml );

	inline bool _decodeHtml( string::iterator& i, const substring& area, string& output )
	{
		auto end = substring( i, area.end() ).findFirst( SemiColonChr );
		if( end )
		{
			auto num = substring( i + 2, end.begin() );
			if( !num.findFirstOtherThan( charcat::number_ascii_digit ) )
			{
				output += static_cast<char_t>( num.toUInt32() );
				i = end.begin();
				return true;
			}
		}
		return false;
	}
	EON_NO_TEST( string, _decodeHtml );

	string string::fromHtml( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto output = _prepOutput( area );
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			if( _canBeHtml( i, area ) )
			{
				if( _decodeHtml( i, area, output ) )
					continue;
			}
			output += *i;
		}
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST_2STEP( string, fromHtml, sub_ascii_only,
		string input( "abcdef" ),
		EON_EQ( "abcdef", input.fromHtml( input.substr( input.begin() + 2, input.last() - 1 ) ) ) );
	EON_TEST_2STEP( string, fromHtml, sub_mix,
		string input( u8"a\u00E6b&#9;c&#11;d&#8;e\rf" ),
		EON_EQ( string( u8"a\u00E6b\tc\vd\be\rf" ),
			input.fromHtml( input.substr( input.begin() + 2, input.last() - 1 ) ) ) );


	EON_TEST( string, doubleQuote, all,
		EON_EQ( "\" text\"", string( " text" ).doubleQuote() ) );
	EON_TEST( string, doubleQuote, all_prequoted,
		EON_EQ( "\" text\"", string( "\" text\"" ).doubleQuote() ) );

	string string::doubleQuote( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto output = _prepOutput( area );
		if( !isDoubleQuoted( area ) )
			output += DblQuoteChr + string( area ) + DblQuoteChr;
		else
			output += area;
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST_2STEP( string, doubleQuote, substr,
		string input( "one two three" ),
		EON_EQ( "one \"two \"three", input.doubleQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );
	EON_TEST_2STEP( string, doubleQuote, substr_prequoted,
		string input( "one \"two \"three" ),
		EON_EQ( "one \"two \"three", input.doubleQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );
	EON_TEST_2STEP( string, doubleQuote, substr_singlequoted,
		string input( "one 'two 'three" ),
		EON_EQ( "one \"'two '\"three", input.doubleQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );

	EON_TEST( string, singleQuote, all,
		EON_EQ( "' text'", string( " text" ).singleQuote() ) );
	EON_TEST( string, singleQuote, all_prequoted, EON_EQ( "' text'", string( "' text'" ).singleQuote() ) );

	string string::singleQuote( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto output = _prepOutput( area );
		if( !isSingleQuoted( area ) )
			output += SglQuoteChr + string( area ) + SglQuoteChr;
		else
			output += area;
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST_2STEP( string, singleQuote, substr,
		string input( "one two three" ),
		EON_EQ( "one 'two 'three", input.singleQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );
	EON_TEST_2STEP( string, singleQuote, substr_prequoted,
		string input( "one 'two 'three" ),
		EON_EQ( "one 'two 'three", input.singleQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );
	EON_TEST_2STEP( string, singleQuote, substr_doublequoted,
		string input( "one \"two \"three" ),
		EON_EQ( "one '\"two \"'three", input.singleQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );

	EON_TEST( string, unQuote, all_unquoted,
		EON_EQ( " text", string( " text" ).unQuote() ) );
	EON_TEST( string, unQuote, all_doublequoted,
		EON_EQ( " text", string( "\" text\"" ).unQuote() ) );
	EON_TEST( string, unQuote, all_singlequoted,
		EON_EQ( " text", string( "' text'" ).unQuote() ) );

	string string::unQuote( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto output = _prepOutput( area );
		if( isDoubleQuoted( area ) || isSingleQuoted( area ) )
			output += substr( area.begin() + 1, area.end() - 1 );
		else
			output += area;
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST_2STEP( string, unQuote, substr_unquoted,
		string input( "one two three" ),
		EON_EQ( "one two three", input.unQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );
	EON_TEST_2STEP( string, unQuote, substr_doublequoted,
		string input( "one \"two \"three" ),
		EON_EQ( "one two three", input.unQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );
	EON_TEST_2STEP( string, unQuote, substr_singlequoted,
		string input( "one 'two 'three" ),
		EON_EQ( "one two three", input.unQuote( input.substr( input.begin() + 4, input.end() - 5 ) ) ) );


	EON_TEST( string, reverse, all,
		EON_EQ( "fedcba", string( "abcdef" ).reverse() ) );

	string string::reverse( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto revs = sub.highToLow();
		auto output = _prepOutput( area );
		for( auto i = revs.begin(); i != revs.end(); --i )
			output += *i;
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST_2STEP( string, reverse, substr,
		string input( "abcdef" ),
		EON_EQ( "aedcbf", input.reverse( input.substr( input.begin() + 1, input.end() - 1 ) ) ) );


	EON_TEST( string, padLeft, shorter,
		EON_EQ( "abcdef", string( "abcdef" ).padLeft( 4 ) ) );
	EON_TEST( string, padLeft, at_size,
		EON_EQ( "abcdef", string( "abcdef" ).padLeft( 6 ) ) );
	EON_TEST( string, padLeft, longer,
		EON_EQ( "  abcdef", string( "abcdef" ).padLeft( 8 ) ) );
	EON_TEST( string, padLeft, custom_fill,
		EON_EQ( "xxabcdef", string( "abcdef" ).padLeft( 8, 'x' ) ) );

	EON_TEST( string, padRight, shorter,
		EON_EQ( "abcdef", string( "abcdef" ).padRight( 4 ) ) );
	EON_TEST( string, padRight, at_size,
		EON_EQ( "abcdef", string( "abcdef" ).padRight( 6 ) ) );
	EON_TEST( string, padRight, longer,
		EON_EQ( "abcdef  ", string( "abcdef" ).padRight( 8 ) ) );
	EON_TEST( string, padRight, custom_fill,
		EON_EQ( "abcdefxx", string( "abcdef" ).padRight( 8, 'x' ) ) );

	string string::padLeftAndRight( index_t target_size, char_t fill ) const
	{
		if( numChars() >= target_size )
			return *this;
		auto diff = target_size - numChars(), half = diff / 2;
		return string( half, fill ) + *this + string( diff - half, fill );
	}
	EON_TEST( string, padLeftAndRight, shorter,
		EON_EQ( "abcdef", string( "abcdef" ).padLeftAndRight( 4 ) ) );
	EON_TEST( string, padLeftAndRight, at_size,
		EON_EQ( "abcdef", string( "abcdef" ).padLeftAndRight( 6 ) ) );
	EON_TEST( string, padLeftAndRight, longer,
		EON_EQ( " abcdef ", string( "abcdef" ).padLeftAndRight( 8 ) ) );
	EON_TEST( string, padLeftAndRight, longer_uneven,
		EON_EQ( " abcdef  ", string( "abcdef" ).padLeftAndRight( 9 ) ) );
	EON_TEST( string, padLeftAndRight, custom_fill,
		EON_EQ( "xabcdefx", string( "abcdef" ).padLeftAndRight( 8, 'x' ) ) );


	struct IndentLines
	{
		inline IndentLines( const string& raw, index_t indentation_level, char_t indentation_char )
		{
			Lines = raw.splitSequential<std::list<substring>>( NewlineChr );
			CurLine = Lines.begin();
			IndentationLevel = indentation_level;
			IndentationChar = indentation_char;
			CurIndentation = CurLine != Lines.end() ? CurLine->indentationLevel( IndentationChar ) : 0;
		}

		inline void indentFirstLine( string& output )
		{
			if( CurIndentation == IndentationLevel )
			{
				output += *CurLine;
				++CurLine;
				FirstLine = false;
			}
		}

		inline void indentLines( string& output )
		{
			for( ; CurLine != Lines.end(); ++CurLine )
			{
				if( FirstLine )
					FirstLine = false;
				else
					output += NewlineChr;
				if( !CurLine->empty() )
					output += string( IndentationLevel, IndentationChar ) + *CurLine;
			}
		}

		std::list<substring> Lines;
		std::list<substring>::iterator CurLine;
		char_t IndentationChar{ TabChr };
		bool FirstLine{ true };
		index_t IndentationLevel{ 0 };
		index_t CurIndentation{ 0 };
	};

	string string::indentLines( index_t indentation_level, char_t indentation_char ) const
	{
		IndentLines lines( *this, indentation_level, indentation_char );
		string output;
		lines.indentFirstLine( output );
		lines.indentLines( output );
		return output;
	}
	EON_TEST( string, indentlines, empty,
		EON_EQ( "", string().indentLines( 2 ) ) );
	EON_TEST( string, indentlines, single_line,
		EON_EQ( "\t\tone two three", string( "one two three" ).indentLines( 2 ) ) );
	EON_TEST( string, indentlines, single_line_custom,
		EON_EQ( "  one two three", string( "one two three" ).indentLines( 2, ' ' ) ) );
	EON_TEST( string, indentlines, multiline,
		EON_EQ( "\t\tone two\n\t\tthree four\n\t\tfive six", string( "one two\nthree four\nfive six" ).indentLines( 2 ) ) );
	EON_TEST( string, indentlines, multiline_first_indented,
		EON_EQ( "\t\tone two\n\t\tthree four\n\t\tfive six",
			string( "\t\tone two\nthree four\nfive six" ).indentLines( 2 ) ) );
	EON_TEST( string, indentlines, multiline_second_indented,
		EON_EQ( "\t\tone two\n\t\t\t\tthree four\n\t\tfive six",
			string( "one two\n\t\tthree four\nfive six" ).indentLines( 2 ) ) );
}
