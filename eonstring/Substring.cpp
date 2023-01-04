#include "Substring.h"
#include <eoninlinetest/InlineTest.h>
#include <cctype>


namespace eon
{
	EON_TEST_3STEP( substring, substring, copy,
		const char* source = "abcdef",
		substring other( source ),
		EON_TRUE( other == substring( other ) ) );

	EON_TEST_3STEP( substring, substring, iterators,
		const char* source = "abcdef",
		substring obj( string_iterator( source ), string_iterator( source ) + 6 ),
		EON_TRUE( obj == substring( source ) ) );

	EON_TEST_3STEP( substring, substring, iterator,
		const char* source = "abcdef",
		substring obj( string_iterator( source ) + 3 ),
		EON_TRUE( obj.empty() && *obj.begin() == 'd' ) );

	EON_TEST( substring, substring, stdstring,
		EON_EQ( 6, substring( std::string( "abcdef" ) ).numChars() ) );

	EON_TEST( substring, substring, cstring,
		EON_EQ( 6, substring( "abcdef" ).numChars() ) );




	EON_TEST_3STEP( substring, clear, basic,
		substring obj( "abcdef" ),
		obj.clear(),
		EON_TRUE( obj.empty() ) );




	EON_TEST_3STEP( substring, sameSource, true,
		const char* source = "abcdef",
		substring obj( source ),
		EON_TRUE( obj.sameSource( source ) ) );
	EON_TEST_3STEP( substring, sameSource, false,
		const char* source = "abcdef",
		substring obj( "bcdefa" ),
		EON_FALSE( obj.sameSource( source ) ) );

	EON_TEST_3STEP( substring, assertSameSource, true,
		const char* source = "abcdef",
		substring obj( source ),
		EON_NO_X( obj.assertSameSource( source ) ) );
	EON_TEST_3STEP( substring, assertSameSource, false,
		const char* source = "abcdef",
		substring obj( "bcdefa" ),
		EON_RAISE( obj.assertSameSource( source ), WrongSource ) );

	EON_TEST_3STEP( substring, sameArea, true,
		const char* source = "abcdef",
		substring obj( source ),
		EON_TRUE( obj.sameArea( substring( source ) ) ) );
	EON_TEST_3STEP( substring, sameArea, false,
		const char* source = "abcdef",
		substring obj( "bcdefa" ),
		EON_FALSE( obj.sameArea( substring( source + 1 ) ) ) );

	EON_TEST( substring, isLowToHigh, true_default,
		EON_TRUE( substring( "abcdef" ).isLowToHigh() ) );
	EON_TEST( substring, isLowToHigh, true_flipped,
		EON_TRUE( substring( "abcdef" ).highToLow().lowToHigh().isLowToHigh() ) );
	EON_TEST( substring, isLowToHigh, false,
		EON_FALSE( substring( "abcdef" ).highToLow().isLowToHigh() ) );

	EON_TEST( substring, isHighToLow, false_default,
		EON_FALSE( substring( "abcdef" ).isHighToLow() ) );
	EON_TEST( substring, isHighToLow, true_flipped,
		EON_TRUE( substring( "abcdef" ).highToLow().isHighToLow() ) );

	substring substring::lowToHigh() const noexcept
	{
		if( !isHighToLow() )
			return *this;
		return substring( End + 1, Beg + 1 );
	}
	EON_NO_TEST( substring, lowToHigh );

	substring substring::highToLow() const noexcept
	{
		if( !isLowToHigh() )
			return *this;
		return substring( End - 1, Beg - 1 );
	}
	EON_NO_TEST( substring, lowToHigh );

	EON_NO_TEST( substring, validUTF8 );

	EON_TEST( substring, operator_bool, true,
		EON_TRUE( substring( "abcdef" ) ) );
	EON_TEST( substring, operator_bool, false_empty,
		EON_FALSE( substring( "" ) ) );
	EON_TEST( substring, operator_bool, false_no_source,
		EON_FALSE( substring() ) );

	EON_TEST( substring, empty, true_no_source,
		EON_TRUE( substring().empty() ) );
	EON_TEST( substring, empty, true_source,
		EON_TRUE( substring( "" ).empty() ) );
	EON_TEST( substring, empty, false,
		EON_FALSE( substring( "abcdef" ).empty() ) );

