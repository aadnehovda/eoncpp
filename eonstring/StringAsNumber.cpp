#include "String.h"
#include <eoninlinetest/InlineTest.h>
#include <cctype>
#include <regex>
#include <unordered_map>


namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// Strings as Numbers
	//

	string string::separateThousands( char_t thousands_sep, const eon::locale* custom_locale ) const
	{
		iterator separator;
		auto area = substr().findFirstNumber( &separator, custom_locale );
		if( !area )
			return *this;
		if( separator )
			area.end() = separator;
		auto diff = ( area.end() - area.begin() ) % 3;
		if( diff == 0 )
			diff = 3;
		area.begin() += diff;
		string output = _prepOutput( area );
		for( auto pos = area.begin(); pos != area.end(); pos += 3 )
			output << thousands_sep << substr( pos, pos + 3 );
		if( area.end() != end() )
			output += substr( area.end(), end() );
		return output;
	}
	EON_TEST( string, separateThousands, empty,
		EON_EQ( "", string().separateThousands() ) );
	EON_TEST( string, separateThousands, non_numerical,
		EON_EQ( "abcdef", string( "abcdef" ).separateThousands() ) );
	EON_TEST( string, separateThousands, all_integer_short,
		EON_EQ( "123", string( "123" ).separateThousands() ) );
	EON_TEST( string, separateThousands, all_integer_even,
		EON_EQ( "123,456,789", string( "123456789" ).separateThousands() ) );
	EON_TEST( string, separateThousands, all_integer_odd,
		EON_EQ( "12,345,678", string( "12345678" ).separateThousands() ) );
	EON_TEST( string, separateThousands, all_float_even,
		EON_EQ( "123,456.789", string( "123456.789" ).separateThousands() ) );
	EON_TEST( string, separateThousands, all_float_odd,
		EON_EQ( "1,234,567.89", string( "1234567.89" ).separateThousands() ) );
	EON_TEST( string, separateThousands, all_float_no_int,
		EON_EQ( ".123,456,789", string( ".123456789" ).separateThousands() ) );
	EON_TEST( string, separateThousands, part_float_short,
		EON_EQ( "abc123.456def", string( "abc123.456def" ).separateThousands() ) );
	EON_TEST( string, separateThousands, part_float_even,
		EON_EQ( "abc123,456.789", string( "abc123456.789" ).separateThousands() ) );
	EON_TEST( string, separateThousands, part_float_odd,
		EON_EQ( "abc1,234,567.89", string( "abc1234567.89" ).separateThousands() ) );


	string string::roundNumber( index_t max_decimals, const eon::locale* custom_locale ) const
	{
		const eon::locale& loc = custom_locale != nullptr ? *custom_locale : locale::get();
		if( !isFloat( &loc ) )
			return *this;

		auto digits = characters<std::vector<char_t>>();
		auto sep = _findDecimalSeparator( digits, loc.decimalSep() );
		if( sep == digits.size() )
			return *this;

		auto num_decimals = digits.size() - sep - 1;
		if( num_decimals <= max_decimals )
			return *this;

		auto value = numeralValue( digits[ sep + 1 + max_decimals ] );
		if( max_decimals == 0 )
			digits.erase( digits.begin() + sep, digits.end() );
		else
			digits.erase( digits.begin() + sep + 1 + max_decimals, digits.end() );
		if( value >= 5 )
			_roundUp( digits, digits.end() - 1, loc.decimalSep()  );

		return string( digits );
	}
	EON_TEST( string, roundNumber, empty,
		EON_EQ( "", string().roundNumber( 2 ) ) );
	EON_TEST( string, roundNumber, non_numerical,
		EON_EQ( "abcd", string( "abcd" ).roundNumber( 2 ) ) );
	EON_TEST( string, roundNumber, integer,
		EON_EQ( "12345", string( "12345" ).roundNumber( 2 ) ) );
	EON_TEST( string, roundNumber, integer_default,
		EON_EQ( "12345", string( "12345" ).roundNumber() ) );
	EON_TEST( string, roundNumber, rounded_float,
		EON_EQ( "123.45", string( "123.45" ).roundNumber( 2 ) ) );
	EON_TEST( string, roundNumber, round_down,
		EON_EQ( "12.34", string( "12.344" ).roundNumber( 2 ) ) );
	EON_TEST( string, roundNumber, round_up,
		EON_EQ( "12.35", string( "12.345" ).roundNumber( 2 ) ) );
	EON_TEST( string, roundNumber, round_down_zero_dec,
		EON_EQ( "12", string( "12.345" ).roundNumber( 0 ) ) );
	EON_TEST( string, roundNumber, round_up_zero_dec,
		EON_EQ( "13", string( "12.543" ).roundNumber( 0 ) ) );
}
