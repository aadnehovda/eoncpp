#include "String.h"
#include <cctype>
#include <regex>
#include <unordered_map>


namespace eon
{
	const string string::Empty;


	string::string( const char* input, index_t input_length, string non_utf8_substitution ) noexcept
	{
		eon::char_t codepoint{ 0 }, state{ 0 };
		unsigned char bytes[ 4 ]{ 0, 0, 0, 0 };
		auto c = input, end = input + input_length;
		while( c < end )
		{
			int i = 0;
			for( ; i < 4 && c < end; ++i, ++c )
			{
				if( !eon::string_iterator::utf8Decode( state, codepoint, static_cast<unsigned char>( *c ) ) )
					goto valid;
				bytes[ i ] = static_cast<unsigned char>( *c );
			}
			*this += non_utf8_substitution;
			continue;
		valid:
			*this += codepoint;
			++c;
		}
	}


	string& string::assign( const char_t* input, index_t input_length )
	{
		clear();
		reserve( input_length );
		for( index_t i = 0; i < input_length; ++i )
			*this += input[ i ];
		return *this;
	}
	string& string::assign( const char* input, index_t input_length )
	{
		iterator i( input, input_length );	// Using iterator to scan the raw string for us!
		if( !i.validUTF8() )
			throw InvalidUTF8();
		NumChars = i.numChars();
		Bytes.assign( input, input_length );
		return *this;
	}

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
	string& string::assign( index_t copies, const std::string& input )
	{
		// Make sure 'other' is not our own buffer!
		if( &input != &Bytes )
		{
			iterator i( input );
			if( !i.validUTF8() )
				throw InvalidUTF8();
			Bytes.reserve( input.size() * copies );
			for( index_t i = 0; i < copies; ++i )
				Bytes.append( input );
			NumChars = i.numChar() * copies;
			return *this;
		}
		else
			return assign( copies, std::string( input ) );
	}
	string& string::assign( index_t copies, const substring& input )
	{
		// Make sure 'input' is not from 'this'!
		if( !input.sameBuffer( Bytes.c_str() ) )
		{
			if( !input.begin().validUTF8() )
				throw InvalidUTF8();
			clear();
			Bytes.reserve( input.toSize() * copies );
			for( index_t i = 0; i < copies; ++i )
				*this += input;
			return *this;
		}
		else
			return assign( copies, string( input ) );
	}

	string& string::operator=( const substring& input )
	{
		if( !input.validUTF8() )
			throw InvalidUTF8();
		if( input.isHighToLow() )
		{
			clear();
			for( auto i = input.begin(); i != input.end(); --i )
				*this += *i;
		}
		else
		{
			NumChars = input.numChars();
			Bytes.assign( input.begin().byteData(), input.numBytes() );
		}
		return *this;
	}

	string& string::operator=( std::string&& input )
	{
		substring sub( input );
		if( !sub.validUTF8() )
			throw InvalidUTF8();

		NumChars = sub.numChars();
		Bytes = std::move( input );
		return *this;
	}

	string& string::operator=( const std::initializer_list<char_t>& input )
	{
		clear();
		reserve( input.size() );
		for( auto& c : input )
			*this += c;
		return *this;
	}
	string& string::operator=( const std::vector<char_t>& input )
	{
		clear();
		reserve( input.size() );
		for( auto& c : input )
			*this += c;
		return *this;
	}
	string& string::operator=( const std::initializer_list<char>& input )
	{
		clear();
		reserve( input.size() );
		for( auto& chr : input )
			*this += static_cast<byte_t>( chr );
		return *this;
	}
	string& string::operator=( const std::initializer_list<unsigned char>& input )
	{
		clear();
		reserve( input.size() );
		for( auto& chr : input )
			*this += static_cast<byte_t>( chr );
		return *this;
	}