	EON_TEST( substring, hasSource, true_empty,
		EON_TRUE( substring( "" ).hasSource() ) );
	EON_TEST( substring, hasSource, true_source,
		EON_TRUE( substring( "abcdef" ).hasSource() ) );
	EON_TEST( substring, hasSource, false,
		EON_FALSE( substring().hasSource() ) );

	EON_TEST( substring, numChars, no_source,
		EON_EQ( 0, substring().numChars() ) );
	EON_TEST( substring, numChars, empty,
		EON_EQ( 0, substring( "" ).numChars() ) );
	EON_TEST( substring, numChars, ASCII,
		EON_EQ( 6, substring( "abcdef" ).numChars() ) );
	EON_TEST( substring, numChars, UTF8,
		EON_EQ( 6, substring( u8"Ø€Œ™©µ" ).numChars() ) );

	index_t substring::numBytes() const noexcept
	{
		if( isHighToLow() )
			return lowToHigh().numBytes();
		auto beg_bytes = Beg.atREnd() ? 0 : Beg.atEnd() ? Beg.numSourceBytes() : Beg.numByte();
		auto end_bytes = End.atREnd() ? 0 : End.atEnd() ? End.numSourceBytes() : End.numByte();
		return end_bytes - beg_bytes;
	}
	EON_TEST( substring, numBytes, no_source,
		EON_EQ( 0, substring().numBytes() ) );
	EON_TEST( substring, numBytes, empty,
		EON_EQ( 0, substring( "" ).numBytes() ) );
	EON_TEST( substring, numBytes, ASCII,
		EON_EQ( 6, substring( "abcdef" ).numBytes() ) );
	EON_TEST( substring, numBytes, ASCII_HighToLow,
		EON_EQ( 6, substring( "abcdef" ).highToLow().numBytes() ) );
	EON_TEST( substring, numBytes, UTF8,
		EON_EQ( 14, substring( u8"Ø€Œ™©µ" ).numBytes() ) );
	EON_TEST( substring, numBytes, UTF8_HighToLow,
		EON_EQ( 14, substring( u8"Ø€Œ™©µ" ).highToLow().numBytes() ) );

	EON_TEST( substring, stdstr, empty,
		EON_EQ( "", substring( "" ).stdstr() ) );
	EON_TEST( substring, stdstr, all,
		EON_EQ( "abcdef", substring( "abcdef" ).stdstr() ) );
	EON_TEST_2STEP( substring, stdstr, part,
		const char* source = "abcdef",
		EON_EQ( "bcde", substring( string_iterator( source ) + 1, string_iterator( source ) + 5 ).stdstr() ) );
	EON_TEST_2STEP( substring, stdstr, part_HighToLow,
		const char* source = "abcdef",
		EON_EQ( "edcb", substring( string_iterator( source ) + 1, string_iterator( source ) + 5 ).highToLow().stdstr() ) );

	byte_t substring::byte( index_t pos ) const noexcept
	{
		if( empty() || pos >= numBytes() )
			return 0;
		if( isLowToHigh() )
			return *( Beg.Pos + pos );
		else
			return *( Beg.Pos - pos );
	}
	EON_TEST( substring, byte, no_source,
		EON_EQ( 0, static_cast<int>( substring().byte( 0 ) ) ) );
	EON_TEST( substring, byte, empty,
		EON_EQ( 0, static_cast<int>( substring( "" ).byte( 0 ) ) ) );
	EON_TEST( substring, byte, out_of_bounds,
		EON_EQ( 0, static_cast<int>( substring( "abcdef" ).byte( 7 ) ) ) );
	EON_TEST( substring, byte, ASCII_full,
		EON_EQ( 99, static_cast<int>( substring( "abcdef" ).byte( 2 ) ) ) );
	EON_TEST_2STEP( substring, byte, ASCII_sub,
		const char* source = "abcdefg",
		EON_EQ( 100, static_cast<int>( substring(
			string_iterator( source ) + 1, string_iterator( source ) + 5 ).byte( 2 ) ) ) );
	EON_TEST( substring, byte, ASCII_full_HighToLow,
		EON_EQ( 100, static_cast<int>( substring( "abcdef" ).highToLow().byte( 2 ) ) ) );
	EON_TEST_2STEP( substring, byte, ASCII_sub_HighToLow,
		const char* source = "abcdefg",
		EON_EQ( 101, static_cast<int>( substring(
			string_iterator( source ) + 6, string_iterator( source ) + 1 ).byte( 2 ) ) ) );

