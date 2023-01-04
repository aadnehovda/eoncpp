#include "String.h"
#include <eoninlinetest/InlineTest.h>
#include <cctype>
#include <regex>
#include <unordered_map>


namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// Iterator and Iterator Access
	//

	EON_TEST_2STEP( string, begin, basic,
		string obj( "abcdef" ),
		EON_EQ( 'a', *obj.begin() ) );

	string::iterator string::bytePos( index_t pos, iterator start ) const
	{
		start = _ensureValidStart( start );
		return _ascii() ? _optimizedAsciiBytePos( pos, start ) : _count( pos, start );
	}
	EON_TEST_2STEP( string, bytePos, ascii,
		string obj( "abcdef" ),
		EON_EQ( string( "d" ), *obj.bytePos( 3 ) ) );
	EON_TEST_2STEP( string, bytePos, utf8,
		string obj EON_CSC( char_t( 913 ), char_t( 914 ), char_t( 915 ), char_t( 916 ) ),
		EON_EQ( 915, static_cast<int>( *obj.bytePos( 3 ) ) ) );

	inline string::iterator string::_ensureValidStart( iterator& start ) const
	{
		if( start )
		{
			start.assertSameSource( Bytes.c_str() );
			return start;
		}
		else
			return begin();
	}
	EON_TEST_2STEP( string, _ensureValidStart, valid,
		string obj( "abc" ),
		EON_NO_X( obj._ensureValidStart( ++obj.begin() ) ) );
	EON_TEST_2STEP( string, _ensureValidStart, invalid,
		string obj( "abc" ),
		EON_RAISE( string( "abc" )._ensureValidStart( ++obj.begin() ), WrongSource ) );

	inline string::iterator string::_optimizedAsciiBytePos( index_t pos, iterator& start ) const
	{
		if( start.numByte() > pos || pos >= Bytes.size() )
			return end();
		pos -= start.numByte();
		return iterator( Bytes.c_str(), Bytes.size(), start.numChar() + pos, Bytes.c_str() + start.numChar() + pos );
	}
	EON_TEST_2STEP( string, _optimizedAsciiBytePos, default_start,
		string obj( "abcdef" ),
		EON_EQ( string( "d" ), *obj._optimizedAsciiBytePos( 3, obj.begin() ) ) );
	EON_TEST_2STEP( string, _optimizedAsciiBytePos, later_start,
		string obj( "abcdef" ),
		EON_EQ( string( "d" ), *obj._optimizedAsciiBytePos( 3, ++obj.begin() ) ) );

	inline string::iterator string::_count( index_t pos, iterator& start ) const
	{
		if( start.numByte() > pos || pos >= Bytes.size() )
			return end();
		for( ; start != end(); ++start )
		{
			if( start.numByte() >= pos )
				return start;
		}
		return end();
	}
	EON_TEST_2STEP( string, _count, default_start,
		string obj EON_CSC( char_t( 913 ), char_t( 914 ), char_t( 915 ), char_t( 916 ) ),
		EON_EQ( 915, static_cast<int>( *obj._count( 3, obj.begin() ) ) ) );
	EON_TEST_2STEP( string, _count, later_start,
		string obj EON_CSC( char_t( 913 ), char_t( 914 ), char_t( 915 ), char_t( 916 ) ),
		EON_EQ( 915, static_cast<int>( *obj._count( 3, ++obj.begin() ) ) ) );


	string::iterator string::rebase( const iterator& pos_from_other_string ) const noexcept
	{
		if( pos_from_other_string
			&& pos_from_other_string.numByte() < numBytes()
			&& pos_from_other_string.numChar() < NumChars )
		{
			// The iterator is inside of our range
			return iterator(
				Bytes.c_str(), numBytes(), NumChars, Bytes.c_str() + pos_from_other_string.numByte() );
		}
		return end();
	}
	EON_TEST_2STEP( string, rebase, inside,
		string obj( "abcde" ),
		EON_EQ( string( "h" ), *string( "fghij" ).rebase( obj.begin() + 2 ) ) );
	EON_TEST_3STEP( string, rebase, outside,
		string obj( "abcde" ),
		string other( "xy" ),
		EON_EQ( other.end(), other.rebase( obj.begin() + 3 ) ) );

	string::iterator string::rebaseMoved( const iterator& pos_from_other_string ) const noexcept
	{
		if( pos_from_other_string )
			return iterator(
				Bytes.c_str(),
				numBytes(),
				NumChars,
				Bytes.c_str() + pos_from_other_string.numByte(),
				pos_from_other_string.numChar() );
		else
			return end();
	}
	EON_TEST_2STEP( string, rebaseMoved, inside,
		string obj( "abcde" ),
		EON_EQ( string( "h" ), *string( "fghij" ).rebaseMoved( obj.begin() + 2 ) ) );
	EON_TEST_3STEP( string, rebaseMoved, outside,
		string obj( "abcde" ),
		string other( "xy" ),
		EON_EQ( other.end(), other.rebaseMoved( obj.begin() + 3 ) ) );


	string::iterator string::decodeIterator( const string& encoded_iterator )
	{
		std::regex pattern{ R"(^(\d+):(\d+)$)" };
		std::smatch match;
		if( std::regex_match( encoded_iterator.stdstr(), match, pattern ) )
		{
			return iterator( Bytes.c_str(), Bytes.size(), NumChars, Bytes.c_str() + std::atoi( match[ 1 ].str().c_str() ),
				std::atoi( match[ 2 ].str().c_str() ) );
		}
		else
			return end();
	}
	EON_TEST( string, decodeIterator, inside_ascii,
		EON_EQ( string( "h" ), *string( "fghij" ).decodeIterator( "2:2" ) ) );
	EON_TEST_2STEP( string, decodeIterator, outside_ascii,
		string obj( "abcde" ),
		EON_EQ( obj.end(), obj.decodeIterator( "6:6" ) ) );
	EON_TEST_2STEP( string, decodeIterator, inside_utf8,
		string obj EON_CSC( char_t( 915 ), char_t( 916 ), char_t( 917 ), char_t( 918 ) ),
		EON_EQ( 917, static_cast<int>( *obj.decodeIterator( "4:2" ) ) ) );

	substring string::decodeSubstring( const string& encode_substring )
	{
		std::regex pattern{ R"(^(\d+:\d+)\-(\d+:\d+)$)" };
		std::smatch match;
		if( std::regex_match( encode_substring.stdstr(), match, pattern ) )
			return substring( decodeIterator( match[ 1 ].str() ), decodeIterator( match[ 2 ].str() ) );
		else
			return substring( end() );
	}
	EON_TEST( string, decodeSubstring, inside_ascii,
		EON_EQ( string( "gh" ), string( string( "fghij" ).decodeSubstring( "1:1-3:3" ) ) ) );
	EON_TEST_2STEP( string, decodeSubstring, partial_outside_ascii,
		string obj( "abcde" ),
		EON_EQ( string( "de" ), string( obj.decodeSubstring( "3:3-6:6" ) ) ) );
	EON_TEST_2STEP( string, decodeSubstring, all_outside_ascii,
		string obj( "abcde" ),
		EON_EQ( string(), string( obj.decodeSubstring( "6:6-8:8" ) ) ) );
	EON_TEST_2STEP( string, decodeSubstring, inside_utf8,
		string obj EON_CSC( char_t( 915 ), char_t( 916 ), char_t( 917 ), char_t( 918 ) ),
		EON_EQ( string EON_CSC( char_t( 916 ), char_t( 917 ) ),
			string( obj.decodeSubstring( "2:1-6:4" ) ) ) );
}