	string::iterator string::bytePos( index_t pos, iterator start ) const
	{
		if( start )
			start.assertSameBuffer( Bytes.c_str() );
		else
			start = begin();
		if( _ascii() )		// Optimized ASCII handling
		{
			if( start.numByte() + pos < Bytes.size() )
				return iterator( Bytes.c_str(), Bytes.size(), start.numChar() + pos, Bytes.c_str() + start.numChar()
					+ pos );
			else
				return end();
		}

		// We have to count
		for( ; start != end() && pos > 0; ++start, --pos )
			;
		return start;
	}

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
	substring string::decodeSubstring( const string& encode_substring )
	{
		std::regex pattern{ R"(^(\d+):(\d+)\-(\d+):(\d+)$)" };
		std::smatch match;
		if( std::regex_match( encode_substring.stdstr(), match, pattern ) )
		{
			return substring( iterator( Bytes.c_str(), Bytes.size(), NumChars, Bytes.c_str()
				+ std::atoi( match[ 1 ].str().c_str() ), std::atoi( match[ 2 ].str().c_str() ) ),
				iterator( Bytes.c_str(), Bytes.size(), NumChars, Bytes.c_str()
					+ std::atoi( match[ 3 ].str().c_str() ), std::atoi( match[ 4 ].str().c_str() ) ) );
		}
		else
			return substring( end() );
	}




	string& string::operator+=( const substring& input )
	{
		if( input.validUTF8() )
		{
			Bytes.append( input.begin().byteData(), input.numBytes() );
			NumChars += input.numChars();
			return *this;
		}
		else
			return *this += string( input );
	}

	string::iterator string::insert( const iterator& pos, const string& substr )
	{
		pos.assertSameBuffer( Bytes.c_str() );
		if( !pos )
		{
			*this += substr;
			return iterator( Bytes.c_str(), Bytes.size(), NumChars, Bytes.c_str() + pos.numByte() );
		}
		Bytes.insert( pos.numByte(), substr.Bytes );
		NumChars += substr.NumChars;
		return iterator( Bytes.c_str(), Bytes.size(), NumChars, Bytes.c_str() + pos.numByte() );
	}