	EON_TEST( substring, enclosed, no_source,
		EON_FALSE( substring().enclosed( '"' ) ) );
	EON_TEST( substring, enclosed, empty,
		EON_FALSE( substring( "" ).enclosed( '"' ) ) );
	EON_TEST( substring, enclosed, false,
		EON_FALSE( substring( "abcdef" ).enclosed( '"' ) ) );
	EON_TEST( substring, enclosed, half,
		EON_FALSE( substring( "\"abcdef" ).enclosed( '"' ) ) );
	EON_TEST( substring, enclosed, same_char,
		EON_TRUE( substring( "\"abcdef\"" ).enclosed( '"' ) ) );
	EON_TEST( substring, enclosed, diff_char,
		EON_TRUE( substring( "(abcdef)" ).enclosed( '(', ')' ) ) );

	bool substring::blank() const noexcept
	{
		auto& chars = Characters::get();
		for( auto cp : *this )
		{
			if( !chars.isSeparatorSpace( cp ) && !chars.isOtherControl( cp ) )
				return false;
		}
		return true;
	}
	EON_TEST( substring, blank, no_source,
		EON_TRUE( substring().blank() ) );
	EON_TEST( substring, blank, empty,
		EON_TRUE( substring( "" ).blank() ) );
	EON_TEST( substring, blank, ASCII_true,
		EON_TRUE( substring( " \t" ).blank() ) );
	EON_TEST( substring, blank, ASCII_false,
		EON_FALSE( substring( " _" ).blank() ) );
	EON_TEST( substring, blank, UTF8_true,
		EON_TRUE( substring( u8" \u00A0\u2000\u3000" ).blank() ) );
	EON_TEST( substring, blank, UTF8_false,
		EON_FALSE( substring( u8" \u00A0\u2000_\u3000" ).blank() ) );

	string_iterator substring::iterator( index_t num_char ) const noexcept
	{
		auto num_chars = numChars();
		if( num_char > num_chars )
			return end().getEnd();

		if( num_chars >= 10 )
		{
			// Only optimize counting if the substring is of a certain size.
			// NOTE: Counting backward is slightly more costly so skew the middle-point.
			auto mid_point = ( num_chars / 3 ) * 2;
			if( num_char >= mid_point )
			{
				if( isLowToHigh() )
					return end() - ( num_chars - num_char );
				else
					lowToHigh().iterator( num_chars - num_char );
			}
		}
		return isLowToHigh() ? begin() + num_char : begin() - num_char;
	}
	EON_TEST( substring, iterator, no_source,
		EON_FALSE( substring().iterator( 0 ).hasSource() ) );
	EON_TEST( substring, iterator, empty,
		EON_TRUE( substring( "" ).iterator( 0 ).hasSource() ) );
	EON_TEST( substring, iterator, lowToHigh,
		EON_EQ( char_t( 'c' ), *substring( "abcdef" ).iterator( 2 ) ) );
	EON_TEST( substring, iterator, highToLow,
		EON_EQ( char_t( 'd' ), *substring( "abcdef" ).highToLow().iterator( 2 ) ) );
	EON_TEST( substring, iterator, lowToHigh_long,
		EON_EQ( char_t( 'u' ), *substring( "abcdefghijklmnopqrstuvwxyz" ).iterator( 20 ) ) );
	EON_TEST( substring, iterator, highToLow_long,
		EON_EQ( char_t( 'f' ), *substring( "abcdefghijklmnopqrstuvwxyz" ).highToLow().iterator( 20 ) ) );

