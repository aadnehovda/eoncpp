#pragma once
#include "StringIterator.h"
#include "Locale.h"


///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// A substring is a part of a larger string, but rather than containing a
	// full copy of the section from the larger string, it merely references it
	// using iterators. (See [eon::string_iterator].)
	//
	// The substring class can be used to reference [eon::string],
	// [std::string], and [const char*].
	//
	// [eon::string] relies heavily on methods implemented for substring.
	//
	// A substring can be ordered 'low-to-high' (normal) and 'high-to-low'
	// (reverse). Unless explicitly stated otherwise, methods required that the
	// substring be in 'low-to-high' ordering.
	//
	// Substrings can have some flags set. For example, if the 'icase' flag is
	// set, all comparison and searching involving the substring in question
	// will ignore case differences.
	//
	// WARNING: A substring is only valid for as long as the source string
	//          remains unchanged! Use of a substring with an invalid source
	//          may result in undefined behavior!
	//
	class substring
	{
		///////////////////////////////////////////////////////////////////////
		//
		// Construction
		//
	public:

		// Default construction, results in a 'false', empty substring
		substring() = default;

		// Construct as a copy of another substring
		inline substring( const substring& other ) noexcept { *this = other; }

		// Construct from a pair of [eon::string_iterator]s
		inline substring( const string_iterator& begin, const string_iterator& end ) {
			begin.assertSameSource( end ); Beg = begin, End = end; }

		// Construct an empty substring for an actual [eon::string_iterator]
		inline explicit substring( const string_iterator& itr ) noexcept { Beg = itr; End = itr; }

		// Construct for an 'std::string' (the entire string)
		inline explicit substring( const std::string& stdstr ) noexcept {
			Beg = string_iterator( stdstr ); End = Beg.getEnd(); }

		// Construct for a 'C-string' (the entire string)
		inline explicit substring( const char* cstr ) noexcept {
			Beg = string_iterator( cstr, strlen( cstr ) ); End = Beg + Beg.numSourceChars(); }

		// Default destruction
		~substring() = default;




		///////////////////////////////////////////////////////////////////////
		//
		// General Modifier Methods
		//
		// Due to the nature of UTF-8 strings in general (and since substring is
		// specifically (but not exclusively)
		//
	public:

		// Clear the substring (make it empty and 'false')
		inline void clear() noexcept { Beg.reset(); End.reset(); }




		///////////////////////////////////////////////////////////////////////
		//
		// Read-only Methods
		//
	public:

		// Check if the specified source is the same as referenced by the substring.
		// NOTE: Must be the same exact memory address!
		inline bool sameSource( const char* source ) const noexcept {
			return Beg.sameSource( source ) && End.sameSource( source ); }

		// Assert that the specified source is the exact same as referenced by the substring.
		// Throws [eon::WrongSource] if not.
		inline void assertSameSource( const char* source ) const { if( !sameSource( source ) ) throw WrongSource(); }

		// Check if the 'other' substring covers the exact same area of the
		// source string as 'this' (with same ordering).
		inline bool sameArea( const substring& other ) const noexcept { return Beg == other.Beg && End == other.End; }


		// Check if ordered 'low-to-high'.
		// NOTE: Source-less and empty substrings have no ordering!
		inline bool isLowToHigh() const noexcept { return Beg < End; }

		// Check if ordered 'high-to-low' (reverse).
		// NOTE: Source-less and empty substrings have no ordering!
		inline bool isHighToLow() const noexcept { return Beg > End; }

		// Get a copy of the substring, ensured to be low-to-high ordered.
		substring lowToHigh() const noexcept;

		// Get a copy of the substring, ensured to be high-to-low ordered.
		substring highToLow() const noexcept;


		// Check if substring is for valid 'UTF-8' iterators.
		inline bool validUTF8() const noexcept { return Beg.ValidUTF8; }


		// Check if the substring iterators have a source and are not both at the end of that source.
		inline operator bool() const noexcept { return Beg.hasSource() && End.hasSource() && ( Beg || End ); }

		// Check if the substring is empty.
		// NOTE: A source-less substring is by definition empty!
		inline bool empty() const noexcept { return numChars() == 0; }

		// Check if there is a source.
		// NOTE: A substring with source can still be empty!
		inline bool hasSource() const noexcept { return Beg.hasSource() && End.hasSource(); }

		// Get number of characters in substring.
		inline index_t numChars() const noexcept { return static_cast<index_t>( End >= Beg ? End - Beg : Beg - End ); }

		// Get number of bytes in substring.
		index_t numBytes() const noexcept;


		// Get a (new) std::string (bytes) from substring.
		// NOTE: If the substring is 'high-to-low', the std::string will be in reverse.
		inline std::string stdstr() const noexcept {
			return isHighToLow() ? _getReverse() : std::string( Beg.byteData(), numBytes() ); }

		// Read a byte inside the substring.
		// Returns zero if source-less substring or position is out of bounds!
		byte_t byte( index_t pos ) const noexcept;


		// Check if the substring is enclosed by a specific pair of characters, such as quotation marks or parenthesis.
		// NOTE: if 'end_char' is 'same_char' it will be the same as 'start_char'!
		inline bool enclosed( char_t start_char, char_t end_char = same_char ) const noexcept {
			return numChars() > 1 && *begin() == start_char && *last() == (
				end_char == same_char ? start_char : end_char ); }


		// Check if the substring is blank (is empty or contains characters from
		// the 'Separator, Space' or 'Other, Control' categories only).
		bool blank() const noexcept;


		// Given a character number, get string iterator for that position.
		// Will count from end if that is closer than start.
		// Returns 'end' iterator if invalid character number.
		// NOTE: The 'num_char' argument is relative to the substring, not the source string!
		// NOTE: If 'high-to-low' ordering, counting is reversed!
		string_iterator iterator( index_t num_char ) const noexcept;


		// Get number of indentations for the first (and possibly only) line of text in the string.
		index_t indentationLevel( char_t indentation_char = TabChr ) const noexcept;


		// Get characters as a sequential container of codepoint elements.
		// High-to-low range will give characters in reverse!
		// NOTE: Container must support 'push_back( char_t )'!
		template<typename container>
		inline container characters() const
		{
			auto output = container();
			for( auto chr : *this )
				output.push_back( chr );
			return output;
		}


		// Get iterator for first character not in the 'Separator, Space' category nor in 'Other, Control'.


		// Get a substring narrowed in front and end, bypassing all characters
		// in the 'Separator, Space' and 'Other, Control' categories.
		// NOTE: Substring must be 'low-to-high'!
		substring trim() const;

		// Get a substring narrowed in front, bypassing all characters in the
		// 'Separator, Space' and 'Other, Control' categories.
		// NOTE: Substring must be 'low-to-high'!
		substring trimLeading() const;

		// Get a substring narrowed at end, bypassing all characters in the
		// 'Separator, Space' and 'Other, Control' categories.
		// NOTE: Substring must be 'low-to-high'!
		substring trimTrailing() const;




		///////////////////////////////////////////////////////////////////////
		//
		// Strings as number
		//
	public:

		// Check if the substring contains only characters in the 'Number, Decimal Digit' category.
		bool numeralsOnly() const noexcept;

		// Check if the substring contains only ASCII digits. (Slightly cheaper than [numeralsOnly].)
		bool isUInt() const noexcept;

		// Check if substring starts with a plus or minus sign that is followed by numerals only!
		inline bool isSignedInteger() const noexcept {
			return numChars() > 1 && ( isPlus( *begin() ) || isMinus( *begin() ) )
				&& substring( begin() + 1, end() ).numeralsOnly(); }

		// Check if the substring starts with ASCII '+' or '-' that is followed by ASCII digits only!
		// (Slightly cheaper than [isSignedInteger].)
		inline bool isInt() const noexcept {
			return numChars() > 1 && ( *begin() == '+' || *begin() == '-' )
				&& substring( begin() + 1, end() ).isUInt(); }

		// Check if substring optionally starts with (any) plus or (any) minus sign and
		// only (any) numerals with a single decimal separator after that.
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		bool isFloatingPoint( const locale* custom_locale = nullptr ) const noexcept;

		// Check if substring optionally starts with '+' or '-' and contains
		// only ASCII digits and a single decimal separator after that.
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		// NOTE: Decimal separator may occur before the first numeral or after the last!
		// (Slightly cheaper than [isFloatingPoint].)
		bool isFloat( const locale* custom_locale = nullptr ) const noexcept;


		// Convert unsigned or signed integer string into long_t (signed 64-bit integer).
		// Assumes [numeralsOnly] or [isSignedInteger] is true. (Will not check!)
		long_t toLongT() const noexcept;

		// Convert unsigned or signed integer string into int_t (signed 32-bit integer).
		// Assumes [numeralsOnly] or [isSignedInteger] is true. (Will not check!)
		inline int_t toIntT() const noexcept { return static_cast<int_t>( toLongT() ); }

		// Convert unsigned or signed integer string into short_t (signed 16-bit integer).
		// Assumes [numeralsOnly] or [isSignedInteger] is true. (Will not check!)
		inline int_t toShortT() const noexcept { return static_cast<short_t>( toLongT() ); }

		// Convert unsigned or signed floating point string into high_t (64- or 96- or 128-bit float).
		// Assumes [numeralsOnly] or [isFloatingPoint] is true. (Will not check!)
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		high_t toHighT( const locale* custom_locale = nullptr ) const noexcept;

		// Convert unsigned or signed floating point string into flt_t (64-bit float).
		// Assumes [numeralsOnly] or [isFloatingPoint] is true. (Will not check!)
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		inline high_t toFltT( const locale* custom_locale = nullptr ) const noexcept {
			return static_cast<flt_t>( toHighT( custom_locale ) ); }

		// Convert unsigned or signed floating point string into low_t (32-bit float).
		// Assumes [numeralsOnly] or [isFloatingPoint] is true. (Will not check!)
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		inline low_t toLowT( const locale* custom_locale = nullptr ) const noexcept {
			return static_cast<low_t>( toHighT( custom_locale ) ); }

		// Convert unsigned integer string into index_t (64-bit unsigned integer).
		// Assumes [numeralsOnly] or [isUInt] is true. (Will not check!)
		index_t toIndex() const noexcept;


		// Convert to int32_t by casting from int_t.
		inline int32_t toInt32() const { return static_cast<int32_t>( toIntT() ); }

		// Convert to int64_t by casting from long_t.
		inline int64_t toInt64() const { return static_cast<int64_t>( toLongT() ); }

		// Convert to double by casting from flt_t.
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		inline double toDouble( const locale* custom_locale = nullptr ) const {
			return static_cast<double>( toFltT( custom_locale ) ); }

		// Convert to long double by casting from high_t.
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		long double toLongDouble( const locale* custom_locale = nullptr ) const {
			return static_cast<long double>( toHighT( custom_locale ) ); }

		// Convert to uint32_t by casting from index_t.
		inline uint32_t toUInt32() const { return static_cast<uint32_t>( toIndex() ); }

		// Convert to uint64_t by casting from index_t.
		inline uint64_t toUInt64() const { return static_cast<uint64_t>( toIndex() ); }

		// Convert to size_t by casting from index_t.
		inline size_t toSize() const { return static_cast<size_t>( toIndex() ); }


		// Assuming the entire substring is a number, get a reduced substring
		// where leading integer zeros and trailing fractional zeros are
		// removed - including the decimal separator if all fractional zeros.
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		// NOTE: If the number ends in separator, the separator will be excluded!
		// NOTE: If the number is signed, leading integer zeros will be included to keep the sign!
		// Example: 0001.0000 -> 1
		// Example: -001.0000 -> -001
		substring trimNumber( const locale* custom_locale = nullptr ) const;

		// Assuming the entire substring is a number, get a reduced substring
		// where leading integer zeros and trailing fractional zeros are
		// removed - leave the decimal separator and one fractional zero if
		// all fractional zeros. (If the number ends with the separator, it
		// will be included.)
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		// NOTE: If the number is signed, leading integer zeros will be
		//       included to keep the sign.
		// Example: 0001.0000 -> 1.0
		substring trimFloat( const locale* custom_locale = nullptr ) const;




		///////////////////////////////////////////////////////////////////////
		//
		// String hashing
		//
	public:

		// TODO: Use locale!

		// FNV-1a hash algorithm used for producing hash values from the
		// source string that is covered by the substring.
#define FNV_PRIME32 16777619
#define FNV_PRIME64 1099511628211LLU
#define FNV_OFFSET32 2166136261
#define FNV_OFFSET64 14695981039346656037LLU
		inline uint32_t hash32() const noexcept { return hash32( Beg.byteData(), End.byteData() ); }
		inline uint64_t hash64() const noexcept { return hash64( Beg.byteData(), End.byteData() ); }
#if defined(_WIN64) || defined(__x86_64__)
		inline size_t hash() const noexcept { return hash64( Beg.byteData(), End.byteData() ); }
#else
		inline size_t hash() const noexcept { return hash32( Beg.byteData(), End.byteData() ); }
#endif

		// Static hash method for raw bytes
		static inline uint32_t hash32( const char* begin, const char* end, uint32_t h = FNV_OFFSET32 ) noexcept {
			for( auto c = begin; c != end; ++c ) { h ^= static_cast<unsigned char>( *c ); h *= FNV_PRIME32; } return h; }
		static inline uint64_t hash64( const char* begin, const char* end, uint64_t h = FNV_OFFSET64 ) noexcept {
			for( auto c = begin; c != end; ++c ) { h ^= static_cast<unsigned char>( *c ); h *= FNV_PRIME64; } return h; }




		///////////////////////////////////////////////////////////////////////
		//
		// Iteration
		//
	public:

		// Get iterator for the start of the substring
		inline string_iterator& begin() noexcept { return Beg; }
		inline const string_iterator& begin() const noexcept { return Beg; }
		inline const string_iterator& cbegin() const noexcept { return Beg; }

		// Get iterator for the end of the substring
		inline string_iterator& end() noexcept { return End; }
		inline const string_iterator& end() const noexcept { return End; }
		inline const string_iterator& cend() const noexcept { return End; }

		// Get iterator for the last character in the substring
		// Returns 'end()' if 'high-to-low' ordering or empty substring.
		inline const string_iterator last() const noexcept { return Beg < End ? End - 1 : End; }




		///////////////////////////////////////////////////////////////////////
		//
		// Splitting into multiple substrings
		//
	public:

		// Split substring on every occurrence of 'delimiter' into a
		// 'sequential container' (typically list<substring> or
		// vector<substring>) - requires the 'push_back' method!
		// If a max number of elements is specified, the last element will
		// reference the unsplit remainder.
		template<typename container_t>
		container_t splitSequential( const substring& delimiter, index_t max_elements = INDEX_MAX ) const
		{
			container_t elms;
			auto start = begin();
			for( auto found = findFirst( delimiter );
				found || elms.size() == max_elements - 1;
				found = substring( start, end() ).findFirst( delimiter ) )
			{
				elms.push_back( substring( start, found.begin() ) );
				start = found.end();
			}
			if( start )
				elms.push_back( substring( start, end() ) );
			return elms;
		}
		template<typename container_t>
		container_t splitSequential( char_t delimiter, index_t max_elements = INDEX_MAX ) const
		{
			container_t elms;
			auto start = begin();
			for( auto found = findFirst( delimiter );
				found || elms.size() == max_elements - 1;
				found = substring( start, end() ).findFirst( delimiter ) )
			{
				elms.push_back( substring( start, found.begin() ) );
				start = found.end();
			}
			if( start )
				elms.push_back( substring( start, end() ) );
			return elms;
		}

		// Split substring on every occurrence of 'delimiter' into a
		// 'non-sequential container' (typically set<substring>,
		// multiset<substring>, unordered_set<substring>, or
		// unordered_multiset<substring>) - requires the insert method!
		template<typename container_t>
		container_t splitNonsequential( const substring& delimiter ) const
		{
			container_t elms;
			auto start = begin();
			for( auto found = findFirst( delimiter );
				found;
				found = substring( start, end() ).findFirst( delimiter ) )
			{
				elms.insert( substring( start, found.begin() ) );
				start = found.begin() + delimiter.numChars();
			}
			if( start )
				elms.insert( substring( start, end() ) );
			return elms;
		}
		template<typename container_t>
		container_t splitNonSequential( char_t delimiter ) const
		{
			container_t elms;
			auto start = begin();
			for( auto found = findFirst( delimiter );
				found;
				found = substring( start, end() ).findFirst( delimiter ) )
			{
				elms.insert( substring( start, found.begin() ) );
				start = found.begin() + 1;
			}
			if( start )
				elms.insert( substring( start, end() ) );
			return elms;
		}




		///////////////////////////////////////////////////////////////////////
		//
		// Comparing
		//
		// All compare methods require all substrings to be 'low-to-high'!
		//
		// The C++ std::locale (std::collate) is not suited for comparing UTF-8
		// strings and therefore not suited for Eon strings - which are pre-
		// dominantly UTF-8. Instead, comparing is done using binary that users
		// requiring custom sorting can provide, while default predicates are
		// used otherwise.
		//
	public:

		// Predicate for comparing two substrings as fast as possible.
		// Returns only -1 for less-than, 0 for equal-to, and 1 for greather-than.
		// NOTE: Compares on bytes, ignoring all other concerns!
		struct fast_compare
		{
			int operator()( const substring& a, const substring& b ) const noexcept;
		};
		static const fast_compare FastCompare;

		// Predicate for compare two characters as fast as possible.
		// Returns -1 for less-than, 0 for equal-to, and 1 fore greater-than.
		struct fast_chr_compare
		{
			inline int operator()( char_t a, char_t b ) const noexcept { return a < b ? -1 : a == b ? 0 : 1; }
		};
		static const fast_chr_compare FastChrCompare;

		// Predicate for comparing two substrings lexiographically, returning 0 if equal,
		// -<first diff char> if less-than, and +<first diff char> if greather-than.
		// Returned <first diff char> is the index of the first (UTF-8)
		// character that differs between the two.
		// NOTE: Compares on UTF-8 codepoints within the strings!
		struct diff_compare
		{
			int operator()( const substring& a, const substring& b ) const noexcept;
		};
		static const diff_compare DiffCompare;

		// Predicate for comparing two substrings lexiographically ignoring case differences,
		// returning 0 if equal, -<first diff char> if less-than, and +<first diff char> if
		// greather-than.
		// Returned <first diff char> is the index of the first (UTF-8)
		// character that differs between the two.
		// NOTE: Compares on UTF-8 codepoints within the strings!
		// NOTE: A custom locale can be provided if the default Eon locale
		//       currently set (for the thread) should be overridden.
		class icase_compare
		{
		public:
			inline icase_compare( const eon::locale* custom_locale = nullptr ) {
				Loc = custom_locale != nullptr ? custom_locale : &eon::locale::get(); }
			int operator()( const substring& a, const substring& b ) const noexcept;
		private:
			const eon::locale* Loc{ nullptr };
		};
		static const icase_compare ICaseCompare;

		// Predicate for comparing two characters while ignoring case difference.
		// Returns -1 for less-than, 0 for equal-to, and 1 fore greater-than.
		// NOTE: A custom locale can be provided if the default Eon locale
		//       currently set (for the thread) should be overridden.
		class icase_chr_compare
		{
		public:
			inline icase_chr_compare( const eon::locale* custom_locale = nullptr ) {
				Loc = custom_locale != nullptr ? custom_locale : &eon::locale::get();
			}
			inline int operator()( char_t a, char_t b ) const noexcept {
				char_t lw_a = Loc->toLower( static_cast<wchar_t>( a ) ), lw_b = Loc->toLower( static_cast<wchar_t>( b ) );
				return lw_a < lw_b ? -1 : lw_a == lw_b ? 0 : 1; }
		private:
			const eon::locale* Loc{ nullptr };
		};
		static const icase_chr_compare ICaseChrCompare;


		// Check if substring starts with a specific substring 'value'.
		// Comparison is done using a binary substring predicate.
		template<typename compare_T = fast_compare>
		inline bool startsWith( const substring& value, const compare_T& cmp = FastCompare ) const noexcept
		{
			if( value.numChars() == numChars() )
				return cmp( *this, value ) == 0;
			else if( value.numChars() < numChars() )
				return cmp( substring( begin(), begin() + value.numChars() ), value ) == 0;
			else
				return false;
		}

		// Check if substring starts with a specific character.
		// Comparison is done using a binary char_t predicate.
		template<typename compare_T = fast_chr_compare>
		inline bool startsWith( char_t value, const compare_T& cmp = FastChrCompare ) const noexcept {
			return cmp( *begin(), value ) == 0; }

		// Check if substring ends with a specific substring.
		// Comparison is done using a binary substring predicate.
		template<typename compare_T = fast_compare>
		inline bool endsWith( const substring& value, const compare_T& cmp = FastCompare ) const noexcept
		{
			if( value.numChars() == numChars() )
				return cmp( *this, value ) == 0;
			else if( value.numChars() < numChars() )
				return cmp( substring( end() - value.numChars(), end() ), value ) == 0;
			else
				return false;
		}

		// Check if substring ends with a specific character.
		// Comparison is done using a binary char_t predicate.
		template<typename compare_T = fast_chr_compare>
		inline bool endsWith( char_t value, const compare_T& cmp = FastChrCompare ) const noexcept {
			return cmp( *last(), value ) == 0; }

		// Do normal comparison.
		// Assumes both 'this' and 'other' are 'low-to-high'!
		// Comparison is done using a binary substring predicate.
		template<typename compare_T = fast_compare>
		int compare( const substring& other, const compare_T& cmp = FastCompare ) const noexcept {
			return cmp( *this, other ); }

		// Check if 'this' substring sorts before 'other' using [eon::substring::FastCompare].
		inline bool operator<( const substring& other ) const noexcept { return compare( other, FastCompare ) < 0; }

		// Check if 'this' substring sorts before or same as 'other' using [eon::substring::FastCompare].
		inline bool operator<=( const substring& other ) const noexcept { return compare( other, FastCompare ) <= 0; }

		// Check if 'this' substring sorts after 'other' using [eon::substring::FastCompare].
		inline bool operator>( const substring& other ) const noexcept { return compare( other, FastCompare ) > 0; }

		// Check if 'this' substring sorts after or same as 'other' using [eon::substring::FastCompare].
		inline bool operator>=( const substring& other ) const noexcept { return compare( other, FastCompare ) >= 0; }

		// Check if 'this' substring sorts same as 'other' using [eon::substring::FastCompare].
		inline bool operator==( const substring& other ) const noexcept { return compare( other, FastCompare ) == 0; }

		// Check if 'this' substring sorts before or after 'other' using [eon::substring::FastCompare].
		inline bool operator!=( const substring& other ) const noexcept { return compare( other, FastCompare ) != 0; }


		// Do partial comparison from start of substrings.
		// Assumes both 'this' and 'other' are 'low-to-high'!
		// Comparison is done using a binary char_t predicate.
		template<typename compare_T = fast_chr_compare>
		int compareSub( const substring& other, index_t num_chars, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			if( !*this )
				return other ? -1 : 0;
			else if( !other )
				return 1;

			string_iterator i = Beg, other_i = other.Beg;
			int pos{ 1 }, cmp_val{ 0 };
			for( ; i != End && other_i != other.end() && pos <= num_chars; ++i, ++other_i, ++pos )
			{
				cmp_val = cmp( *i, *other_i );
				if( cmp_val != 0 )
					return cmp_val < 0 ? -pos : pos;
			}
			return static_cast<index_t>( pos - 1 ) == num_chars ? 0 : i != End ? pos : -pos;
		}


		// Compare by iterator positions, rather than characters.
		// Returns -1 if 'this' starts before 'other' or same start but ends before.
		// Returns 0 if start and end positions are identical.
		// Returns 1 if 'this' starts after 'other' of same start but ends after.
		inline int comparePos( const substring& other ) const noexcept {
			return Beg < other.Beg ? -1 : other.Beg < Beg ? 1 : End < other.End ? -1 : other.End < End ? 1 : 0; }




		///////////////////////////////////////////////////////////////////////
		//
		// Searching
		//
	public:

		// Check if substring contains the specified substring.
		// Comparison is done using a binary substring predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastCompare] is used!
		template<typename compare_T = fast_compare>
		inline bool contains( const substring& sub, const compare_T& cmp = FastCompare ) const noexcept {
			return static_cast<bool>( findFirst( sub, cmp ) ); }

		// Check if substring contains the specified codepoint.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		inline bool contains( char_t codepoint, const compare_T& cmp = FastChrCompare ) const noexcept {
			return static_cast<bool>( findFirst( codepoint, cmp ) ); }

		// Check if substring contains any of the characters in the specified substring.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		inline bool containsAnyOf( const substring& sub, const compare_T& cmp = FastChrCompare ) const noexcept {
			return static_cast<bool>( findFirstOf( sub, cmp ) ); }

		// Check if substring contains any characters other than the ones in the specified substring.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		inline bool containsOtherThan( const substring& sub, const compare_T& cmp = FastChrCompare ) const noexcept {
			return static_cast<bool>( findFirstNotOf( sub, cmp ) ); }


		// Find first occurrence of 'to_find' substring in 'this'.
		// Both substrings must be 'low-to-high'! (No checking is done!)
		// Returns the found substring within 'this' - ''false' substring if not found.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		substring findFirst( const substring& to_find, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			if( empty() || to_find.empty() )
				return substring( End.getEnd() );
			if( Beg.bytesOnly() && (void*)&cmp == (void*)&FastChrCompare )
				return _optimizedFindFirst( to_find );
			for( string_iterator i = begin(); i != end(); ++i )
			{
				string_iterator i_beg = i, j = to_find.begin();
				for( ; j != to_find.end() && i != end() && cmp( *i, *j ) == 0; ++j, ++i )
					;
				if( j == to_find.end() )
					return substring( i_beg, i );
			}
			return substring( End.getEnd() );
		}

		// Find first occurrence of 'to_find' codepoint in 'this'.
		// The substring must be 'low-to-high'! (No checking is done!)
		// Returns the found codepoint as a substring - 'false' substring if not found.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		substring findFirst( char_t to_find, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			if( empty() )
				return substring( End.getEnd() );
			if( Beg.bytesOnly() && &cmp == &FastChrCompare )
				return _optimizedFindFirst( to_find );
			for( string_iterator i = begin(); i != end(); ++i )
			{
				if( cmp( *i, to_find ) == 0 )
					return substring( i, i + 1 );
			}
			return substring( End.getEnd() );
		}


		// Find the first number within the substring.
		// Will use 'custom_locale' if specified and the default Eon locale if not!
		// Returns a substring wrapping only the found number, 'false' if not found.
		// If the 'separator' iterator argument is specified, it will contain
		// the position of the decimal separator - if it exists.
		// NOTE: If the decimal separator appears before the first numeral, it will not be counted as part of the number!
		substring findFirstNumber( string_iterator* separator = nullptr, const locale* custom_locale = nullptr );


		// Find last occurrence of 'to_find' substring in 'this'.
		// Both substrings must be 'high-to-low'! (No checking is done!)
		// Returns the found substring - 'false' substring if not found.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		substring findLast( const substring& to_find, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			if( empty() || to_find.empty() )
				return substring( End.getEnd() );
			if( Beg.bytesOnly() && (void*)&cmp == (void*)&FastChrCompare )
				return _optimizedFindLast( to_find );
			for( string_iterator i = begin(); i != end(); --i )
			{
				string_iterator i_beg = i, j = to_find.begin();
				for( ; j != to_find.end() && i != end() && cmp( *i, *j ) == 0; --j, --i )
					;
				if( j )
					return substring( i_beg, i );
			}
			return substring( End.getEnd() );
		}

		// Find last occurrence of 'to_find' codepoint in 'this'.
		// The substring must be 'high-to-low'! (No checking is done!)
		// Returns the found codepoint as a substring - 'false' substring if not found.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		substring findLast( char_t to_find, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			if( empty() )
				return substring( End.getEnd() );
			if( Beg.bytesOnly() && (void*)&cmp == (void*)&FastChrCompare )
				return _optimizedFindLast( to_find );
			for( string_iterator i = Beg; i != End; --i )
			{
				if( cmp( *i, to_find ) == 0 )
					return substring( i, i + 1 );
			}
			return substring( End.getEnd() );
		}


		// Find first occurrence of any of the specified 'characters'.
		// NOTE: 'this' substring must be 'low-to-high' ordered! (No checking is done!)
		// Returns iterator for the found character - 'false' iterator if not found.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		string_iterator findFirstOf( const substring& characters, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			for( string_iterator i = begin(); i != end(); ++i )
			{
				if( characters.contains( *i, cmp ) )
					return i;
			}
			return end().getEnd();
		}

		// Find first occurrence of any character in the specified character class(es).
		// NOTE: 'this' substring must be 'low-to-high' ordered! (No checking is done!)
		// NOTE: Multiple classes can be or'ed together!
		// Returns iterator for the found character - 'false' iterator if not found.
		string_iterator findFirstOf( charcat category ) const noexcept;


		// Find last occurrence of any of the specified 'characters'.
		// NOTE: 'this' substring must be 'high-to-low' ordered! (No checking is done!)
		// Returns iterator for the found character - 'false' iterator if not found.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		string_iterator findLastOf( const substring& characters, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			for( auto i = begin(); i != end(); --i )
			{
				if( characters.contains( *i, cmp ) )
					return i;
			}
			return end().getEnd();
		}

		// Find last occurrence of any character in the specified character class(es).
		// NOTE: 'this' substring must be 'high-to-low' ordered! (No checking is done!)
		// NOTE: Multiple classes can be or'ed together!
		// Returns iterator for the found character - 'false' iterator if not found.
		string_iterator findLastOf( charcat category ) const noexcept;


		// Find first occurrence of any character not among the specified 'characters'.
		// NOTE: 'this' substring must be 'low-to-high' ordered! (No checking is done!)
		// Returns iterator for the found character - 'false' iterator if not found.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		string_iterator findFirstNotOf( const substring& characters, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			for( auto i = begin(); i != end(); ++i )
			{
				if( !characters.contains( *i, cmp ) )
					return i;
			}
			return end().getEnd();
		}

		// Find first occurrence of any character not in the specified character class(es).
		// NOTE: 'this' substring must be 'low-to-high' ordered! (No checking is done!)
		// NOTE: Multiple classes can be or'ed together!
		// Returns iterator for the found character - 'false' iterator if not found.
		string_iterator findFirstNotOf( charcat category ) const noexcept;


		// Find last occurrence of any character not among the specified 'characters'.
		// NOTE: 'this' substring must be 'high-to-low' ordered! (No checking is done!)
		// Returns iterator for the found character - 'false' iterator if not found.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		string_iterator findLastNotOf( const substring& characters, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			for( auto i = begin(); i != end(); --i )
			{
				if( !characters.contains( *i, cmp ) )
					return i;
			}
			return end().getEnd();
		}

		// Find last occurrence of any character not in the specified character class(es).
		// NOTE: 'this' substring must be 'high-to-low' ordered! (No checking is done!)
		// NOTE: Multiple classes can be or'ed together!
		// Returns iterator for the found character - 'false' iterator if not found.
		string_iterator findLastNotOf( charcat category ) const noexcept;


		// Find the first character in 'this' substring that differs from 'other',
		// 'false' iterator if none (equal substrings).
		// Both substrings must be 'low-to-high'!
		// Comparison is done using a binary char_t predicate.
		template<typename compare_T = fast_chr_compare>
		string_iterator findFirstDiff( const substring& other, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			auto i = begin(), j = other.begin();
			for( ; i != end() && j != other.end(); ++i, ++j )
			{
				if( cmp( *i, *j ) != 0 )
					return i;
			}
			if( i != end() )
				return i;
			else
				return end().getEnd();
		}


		// Similar to [findFirst] for substring argument but sections enclosed
		// by matching 'start_sect' and 'end_sect' characters will be skipped.
		// For example, quoted sections, parenthesized sections, etc.
		// NOTE: If 'start_sect' is different from 'end_sect', nested occurrences will be accounted for!
		// NOTE: If 'start_sect' is identical to 'end_sect', escaped versions of the character within or
		//       outside an enclosed section will not be interpreted as neither 'start_sect' nor 'end_sect'!
		// NOTE: Both substrings must be 'low-to-high'!
		// NOTE: if 'end_sect' is 'same_char', it will be the same as 'start_sect'!
		// Comparison is done using a binary substring predicate.
		template<typename compare_T = fast_chr_compare>
		substring findFirstIgnoreSections(
			const substring& other,
			char_t start_sect,
			char_t end_sect = same_char,
			const compare_T& cmp = FastChrCompare ) const noexcept
		{
			end_sect = end_sect == same_char ? start_sect : end_sect;
			bool escaped = false;
			for( string_iterator i = begin(); i < end(); ++i )
			{
				if( escaped )
					escaped = false;
				else
				{
					if( start_sect == end_sect && *i == BackSlashChr )
						escaped = true;
					else if( *i == start_sect )
						_bypassSection( start_sect, end_sect, i );
					else
					{
						string_iterator j = other.begin(), k = i;
						for( ; j != other.end() && k != end() && cmp( *k, *j ) == 0; ++j, ++k )
							;
						if( j == other.end() )
							return substring( i, k );
					}
				}
			}
			return substring( End.getEnd() );
		}

		// Similar to [findFirst] for character argument but sections enclosed
		// by matching 'start_sect' and 'end_sect' characters will be skipped.
		// For example, quoted sections, parenthesized sections, etc.
		// NOTE: If 'start_sect' is different from 'end_sect', nested occurrences will be accounted for!
		// NOTE: If 'start_sect' is identical to 'end_sect', escaped versions of the character within or
		//       outside an enclosed section will not be interpreted as neither 'start_sect' nor 'end_sect'!
		// NOTE: Both substrings must be 'low-to-high'!
		// NOTE: if 'end_sect' is 'same_char', it will be the same as 'start_sect'!
		// Comparison is done using a binary char_t predicate.
		template<typename compare_T = fast_chr_compare>
		substring findFirstIgnoreSections(
			char_t codepoint,
			char_t start_sect,
			char_t end_sect = same_char,
			const compare_T& cmp = FastChrCompare ) const noexcept
		{
			end_sect = end_sect == same_char ? start_sect : end_sect;
			bool escaped = false;
			for( string_iterator i = begin(); i != end(); ++i )
			{
				if( escaped )
					escaped = false;
				else
				{
					if( start_sect == end_sect && *i == BackSlashChr )
						escaped = true;
					else if( *i == start_sect )
						_bypassSection( start_sect, end_sect, i );
					else if( cmp( *i, codepoint ) == 0 )
						return substring( i, i + 1 );
				}
			}
			return substring( End.getEnd() );
		}


		// Find first occurrence of a character not from a [charcat] character 'category'.
		// NOTE: The substring must be 'low-to-high'!
		string_iterator findFirstOtherThan( charcat category ) const noexcept;

		// Find last occurrence of a character not from a [charcat] character 'category'.
		// NOTE: The substring must be 'high-to-low'!
		string_iterator findLastOtherThan( charcat category ) const noexcept;


		// Get as new substring everything inside 'this' substring that appears
		// before the first occurrence of 'delimiter' substring.
		// Comparison is done using a binary substring predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastCompare] is used!
		template<typename compare_T = fast_compare>
		substring beforeFirst( const substring& delimiter, const compare_T& cmp = FastCompare ) const noexcept
		{
			auto found = findFirst( delimiter, cmp );
			if( found )
				return substring( begin(), found.begin() );
			else
				return substring( end() );
		}

		// Get as new substring everything inside 'this' substring that appears
		// before the first occurrence of 'delimiter' character.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		substring beforeFirst( char_t delimiter, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			auto found = findFirst( delimiter, cmp );
			if( found )
				return substring( begin(), found.begin() );
			else
				return substring( end() );
		}


		// Get as new substring everything inside 'this' substring appearing
		// before the last occurrence of 'delimiter' substring.
		// Comparison is done using a binary substring predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastCompare] is used!
		template<typename compare_T = fast_compare>
		substring beforeLast( const substring& delimiter, const compare_T& cmp = FastCompare ) const noexcept
		{
			auto found = highToLow().findLast( delimiter, cmp );
			if( found )
				return substring( begin(), found.begin() );
			else
				return substring( end() );
		}

		// Get as new substring everything inside 'this' substring appearing
		// before the last occurrence of 'delimiter' character.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		substring beforeLast( char_t delimiter, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			auto found = highToLow().findLast( delimiter, cmp );
			if( found )
				return substring( begin(), found.begin() );
			else
				return substring( end() );
		}


		// Get as new substring everything inside 'this' substring appearing
		// after the first occurrence of 'delimiter' substring.
		// Comparison is done using a binary substring predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastCompare] is used!
		template<typename compare_T = fast_compare>
		substring afterFirst( const substring& delimiter, const compare_T& cmp = FastCompare ) const noexcept
		{
			auto found = findFirst( delimiter, cmp );
			if( found )
				return substring( found.end(), end() );
			else
				return substring( end() );
		}

		// Get as new substring everything inside 'this' substring appearing
		// after the first occurrence of 'delimiter' character.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		substring afterFirst( char_t delimiter, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			auto found = findFirst( delimiter );
			if( found )
				return substring( found.end(), end() );
			else
				return substring( end() );
		}


		// Get as new substring everything inside 'this' substring appearing
		// after the last occurrence of 'delimiter' substring.
		// Comparison is done using a binary substring predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastCompare] is used!
		template<typename compare_T = fast_compare>
		substring afterLast( const substring& delimiter, const compare_T& cmp = FastCompare ) const noexcept
		{
			auto found = highToLow().findLast( delimiter, cmp );
			if( found )
				return substring( found.end(), end() );
			else
				return substring( end() );
		}

		// Get as new substring everything inside 'this' substring appearing
		// after the last occurrence of 'delimiter' character.
		// Comparison is done using a binary char_t predicate.
		// NOTE: ASCII-only optimization is only done if [eon::substring::FastChrCompare] is used!
		template<typename compare_T = fast_chr_compare>
		substring afterLast( char_t delimiter, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			auto found = highToLow().findLast( delimiter, cmp );
			if( found )
				return substring( found.end(), end() );
			else
				return substring( end() );
		}




		///////////////////////////////////////////////////////////////////////
		//
		// Counting
		//
	public:

		// Count occurrences of a character.
		// The substring must be 'low-to-high'!
		// Comparison is done using a binary char_t predicate.
		template<typename compare_T = fast_chr_compare>
		index_t count( char_t to_count, const compare_T& cmp = FastChrCompare ) const noexcept
		{
			index_t cnt = 0;
			for( string_iterator i = begin(); i != end(); ++i )
			{
				if( cmp( *i, to_count ) == 0 )
					++cnt;
			}
			return cnt;
		}

		// Count occurrences of a sub-string.
		// Both substrings must be 'low-to-high'!
		// NOTE: All occurrences are counted, including overlaps!
		// Comparison is done using a binary substring predicate.
		template<typename compare_T = fast_compare>
		index_t count( const substring& to_count, const compare_T& cmp = FastCompare ) const noexcept
		{
			if( numBytes() > 0 && numBytes() < to_count.numChars() )
				return 0;
			index_t cnt = 0;
			auto found = findFirst( to_count, cmp );
			for( ; found; ++cnt )
				found = substring( found.begin() + 1, end() ).findFirst( to_count, cmp );
			return cnt;
		}




		///////////////////////////////////////////////////////////////////////
		//
		// Helpers
		//
	private:

		inline substring _optimizedFindFirst( const substring& to_find ) const noexcept
		{
			// If the sub-string isn't bytes-only, we know it cannot exist within the source!
			if( !to_find.begin().bytesOnly() )
				return substring( End.getEnd() );

			auto found = _findFirst( Beg.Pos, numBytes(), to_find.begin().Pos, to_find.numBytes() );
			if( found != nullptr )
			{
				return substring( string_iterator( Beg, found, found - Beg.Source ),
					string_iterator( Beg, found + to_find.numBytes(), ( found + to_find.numBytes() ) - Beg.Source ) );
			}
			else
				return substring( End.getEnd() );
		}

		inline substring _optimizedFindFirst( char_t to_find ) const noexcept
		{
			// If the codepoint isn't ASCII, then only accept search if 'this' also isn't UTF-8!
			if( to_find > 127 && Beg.ValidUTF8 )
				return substring( End.getEnd() );

			auto found = _findFirst( Beg.Pos, numBytes(), static_cast<char>( to_find ) );
			if( found != nullptr )
				return substring( string_iterator( Beg, found, found - Beg.Source ),
					string_iterator( Beg, found + 1, ( found + 1 ) - Beg.Source ) );
			else
				return substring( End.getEnd() );
		}

		inline substring _optimizedFindLast( const substring& to_find ) const noexcept
		{
			// If the sub-string isn't bytes-only, we know it cannot exist within the source!
			if( !to_find.begin().bytesOnly() )
				return substring( End.getEnd() );

			auto found = _findLast( Beg.Pos, numBytes(), to_find.begin().Pos, to_find.numBytes() );
			if( found != nullptr )
			{
				return substring( string_iterator( Beg, found, found - Beg.Source ),
					string_iterator( Beg, found + to_find.numBytes(), ( found + to_find.numBytes() ) - Beg.Source ) );
			}
			else
				return substring( End.getEnd() );
		}

		inline substring _optimizedFindLast( char_t to_find ) const noexcept
		{
			// If the codepoint isn't ASCII, then only accept search if 'this' also isn't UTF-8!
			if( to_find > 127 && Beg.ValidUTF8 )
				return substring( End.getEnd() );

			auto found = _findLast( Beg.Pos, numBytes(), static_cast<char>( to_find ) );
			if( found != nullptr )
			{
				if( found < Beg.SourceEnd )
					return substring( string_iterator( Beg, found, found - Beg.Source ),
						string_iterator( Beg, found + 1, ( found + 1 ) - Beg.Source ) );
				else
					return substring( string_iterator( Beg, found,
						found - Beg.Source ), End );
			}
			else
				return substring( End.getEnd() );
		}

		void _bypassSection( char_t start_sect, char_t end_sect, string_iterator& i ) const;

		static inline const char* _findFirst( const char* str, index_t str_size, char c ) noexcept {
			return (char*)memchr( str, c, str_size ); }
		const char* _findFirst( const char* source, index_t source_size, const char* substr, index_t substr_size )
			const noexcept;

		static const char* _findLast( const char* str, index_t str_size, char chr ) noexcept;
		const char* _findLast( const char* source, index_t source_size, const char* substr, index_t substr_size )
			const noexcept;

		std::string _getReverse() const;



		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	private:

		string_iterator Beg, End;

		enum class Flag : uint8_t
		{
			none = 0x00,
			icase = 0x01,
			force_favor_speed = 0x02,
			force_favor_humans = 0x04,
			force_favor_diff = force_favor_speed | force_favor_humans
		};
		Flag Flags{ Flag::none };
	};
}