	string& string::erase( const substring& sub ) noexcept
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		if( area.begin().numByte() >= Bytes.size() )
			return *this;
		if( area.begin().numByte() + area.numBytes() > Bytes.size() )
			area.end() = end();
		Bytes.erase( area.begin().numByte(), area.numBytes() );
		NumChars -= area.numChars();
		return *this;
	}




	string string::upper( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result += substr( begin(), area.begin() );
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			if( *i <= 0xFFFF )
				result += static_cast<char_t>( std::toupper( static_cast<int>( *i ) ) );
			else
				result += *i;
		}
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}
	string string::lower( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result += substr( begin(), area.begin() );
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			if( *i <= 0xFFFF )
				result += static_cast<char_t>(
					std::tolower( static_cast<int>( *i ) ) );
			else
				result += *i;
		}
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}

	string string::ucFirst( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		if( area.numBytes() == 1 )
			return upper( area );
		else
		{
			return string( substr( begin(), area.begin() ) ) + string( substr( area.begin(), area.begin() + 1 ) ).upper()
				+ string( substr( area.begin() + 1 ) );
		}
	}
	string string::ucWords( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result += substr( begin(), area.begin() );
		bool new_word = true;
		auto& chars = Characters::get();
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			if( new_word )
			{
				if( chars.isLetterLowerCase( *i ) )
					result += static_cast<char_t>( std::toupper( *i ) );
				else
					result += *i;
				if( chars.isLetter( *i ) )
					new_word = false;
			}
			else
			{
				result += *i;
				if( chars.isSeparator( *i ) || chars.isPunctuation( *i ) )
					new_word = true;
			}
		}
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}
	string string::ucSentences( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result += substr( begin(), area.begin() );
		bool new_sentence = true;
		auto& chars = Characters::get();
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			if( new_sentence )
			{
				if( chars.isLetterLowerCase( *i ) )
					result += static_cast<char_t>( std::toupper( *i ) );
				else
					result += *i;
				if( chars.isLetter( *i ) )
					new_sentence = false;
			}
			else
			{
				result += *i;
				if( *i == '.' || *i == ':' || *i == '!' || *i == '?' )
					new_sentence = true;
			}
		}
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}

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

	string string::replace( const string& find, const string& replacement, const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto found = this->findFirst( find, sub );
		if( !found )
			return *this;

		string result;
		if( found.begin() != begin() )
			result += substr( begin(), found.begin() );
		else if( sub.begin() > begin() )
			result += substr( begin(), sub.begin() );
		auto pos = end();
		while( found )
		{
			result += replacement;
			pos = found.end();
			found = this->findFirst( find, substring( pos, sub.end() ) );
			if( found && found.begin() > pos )
				result += substr( pos, found.begin() );
		}
		if( pos != end() )
			result += substr( pos );
		return result;
	}
	string string::replace( char_t find, char_t replacement, const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result += substr( begin(), area.begin() );
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			if( *i == find )
				result += replacement;
			else
				result += *i;
		}
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}

	string string::replace( const std::map<string, string>& find_replace, const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		
		string result;
		if( sub.begin() != begin() )
			result += substr( begin(), sub.begin() );
		for( iterator i = sub.begin(); i != sub.end(); ++i )
		{
			for( auto& elm : find_replace )
			{
				if( substr( i, sub.end() ).compare( elm.first.substr(), elm.first.numChars() ) == 0 )
				{
					result += elm.second;
					i += elm.first.numChars() - 1;
					goto done;
				}
			}
			result += *i;
		done:
			;
		}
		return result;
	}
	string string::replace( const std::map<char_t, char_t>& find_replace, const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result += substr( begin(), area.begin() );
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			auto found = find_replace.find( *i );
			if( found != find_replace.end() )
				result += found->second;
			else
				result += *i;
		}
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}

	inline string _escape( const string& src, const substring& sub, const std::unordered_map<char_t, char_t>& singletons )
	{
		if( sub.empty() )
			return src;
		auto area = sub.lowToHigh();
		string result;
		auto& chars = Characters::get();
		for( auto c = area.begin(); c != area.end(); ++c )
		{
			auto found = singletons.find( *c );
			if( found != singletons.end() )
			{
				if( found->second != nochar )
				{
					result += BackSlashChr;
					result += found->second;
				}
				else
					result += *c;
			}

			// Null gets output as octal
			else if( *c == NullChr )
			{
				result += BackSlashChr;
				result += ZeroChr;
			}

			// Non-printable gets output as Unicode escapes
			else if( chars.isOther( *c ) )
			{
				result += BackSlashChr;
				result += "U";
				auto end = c + 1;
				for( index_t i = 0; i < end.numByte(); ++i )
					result += byteToHex( *( c.byteData() + i ) );
			}

			else
				result += *c;
		}
		return result;
	}
	string string::escape( const substring& sub ) const
	{
		static std::unordered_map<char_t, char_t> singletons{
			{ '\'', nochar }, { '"', '"' }, { '?', nochar }, { '\\', '\\' },
			{ '\a', 'a' }, { '\b', 'b' }, { '\f', 'f' }, { '\n', 'n' },
			{ '\r', 'r' }, { '\t', nochar }, { '\v', 'v' } };
		return _escape( *this, sub, singletons );
	}
	string string::escapeAll( const substring& sub ) const
	{
		static std::unordered_map<char_t, char_t> singletons{
			{ '\'', '\'' }, { '"', '"' }, { '?', '?' }, { '\\', '\\' },
			{ '\a', 'a' }, { '\b', 'b' }, { '\f', 'f' }, { '\n', 'n' },
			{ '\r', 'r' }, { '\t', 't' }, { '\v', 'v' } };
		return _escape( *this, sub, singletons );
	}
	string string::unescape( const substring& sub ) const
	{
		static std::unordered_map<char_t, char_t> singletons{
			{ '\'', '\'' }, { '"', '"' }, { '?', '?' }, { '\\', '\\' },
			{ 'a', '\a' }, { 'b', '\b' }, { 'f', '\f' }, { 'n', '\n' },
			{ 'r', '\r' }, { 't', '\t' }, { 'v', '\v' } };

		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		for( auto c = area.begin(); c != area.end(); ++c )
		{
			auto c1 = c + 1;
			if( *c == BackSlashChr && c1 != area.end() )
			{
				auto found = singletons.find( *c1 );
				if( found != singletons.end() )
				{
					result += found->second;
					++c;
					continue;
				}

				// Unicode
				else if( ( *c1 == 'u' || *c1 == 'U' )
					&& c1 + 1 != area.end() && isHexDigit( *( c1 + 1 ) ) )
				{
					++c;
					char_t val{ 0 };
					for( int j = 0; j < 8; ++j )
					{
						val <<= 4;
						val += hexToNum( static_cast<byte_t>( *++c ) );
						if( c + 1 == area.end() || !isHexDigit( *( c + 1 ) ) )
							break;
					}
					result += val;
					continue;
				}

				// Hex
				else if( ( *c1 == 'x' || *c1 == 'X' ) && c1 + 1 != area.end() && isHexDigit( *( c1 + 1 ) ) )
				{
					++c;
					byte_t val{ 0 };
					int shifts = 0;
					while( true )
					{
						val <<= 4;
						val += hexToNum( static_cast<byte_t>( *++c ) );
						if( ++shifts == 2 )
						{
							shifts = 0;
							result += val;
							val = 0;
						}
						if( c + 1 == area.end() || !isHexDigit( *( c + 1 ) ) )
							break;
					}
					continue;
				}

				// Octal
				else if( isOctalDigit( *c1 ) )
				{
					uint32_t val{ 0 };
					bool have_val = false;
					while( true )
					{
						val = ( val << 3 ) + ( *++c - '0' );
						have_val = true;
						if( val > UINT32_MAX )
						{
							uint32_t chr = val >> 2;
							result += static_cast<char_t>( chr );
							val -= chr << 2;
						}
						if( c + 1 == area.end() || !isOctalDigit( *( c + 1 ) ) )
						{
							if( have_val )
							{
								result += static_cast<char_t>( val );
								have_val = false;
							}
							break;
						}
					}
					continue;
				}
			}
			result += *c;
		}
		return result;
	}

	string string::escapeNonPrintable( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		for( auto c : area )
		{
			if( c < '\n' || ( c > '\n' && c < ' ' ) || c > 126 )
				result += "&#" + string( static_cast<int32_t>( c ) ) + SemiColonChr;
			else
				result += c;
		}
		return result;
	}
	string string::unescapeNonPrintable( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		for( auto i = area.begin(); i != area.end(); ++i )
		{
			if( *i == '&' && i.numChar() < NumChars - 4 && *( i + 1 ) == '#' )
			{
				auto end = substring( i, area.end() ).findFirst( SemiColonChr );
				if( end )
				{
					auto num = substr( i + 2, end.begin() );
					if( !num.findFirstOtherThan( charcat::number_ascii_digit ) )
					{
						result += static_cast<char_t>( num.toUInt32() );
						i = end.begin();
						continue;
					}
				}
			}
			result += *i;
		}
		return result;
	}

	string string::doubleQuote( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result = substr( begin(), area.begin() );
		if( !isDoubleQuoted( area ) )
			result += DblQuoteChr + string( area ) + DblQuoteChr;
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}
	string string::singleQuote( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result = substr( begin(), area.begin() );
		if( !isSingleQuoted( area ) )
			result += SglQuoteChr + string( area ) + SglQuoteChr;
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}

	string string::unQuote( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		string result;
		if( area.begin() > begin() )
			result = substr( begin(), area.begin() );
		if( isDoubleQuoted( area ) || isSingleQuoted( area ) )
			result += substr( area.begin() + 1, area.end() - 1 );
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}

	string string::reverse( const substring& sub ) const
	{
		if( sub.empty() )
			return *this;
		auto area = sub.lowToHigh();
		auto revs = sub.highToLow();
		string result;
		if( area.begin() > begin() )
			result = substr( begin(), area.begin() );
		for( auto i = revs.begin(); i != revs.end(); --i )
			result += *i;
		if( area.end() != end() )
			result += substr( area.end() );
		return result;
	}

	string string::padLeftAndRight( index_t target_size, char_t fill ) const
	{
		if( numChars() >= target_size )
			return *this;

		auto diff = target_size - numChars(), half = diff / 2;
		return string( half, fill ) + *this + string( diff - half, fill );
	}

	string string::indentLines( index_t indentation_level, char_t indentation_char ) const
	{
		auto lines = splitSequential<std::list<substring>>( NewlineChr );
		string result;
		bool first_line = true;

		auto line = lines.begin();
		auto level = line->indentationLevel( indentation_char );
		if( level == indentation_level )
		{
			result += *line;
			++line;
			first_line = false;
		}

		for( ; line != lines.end(); ++line )
		{
			if( first_line )
				first_line = false;
			else
				result += NewlineChr;
			if( !line->empty() )
				result += string( indentation_level, indentation_char ) + *line;
		}
		return result;
	}




	string string::separateThousands( char_t thousands_sep, char_t decimal_separator ) const
	{
		if( !isFloat() && !isUInt() && !isInt() )
			return *this;

		auto sep = findFirst( decimal_separator );
		if( !sep )
			sep = substring( end() );

		if( sep.begin() - begin() < 4 )
			return *this;

		string formatted( substr( begin(), begin() + sep.begin().numChar() % 3 ) );
		auto pos = begin() + formatted.numChars();
		while( pos != sep.begin() )
		{
			if( !formatted.empty() )
				formatted += thousands_sep;
			formatted += substr( pos, pos + 3 );
			pos += 3;
		}
		formatted += substr( sep.begin() );
		return formatted;
	}
	string string::roundNumber( index_t max_decimals, char_t decimal_separator ) const
	{
		if( !isFloat() && !isUInt() && !isInt() )
			return *this;

		auto digits = chars();
		auto sep = _findDecimalSeparator( digits, decimal_separator);
		if( sep == digits.size() )
			return *this;

		auto num_decimals = digits.size() - sep - 1;
		if( num_decimals <= max_decimals )
			return *this;

		auto digit = digits[ sep + 1 + max_decimals ] - ZeroChr;
		if( max_decimals == 0 )
			digits.erase( digits.begin() + sep, digits.end() );
		else
			digits.erase( digits.begin() + sep + 1 + max_decimals, digits.end() );
		if( digit >= 5 )
			_roundUp( digits, digits.size() - 1 );

		return string( digits );
	}




	string& string::operator=( const std::wstring& stdwstr )
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return *this = conv.to_bytes( stdwstr );
	}
	string& string::operator=( const wchar_t* cstr )
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return *this = conv.to_bytes( cstr );
	}
	std::wstring string::wstr() const
	{
		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
		return conv.from_bytes( Bytes );
	}




	string& string::push_back( const string& element, const string& separator )
	{
		if( !empty() )
			*this += separator;
		*this += element;
		return *this;
	}
	string& string::push_front( const string& element, const string& separator )
	{
		if( !empty() )
			*this = element + separator + *this;
		else
			*this = element;
		return *this;
	}
	string& string::orderList( const string& separator )
	{
		auto elements = substr().splitNonsequential<std::set<substring>>( separator.substr() );
		return *this = separator.join( elements );
	}




	void string::setLocale( const std::string& name )
	{
		static std::string locale_name;
		if( name != locale_name )
		{
			locale_name = name;
			std::locale::global( std::locale( locale_name ) );
		}
	}

	string string::toString( double value )
	{
		static char digits[ 325 ];
#ifdef EON_WINDOWS
		sprintf_s( digits, 325, "%.8f", value );
#else
		snprintf( digits, 325, "%.8f", value );
#endif
		auto size = strlen( digits );
		for( ; digits[ size - 1 ] == '0' && digits[ size - 2 ] != '.';
			--size )
			;
		return string( std::string( digits, size ), true );
	}
	string string::toString( long double value )
	{
		static char digits[ 480 ];
#ifdef EON_WINDOWS
		sprintf_s( digits, 480, "%.10Lf", value );
#else
		snprintf( digits, 480, "%.10Lf", value );
#endif
		auto size = strlen( digits );
		for( ; digits[ size - 1 ] == '0' && digits[ size - 2 ] != '.';
			--size )
			;
		return string( std::string( digits, size ), true );
	}

	const std::string& string::bom()
	{
		static std::string _bom;
		if( _bom.empty() )
		{
			_bom = (char)0xEF;
			_bom += (char)0xBB;
			_bom += (char)0xBF;
		}
		return _bom;
	}




	index_t string::_findDecimalSeparator( std::vector<char_t>& digits, char_t decimal_separator ) noexcept
	{
		for( index_t i = 0; i < digits.size(); ++i )
		{
			if( digits[ i ] == decimal_separator )
				return i;
		}
		return digits.size();
	}

	void string::_roundUp( std::vector<char_t>& digits, index_t i ) noexcept
	{
		// The digit at i + 1 is greater than 5, so we have to round up.
		if( digits[ i ] == PointChr )
			--i;
		auto digit = digits[ i ] - ZeroChr;
		if( i == 0 )
		{
			digits[ i ] = '9';
			digits.insert( digits.begin(), static_cast<byte_t>( '1' ) );
			return;
		}
		if( digit == 9 )
		{
			digits[ i ] = '0';
			_roundUp( digits, i - 1 );
		}
		else
			digits[ i ] = static_cast<byte_t>( ZeroChr + digit + 1 );
	}
}