	index_t substring::indentationLevel( char_t indentation_char ) const noexcept
	{
		index_t level = 0;
		for( auto cp : *this )
		{
			if( cp != indentation_char )
				break;
			++level;
		}
		return level;
	}
	EON_TEST( substring, indentationLevel, no_source,
		EON_EQ( 0, substring().indentationLevel( ' ' ) ) );
	EON_TEST( substring, indentationLevel, empty,
		EON_EQ( 0, substring( "" ).indentationLevel( ' ' ) ) );
	EON_TEST( substring, indentationLevel, ASCII_all_indents,
		EON_EQ( 2, substring( "  " ).indentationLevel( ' ' ) ) );
	EON_TEST( substring, indentationLevel, ASCII,
		EON_EQ( 2, substring( "  abc" ).indentationLevel( ' ' ) ) );
	EON_TEST( substring, indentationLevel, UTF8_all_indents,
		EON_EQ( 2, substring( u8"™™" ).indentationLevel( char_t( 8482 ) ) ) );
	EON_TEST( substring, indentationLevel, UTF8,
		EON_EQ( 2, substring( u8"™™abc" ).indentationLevel( char_t( 8482 ) ) ) );

	EON_TEST( substring, characters, no_source,
		EON_EQ( 0, substring().characters<std::vector<char_t>>().size() ) );
	EON_TEST( substring, characters, empty,
		EON_EQ( 0, substring( "" ).characters<std::vector<char_t>>().size() ) );
	EON_TEST( substring, characters, ASCII,
		EON_EQ( 6, substring( "abcdef" ).characters<std::vector<char_t>>().size() ) );
	EON_TEST( substring, characters, UTF8,
		EON_EQ( 6, substring( u8"Ø€Œ™©µ" ).characters<std::vector<char_t>>().size() ) );

	substring substring::trim() const
	{
		if( empty() || isHighToLow() )
			return *this;
		auto beg = findFirstNotOf( charcat::separator_space | charcat::other_control );
		auto end = highToLow().findLastNotOf( charcat::separator_space | charcat::other_control );
		return substring( beg ? beg : End, end ? end + 1 : End );
	}
	EON_TEST_2STEP( substring, trim, ASCII,
		const char* source = "  abcdefg  ",
		EON_EQ( "abcdefg", substring( source ).trim().stdstr() ) );
	EON_TEST_2STEP( substring, trim, UTF8,
		const char* source = u8"\u00A0\u2008Ø€Œ™©µ\u200A\u3000",
		EON_EQ( u8"Ø€Œ™©µ", substring( source ).trim().stdstr() ) );
	EON_TEST_2STEP( substring, trim, all,
		const char* source = " ",
		EON_EQ( "", substring( source ).trim().stdstr() ) );

	substring substring::trimLeading() const
	{
		if( empty() || isHighToLow() )
			return *this;
		auto beg = findFirstNotOf( charcat::separator_space | charcat::other_control );
		return substring( beg ? beg : End, End );
	}
	EON_TEST_2STEP( substring, trimLeading, ASCII,
		const char* source = "  abcdefg  ",
		EON_EQ( "abcdefg  ", substring( source ).trimLeading().stdstr() ) );
	EON_TEST_2STEP( substring, trimLeading, UTF8,
		const char* source = u8"\u00A0\u2008Ø€Œ™©µ\u200A\u3000",
		EON_EQ( u8"Ø€Œ™©µ\u200A\u3000", substring( source ).trimLeading().stdstr() ) );
	EON_TEST_2STEP( substring, trimLeading, all,
		const char* source = " ",
		EON_EQ( "", substring( source ).trimLeading().stdstr() ) );

	substring substring::trimTrailing() const
	{
		if( empty() || isHighToLow() )
			return *this;
		auto end = highToLow().findLastNotOf( charcat::separator_space | charcat::other_control );
		return substring( Beg, end ? end + 1 : End );
	}
	EON_TEST_2STEP( substring, trimTrailing, ASCII,
		const char* source = "  abcdefg  ",
		EON_EQ( "  abcdefg", substring( source ).trimTrailing().stdstr() ) );
	EON_TEST_2STEP( substring, trimTrailing, UTF8,
		const char* source = u8"\u00A0\u2008Ø€Œ™©µ\u200A\u3000",
		EON_EQ( u8"\u00A0\u2008Ø€Œ™©µ", substring( source ).trimTrailing().stdstr() ) );




	bool substring::numeralsOnly() const noexcept
	{
		for( auto chr : *this )
		{
			if( !eon::isNumeral( chr ) )
				return false;
		}
		return true;
	}
	bool substring::isUInt() const noexcept
	{
		for( auto chr : *this )
		{
			if( !eon::isDigit( chr ) )
				return false;
		}
		return true;
	}

	bool substring::isFloatingPoint( const locale* custom_locale ) const noexcept
	{
		if( empty() )
			return false;
		const locale& loc = custom_locale != nullptr ? *custom_locale : locale::get();
		string_iterator i = begin();
		if( isPlus( *i ) || isMinus( *i ) )
			++i;
		int sep = 0;
		for( ; i != end() && sep > 2; ++i )
		{
			if( *i == loc.decimalSep() )
				++sep;
			else if( !isNumeral( *i ) )
				return false;
		}
		return sep > 0 && sep < 2;
	}

	bool substring::isFloat( const locale* custom_locale ) const noexcept
	{
		if( empty() )
			return false;
		const locale& loc = custom_locale != nullptr ? *custom_locale : locale::get();
		string_iterator i = begin();
		if( *i == '+' || *i == '-' )
			++i;
		int sep = 0;
		for( ; i != end() && sep < 2; ++i )
		{
			if( *i == loc.decimalSep() )
				++sep;
			else if( !isDigit( *i ) )
				return false;
		}
		return sep > 0 && sep < 2;
	}

	long_t substring::toLongT() const noexcept
	{
		long_t value = 0;
		if( empty() )
			return value;
		string_iterator i = begin();
		long_t sign = isMinus( *i ) ? -1 : isPlus( *i ) ? 1 : 0;
		if( sign == 0 )
			sign = 1;
		else
			++i;
		for( ; i != end(); ++i )
		{
			value *= 10;
			value += numeralValue( *i );
		}
		return value * sign;
	}

	high_t substring::toHighT( const locale* custom_locale ) const noexcept
	{
		high_t value = 0.0;
		if( empty() )
			return value;
		const locale& loc = custom_locale != nullptr ? *custom_locale : locale::get();
		string_iterator i = begin();
		high_t sign = isMinus( *i ) ? -1.0 : isPlus( *i ) ? 1.0 : 0.0;
		if( sign == 0.0 )
			sign = 1.0;
		else
			++i;
		bool before_sep{ true };
		high_t decimals{ 0.0 }, decimal_power{ 1.0 };
		for( ; i != end(); ++i )
		{
			if( before_sep )
			{
				if( *i == loc.decimalSep() )
					before_sep = false;
				else
				{
					value *= 10.0;
					value += static_cast<high_t>( numeralValue( *i ) );
				}
			}
			else
			{
				decimals *= 10.0;
				decimal_power *= 10.0;
				decimals += static_cast<high_t>( numeralValue( *i ) );
			}
		}
		return sign * ( value + ( decimals / decimal_power ) );
	}

	index_t substring::toIndex() const noexcept
	{
		index_t value = 0;
		for( auto chr : *this )
		{
			value *= 10;
			value += numeralValue( chr );
		}
		return value;
	}

	substring substring::trimNumber( const locale* custom_locale ) const
	{
		// Find first non-zero
		auto first_non_zero = begin();
		while( first_non_zero != end() && *first_non_zero == ZeroChr )
			++first_non_zero;

		// All zeros, then return first zero
		if( first_non_zero == end() )
			return substring( first_non_zero - 1, end() );

		// Find the decimal separator
		const locale& loc = custom_locale != nullptr ? *custom_locale : locale::get();
		auto sep = first_non_zero;
		while( sep != end() && *sep != loc.decimalSep() )
			++sep;

		// If no separator, we have an all integer number
		if( sep == end() )
			return substring( first_non_zero, end() );

		// Find last non-zero
		auto last_non_zero = last();
		while( last_non_zero != sep && *last_non_zero == ZeroChr )
			--last_non_zero;

		// If no trailing fractional zeros, return the integer part
		if( last_non_zero == sep )
		{
			if( first_non_zero == last_non_zero )
			{
				if( first_non_zero > begin() )
					return substring( begin(), begin() + 1 );
				else if( last_non_zero < end() )
					return substring( last_non_zero + 1, last_non_zero + 2 );
				else
					return substring( first_non_zero, last_non_zero );
			}
			return substring( first_non_zero, sep );
		}

		return substring( first_non_zero, last_non_zero + 1 );
	}
	substring substring::trimFloat( const locale* custom_locale ) const
	{
		// Find first non-zero
		auto first_non_zero = begin();
		while( first_non_zero != end() && *first_non_zero == ZeroChr )
			++first_non_zero;

		// All zeros, then return empty substring
		if( first_non_zero == end() )
			return substring( end().getEnd() );

		// If first non-zero is the point, then include previous zero (if any)
		const locale& loc = custom_locale != nullptr ? *custom_locale : locale::get();
		if( *first_non_zero == loc.decimalSep() && first_non_zero != begin() )
			--first_non_zero;

		// Find the decimal separator
		auto sep = first_non_zero;
		while( sep != end() && *sep != loc.decimalSep() )
			++sep;

		// If no separator, we have an all integer number
		if( sep == end() )
			return substring( first_non_zero, end() );

		// Find last non-zero
		auto last_non_zero = last();
		while( last_non_zero != sep && *last_non_zero == ZeroChr )
			--last_non_zero;

		// If no trailing fractional zeros
		if( last_non_zero == sep )
		{
			// If one or more trailing zeros, include one - otherwise just
			// include the sep
			if( last_non_zero + 1 != end() )
				return substring( first_non_zero, last_non_zero + 2 );
			else
				return substring( first_non_zero, last_non_zero + 1 );
		}

		return substring( first_non_zero, last_non_zero + 1 );
	}




	const substring::fast_compare substring::FastCompare;
	const substring::fast_chr_compare substring::FastChrCompare;
	const substring::diff_compare substring::DiffCompare;
	const substring::icase_compare substring::ICaseCompare;
	const substring::icase_chr_compare substring::ICaseChrCompare;

	int substring::fast_compare::operator()( const substring& a, const substring& b ) const noexcept
	{
		if( !a )
			return b ? -1 : 0;
		else if( !b )
			return 1;
		auto cmp = memcmp( a.Beg.Pos, b.Beg.Pos, a.numBytes() < b.numBytes() ? a.numBytes() : b.numBytes() );
		return cmp != 0 || a.numBytes() == b.numBytes() ? cmp : a.numBytes() < b.numBytes() ? -1 : 1;
	}

	int substring::diff_compare::operator()( const substring& a, const substring& b ) const noexcept
	{
		if( !a )
			return b ? -1 : 0;
		else if( !b )
			return 1;
		string_iterator a_i = a.Beg, b_i = b.Beg;
		int pos = 1;
		for( ; a_i != a.End && b_i != b.end(); ++a_i, ++b_i, ++pos )
		{
			if( *a_i < *b_i )
				return -pos;
			else if( *b_i < *a_i )
				return pos;
		}
		return a_i == a.End && b_i == b.End ? 0 : a_i != a.End ? pos : -pos;
	}

	int substring::icase_compare::operator()( const substring& a, const substring& b ) const noexcept
	{
		if( !a )
			return b ? -1 : 0;
		else if( !b )
			return 1;
		string_iterator a_i = a.Beg, b_i = b.Beg;
		int pos = 1;
		for( ; a_i != a.End && b_i != b.end(); ++a_i, ++b_i, ++pos )
		{
			if( *a_i != *b_i )
			{
				auto lw_a = Loc->toLower(
					static_cast<wchar_t>( *a_i ) ), lw_b = Loc->toLower( static_cast<wchar_t>( *b_i ) );
				if( lw_a != lw_b )
					return lw_a < lw_b ? -pos : pos;
			}
		}
		return a_i == a.End && b_i == b.End ? 0 : a_i != a.End ? pos : -pos;
	}




	substring substring::findFirstNumber( string_iterator* separator, const locale* custom_locale )
	{
		auto& chars = Characters::get();
		const locale& loc = custom_locale != nullptr ? *custom_locale : locale::get();
		substring number;
		for( string_iterator i = begin(); i != end(); ++i )
		{
			if( !number.begin() )
			{
				if( chars.isNumber( *i ) )
					number.begin() = i; number.end() = i + 1;
			}
			else
			{
				if( chars.isNumber( *i ) )
					++number.end();
				else if( separator != nullptr && *i == loc.decimalSep() && !*separator )
					*separator = i;
				else
					break;
			}
		}
		return number;
	}

	string_iterator substring::findFirstOf( charcat category ) const noexcept
	{
		auto& chars = Characters::get();
		for( auto i = begin(); i != end(); ++i )
		{
			if( chars.is( *i, category ) )
				return i;
		}
		return end().getEnd();
	}

	string_iterator substring::findLastOf( charcat category ) const noexcept
	{
		auto& chars = Characters::get();
		for( auto i = begin(); i != end(); --i )
		{
			if( chars.is( *i, category ) )
				return i;
		}
		return end().getEnd();
	}
	string_iterator substring::findFirstNotOf( charcat category ) const noexcept
	{
		auto& chars = Characters::get();
		for( auto i = begin(); i != end(); ++i )
		{
			if( !chars.is( *i, category ) )
				return i;
		}
		return end().getEnd();
	}

	string_iterator substring::findLastNotOf( charcat category ) const noexcept
	{
		auto& chars = Characters::get();
		for( auto i = begin(); i != end(); --i )
		{
			if( !chars.is( *i, category ) )
				return i;
		}
		return end().getEnd();
	}


	string_iterator substring::findFirstOtherThan( charcat category ) const noexcept
	{
		auto& chars = Characters::get();
		for( auto i = begin(); i != end(); ++i )
		{
			if( !chars.is( *i, category ) )
				return i;
		}
		return end().getEnd();
	}
	string_iterator substring::findLastOtherThan( charcat category ) const noexcept
	{
		auto& chars = Characters::get();
		for( auto i = begin(); i != end(); --i )
		{
			if( !chars.is( *i, category ) )
				return i;
		}
		return end().getEnd();
	}




	void substring::_bypassSection( char_t start_sect, char_t end_sect, string_iterator& i ) const
	{
		int sections = 1;
		bool escaped = false;
		for( ++i; i != end(); ++i )
		{
			if( escaped )
				escaped = false;
			else
			{
				if( start_sect != end_sect )
				{
					if( *i == start_sect )
						++sections;
				}
				else
				{
					if( *i == BackSlashChr )
						escaped = true;
				}
				if( *i == end_sect )
				{
					if( --sections == 0 )
						break;
				}
			}
		}
	}

	const char* substring::_findFirst( const char* source, index_t source_size,
		const char* substr, index_t substr_size ) const noexcept
	{
		if( substr_size > source_size )
			return nullptr;
		const char* end = source + source_size - ( substr_size - 1 );
		for( auto c = _findFirst( source, end - source, *substr );
			c != nullptr && c != end;
			c = _findFirst( c + 1, end - ( c + 1 ), *substr ) )
		{
			if( memcmp( c + 1, substr + 1, substr_size - 1 ) == 0 )
				return c;
		}
		return nullptr;
	}
	const char* substring::_findLast( const char* str, index_t str_size, char chr ) noexcept
	{
		for( auto c = str, end = str - str_size; c != end; --c )
		{
			if( *c == chr )
				return c;
		}
		return nullptr;
	}
	const char* substring::_findLast(
		const char* source, index_t source_size, const char* substr, index_t substr_size ) const noexcept
	{
		const char* rbeg = source - substr_size + 1;
		const char* rend = source - source_size;
		for( auto c = rbeg; c != rend; --c )
		{
			if( *c != *substr )
				continue;
			if( source_size == 1 )
				return c;
			if( memcmp( c + 1, substr + 1, substr_size - 1 ) == 0 )
				return c;
		}
		return nullptr;
	}

	std::string substring::_getReverse() const
	{
		std::string revs; revs.reserve( numBytes() );
		uint32_t bytes{ 0 };
		for( auto i = begin(); i != end(); --i )
		{
			index_t size = string_iterator::unicodeToBytes( *i, bytes );
			revs += std::string( (const char*)&bytes, size );
		}
		return revs;
	}
}
