#pragma once
#include "UniChar.h"
#include "Substring.h"
#include <set>
#include <list>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <cmath>



///////////////////////////////////////////////////////////////////////////////
// 
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	// Reference to a codepoint in a UTF-8 string.
	struct Utf8CharRef
	{
		// Number of bytes from start of string.
		index_t BytePos{ 0 };

		// Number of UTF-8 characters from start of string.
		// NOTE: This will always be equal to or greater than [eon::Utf8CharRef::BytePos]!
		index_t CharPos{ 0 };
	};




	///////////////////////////////////////////////////////////////////////////
	//
	// Eon UTF-8 String Class - eon::string
	//
	// A fully implemented string class with a variety of useful methods for
	// string manipulation.
	//
	// NOTE: Unlike [std::string], eon::string will not let you modify characters in-place.
	// NOTE: Direct access to characters is costly, prefer using iterators!
	// WARNING: Is not thread-safe!
	//
	// Makes heavy use of [eon::string_iterator] and [eon::substring] classes.
	// The use of the latter sets eon::string apart from [std::string] in that
	// [eon::substrings] are not "real" strings but a pair of [eon::iterator]s
	// that marks the substring in the original source string.
	// While [eon::string] has many useful methods, these typically delegates
	// to [eon::substring] for the actual work.
	//
	// NOTE: [eon::substring] will often have more detailed/specialized methods \
	//       that what's available from [eon::string]!
	// NOTE: Use [eon::string::substr()] to get the entire string as a [eon::substring]!
	//
	// We can also get the entire string as a [eon::substring] by calling
	// substring's constructor with calls to [eon::string::begin] and
	// [eon::string::end] as arguments.
	//
	class string
	{
		///////////////////////////////////////////////////////////////////////
		//
		// String Definitions
		//
	public:

		// For consistency with [std::string] and std containers, we define an
		// iterator class inside the string class.
		// NOTE: eon::string::iterator is identical to [eon::string_iterator]!
		using iterator = string_iterator;




		///////////////////////////////////////////////////////////////////////
		//
		// String Construction
		//
	public:

		// Construct an empty string.
		string() = default;
		
		// Construct by taking ownership of the details of another string
		inline string( string&& other ) noexcept { *this = std::move( other ); }

		// Construct as a copy of another string
		inline string( const string& other ) { *this = other; }


		// Construct as a copy of a substring marked by the specified [eon::string::iterator] pair.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( const iterator& input_beg, const iterator& input_end ) {
			*this = substring( input_beg, input_end ); }

		// Construct as a copy of a [eon::substring].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( const substring& input ) { *this = input; }


		// Construct as a copy of a [std::string].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( const std::string& input ) { *this = input; }

		// Construct by taking ownership of a [std::string].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( std::string&& input ) { *this = std::move( input ); }

		// Construct as a copy of a C string.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( const char* input ) { *this = input; }

		// Construct as a copy of a C string of specified length.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( const char* input, index_t input_length ) { *this = std::string( input, input_length ); }

		// Construct as a copy of a C string of specified length.
		// Will not throw exception on invalid UTF-8 parts but substitute those with the specified substitution string.
		string( const char* input, index_t input_size, string non_utf8_substitution ) noexcept;


		// Construct from a single [eon::char_t] Unicode codepoint.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline explicit string( char_t input ) { *this = input; }

		// Construct from a single signed character.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline explicit string( char input ) { *this = static_cast<char_t>( input ); }

		// Construct from a single unsigned character.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline explicit string( unsigned char input ) { *this = static_cast<char_t>( input ); }


		// Construct from literal list of [eon::char_t] Unicode codepoints.
		// WARNING: Throws eon::[eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( std::initializer_list<char_t> input ) { *this = input; }

		// Construct from vector of [eon::char_t] Unicode codepoints.
		// WARNING: Throws eon::[eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( const std::vector<char_t>& input ) { *this = input; }

		// Construct from literal list of signed characters.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( std::initializer_list<char> input ) { *this = input; }

		// Construct from literal list of unsigned characters.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( std::initializer_list<unsigned char> input ) { *this = input; }


		// Construct as a number of copies of the specified [eon::char_t] Unicode codepoint.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( index_t copies, char_t input ) { assign( copies, input ); }

		// Construct as a number of copies of another string.
		inline string( index_t copies, const string& other ) { assign( copies, other ); }

		// Construct as a number of copies of the specified [std::string].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( index_t copies, const std::string& input ) { assign( copies, input ); }

		// Construct as a number of copies of the specified [eon::substring].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string( index_t copies, const substring& input ) { assign( copies, input ); }


		// Construct as "true" or "false" based on boolean input value.
		inline explicit string( bool value ) { *this = value; }


		// Construct as string representation of the input [eon::int_t].
		inline explicit string( int_t input ) { *this = toString( input ); }

		// Construct as string representation of the input [eon::short_t].
		inline explicit string( short_t value ) { *this = toString( value ); }

		// Construct as string representation of the input [eon::long_t].
		inline explicit string( long_t value ) { *this = toString( value ); }

		// Construct as string representation of the input [eon::index_t].
		inline explicit string( index_t value ) { *this = toString( value ); }

		// Construct as string representation of the input [eon::flt_t].
		inline explicit string( flt_t value ) { *this = toString( value ).trimFloat(); }

		// Construct as string representation of the input [eon::low_t].
		inline explicit string( low_t value ) { *this = toString( value ).trimFloat(); }

		// Construct as string representation of the input [eon::high_t].
		inline explicit string( high_t value ) { *this = toString( value ).trimFloat(); }

#ifdef EON_WINDOWS

		// Construct as string representation of the input [std::uint16_t].
		inline explicit string( uint16_t value ) { *this = toString( value ); }

		// Construct as string representation of the input [std::uint32_t].
		inline explicit string( uint32_t value ) { *this = toString( value ); }

		// Construct as string representation of the input [long].
		inline explicit string( long value ) { *this = toString( value ); }

		// Construct as string representation of the input [unsigned long].
		inline explicit string( unsigned long value ) { *this = toString( value ); }

#else
		// Construct as string representation of the input [unsigned short].
		inline explicit string( unsigned short value ) { *this = toString( value ); }

		// Construct as string representation of the input [unsigned int].
		inline explicit string( unsigned int value ) { *this = toString( value ); }

#	ifdef EON_SUN
		// Construct as string representation of the input [long].
		inline explicit string( long value ) { *this = toString( value ); }

		// Construct as string representation of the input [unsigned long].
		inline explicit string( unsigned long value ) { *this = toString( value ); }

#	else
		// Construct as string representation of the input [std::int8_t].
		inline explicit string( int8_t value ) { *this = toString( value ); }

		// Construct as string representation of the input [long long unsigned int].
		inline explicit string( long long unsigned int value ) { *this = toString( value ); }

#		ifdef EON_APPLE
		// Construct as string representation of the input [long].
		inline explicit string( long value ) { *this = toString( value ); }

#		endif
#	endif
#endif


		// Default destructor.
		virtual ~string() = default;


	private:

		// Constructor for internal use, where we know the input string is ASCII only.
		// (Faster than normal construction!)
		inline string( std::string&& value, bool identifier ) noexcept {
			Bytes = std::move( value ); NumChars = Bytes.size(); }




		///////////////////////////////////////////////////////////////////////
		//
		// String Assignment Metods
		//
	public:

		// Discard current details and copy the input [eon::char_t] codepoints as UTF-8.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicoded codepoints!
		string& assign( const char_t* input, index_t input_length );

		// Discard current details and copy the input C string.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		string& assign( const char* input, index_t input_length );


		// Discard current details and assign the specified number of copies of the [eon::char_t] codepoint.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoint!
		string& assign( index_t copies, char_t input );

		// Discard current details and assign the specified number of copies of the character.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoint!
		inline string& assign( index_t copies, char input ) { return assign( copies, static_cast<char_t>( input ) ); }

		// Discard current details and assign the specified number of copies of the other string.
		string& assign( index_t copies, const string& other );

		// Discard current details and assign the specified number of copies of the [std::string].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		string& assign( index_t copies, const std::string& input );

		// Discard current details and assign the specified number of copies of the [eon::substring].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8
		string& assign( index_t copies, const substring& input );


		// Discard current details and copy those of another string.
		inline string& operator=( const string& other ) { Bytes = other.Bytes; NumChars = other.NumChars; return *this; }

		// Discard current details and take over ownership of those of another string.
		inline string& operator=( string&& other ) noexcept {
			Bytes = std::move( other.Bytes ); NumChars = other.NumChars; other.NumChars = 0; return *this; }


		// Discard current details and copy new from an [eon::substring].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		string& operator=( const substring& input );


		// Discard current details and copy new from a [std::string].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string& operator=( const std::string& input ) { return assign( input.c_str(), input.size() ); }

		// Discard current details and take over ownership of a [std::string].
		// NOTE: This is possible because [std::string] is an underlying structure of [eon::string].
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		string& operator=( std::string&& input );

		// Discard current details and copy new from a C string.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string& operator=( const char* input ) { return assign( input, strlen( input ) ); }

		// Descard current details and use a single [eon::char_t] codepoint as new value.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoint!
		inline string& operator=( char_t input ) {
			uint32_t bytes; auto num = iterator::unicodeToBytes( input, bytes ); return assign( (const char*)&bytes, num ); }

		// Discard current details and use a single signed character as new value.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoint!
		inline string& operator=( char input ) { return *this = static_cast<char_t>( input ); }

		// Discard current details and use a single unsigned character as new value.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoint!
		inline string& operator=( unsigned char input ) { return *this = static_cast<char_t>( input ); }


		// Discard current details and use a literal list of codepoints as new value.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoints!
		string& operator=( const std::initializer_list<char_t>& input );

		// Discard current details and use a vector of codepoints as new value.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoints!
		string& operator=( const std::vector<char_t>& input );

		// Discard current details and use a literal list of signed characters as new value.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoints!
		string& operator=( const std::initializer_list<char>& input );

		// Discard current details and use a literal list of unsigned characters as new value.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid Unicode codepoints!
		string& operator=( const std::initializer_list<unsigned char>& input );


		// Assign as "true" or "false" based on the boolean value.
		inline string& operator=( bool input ) { return *this = input ? "true" : "false"; }
		



		///////////////////////////////////////////////////////////////////////
		//
		// Basic Read-only Methods
		//
	public:

		// Get number of [eon::char_t] (Unicode codepoints) in string.
		inline index_t numChars() const noexcept { return NumChars; }

		// Get number of [eon::char_t] (Unicode codepoints) in string.
		inline index_t length() const noexcept { return NumChars; }


		// Get number of bytes in string.
		inline index_t numBytes() const noexcept { return Bytes.size(); }

		// Get buffer size (capacity) in number of bytes.
		inline index_t bufferSize() const noexcept { return Bytes.capacity(); }

		// Check if the string is empty.
		inline bool empty() const noexcept { return NumChars == 0; }

		// Access the underlying bytes as a [std::string].
		// NOTE: This is intant since [eon::string] uses std::string as underlying structure!
		inline const std::string& stdstr() const noexcept { return Bytes; }

		// Access the underlying bytes as a C-string.
		// NOTE: This is the same as calling [eon::string::stdstr()] and then [std::string::c_str] on the returned value!
		inline const char* c_str() const noexcept { return Bytes.c_str(); }

		// Access an individual immutable byte by index.
		// NOTE: This is instant, but the byte may be part of a larger Unicode character!
		inline char byte( index_t pos ) const { return Bytes[ pos ]; }


		// Check if the string is enclosed by double quotation marks.
		inline bool isDoubleQuoted() const noexcept { return substr().enclosed( DblQuoteChr ); }

		// Check if a part of the string is enclosed by double quotation marks.
		inline bool isDoubleQuoted( const substring& sub ) const noexcept { return sub.enclosed( DblQuoteChr ); }

		// Check if the string is enclosed by single quotation marks.
		inline bool isSingleQuoted() const noexcept { return substr().enclosed( SglQuoteChr ); }

		// Check if a part of the string is enclosed by single quotation marks.
		inline bool isSingleQuoted( const substring& sub ) const noexcept { return sub.enclosed( SglQuoteChr ); }


		// Check if the string is 'blank' (contains only whitespaces).
		inline bool blank() const noexcept { return substr().blank(); }

		// Get number of indentations for the first (and possibly only) line of text in the string.
		// NOTE: This is the number of times the indentation character occurs at the start of the string!
		inline index_t indentationLevel( char_t indentation_char = TabChr ) const noexcept {
			return substr().indentationLevel(); }


		// Get string as a vector of [eon::char_t] Unicode codepoints.
		inline std::vector<char_t> chars() const { return substr().chars(); }

		// Get part of string as a vector of [eon::char_t] Unicode codepoints.
		inline std::vector<char_t> chars( const substring& sub ) const { return sub.lowToHigh().chars(); }




		///////////////////////////////////////////////////////////////////////
		//
		// Iterator and Iterator Access
		//
	public:

		// Get iterator for first character in the string.
		// Returns [eon::string::end] if string is empty.
		inline iterator begin() const noexcept { return iterator( Bytes.c_str(), Bytes.size(), NumChars, Bytes.c_str() ); }

		// Get iterator for end of string (one past the last character).
		inline iterator end() const noexcept {
			return iterator( Bytes.c_str(), Bytes.size(), NumChars, Bytes.c_str() + Bytes.size(), NumChars ); }

		// Get iterator for the last character in the string.
		// Returns [eon::string::end] if string is empty.
		inline iterator last() const noexcept { return NumChars > 0 ? end() - 1 : end(); }


		// Given a byte position within the string, get the (closest next) character
		// position in the form of an iterator.
		// The counting will start from the beginning of the string,
		// unless an already known position is specified as 'start'.
		// Returns [eon::string::end] if either 'pos' is beyond the end of the string,
		// or the byte position of 'start' is beyond 'pos'.
		iterator bytePos( index_t pos, iterator start = iterator() ) const;


		// Given an [eon::string::iterator] for another string, rebase it to 'this'.
		// (Make a new iterator that refers to the same position in 'this' string.)
		// Returns [eon::string::end] if the iterator from the other string is beyond what 'this' string contains.
		// This method is safe but involves linear counting!
		iterator rebase( const iterator& pos_from_other_string ) const noexcept;

		// Given an [eon::substring] of another string, rebase it to 'this'.
		// (Make a new substring that refers to the same substring in 'this' string.)
		// This method is safe, but involves linear counting!
		inline substring rebase( const substring& other ) const noexcept {
			return substring( rebase( other.begin() ), rebase( other.end() ) ); }

		// Given an [eon::string::iterator] for another string, rebase it to 'this'.
		// (Make a new iterator that refers to the same position in 'this' string.)
		// Returns [eon::string::end] if the iterator from the other string is beyond what 'this' string contains.
		// WARNING: While very fast (no counting), used unwisely, it can result in undefined behavior!
		// You have to know for sure that both strings are identical (at least
		// up to and including the [eon::char_t] Unicode codepoint referenced by the iterator).
		iterator rebaseMoved( const iterator& other ) const noexcept;

		// Given an [eon::substring] of another string, rebase it to 'this'.
		// (Make a new substring that refers to the same substring in 'this' string.)
		// WARNING: While very fast (no counting), used unwisely, it can result in undefined behavior!
		// You have to know for sure that both strings are identical (at least
		// up to and including the area of the substring.
		inline substring rebaseMoved( const substring& other ) const noexcept {
			return substring( rebaseMoved( other.begin() ), rebaseMoved( other.end() ) ); }


		// Assuming the 'encoded_iterator' string is formatted as {"<byte pos>:<char pos>"},
		// then convert it into an actual [eon::string::iterator] for 'this' string.
		// If the encoded iterator does not refer to a valid position in the string,
		// the returned value will be equal to that returned by [eon::string::end)]!
		// (See matching [eon::string::encode] method.)
		iterator decodeIterator( const string& encoded_iterator );

		// Assuming the 'encoded_substring' string is formatted as {"<byte pos>:<char pos>-<byte pos>:<char pos>"},
		// then convert it into an action [eon::substring] for 'this' string.
		// If the encoded substring does not refer to a valid portion in the string,
		// the returned value will be an 'empty' substring!
		// (See matching [eon::string::encode] method.)
		substring decodeSubstring( const string& encoded_substring );

		// Convert an [eon::string_iterator] into a string formatted as {"<byte pos>:<char pos>"}.
		// NOTE: This is useful for streaming, serializing, and storing.
		// (See matching [eon::string::decodeIterator] method.)
		static inline string encode( const string_iterator& itr ) {
			return toString( itr.numByte() ) + ":" + toString( itr.numChar() ); }

		// Convert an [eon::substring] into a string formatted as {"<byte pos>:<char pos>-<byte pos>:<char pos>"}.
		// NOTE: This is useful for streaming, serializing, and storing.
		// (See matching [eon::string::decodeIterator] method.)
		static inline string encode( const substring& sub ) { return encode( sub.begin() ) + "-" + encode( sub.end() ); }




		///////////////////////////////////////////////////////////////////////
		//
		// Searching
		//
		// Methods searching for substrings will return [eon::substring] objects
		// as result. These will be "false" for unsuccessful searches.
		//
	public:

		// Check if string contains the specified [eon::char_t] Unicode codepoint.
		inline bool contains( char_t cp ) const noexcept { return substr().contains( cp ); }

		// Check if string contains the specified substring.
		inline bool contains( const string& sub ) const noexcept { return substr().contains( sub.substr() ); }

		// Check if string contains any of the charcters in the 'characters' string.
		inline bool containsAnyOf( const string& characters ) const noexcept {
			return substr().containsAnyOf( characters.substr() ); }

		// Check if string contains any charcters other than the ones in the 'characters' string.
		inline bool containsOtherThan( const string& characters ) const noexcept {
			return substr().containsOtherThan( characters.substr() ); }


		// Find first occurrence of another (equal or shorter) string.
		inline substring findFirst( const string& to_find ) const noexcept { return substr().findFirst( to_find.substr() ); }

		// Find first occurrence of another string, but limit search of 'this' to the specified 'sub' [eon::substring].
		inline substring findFirst( const string& to_find, const substring& sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.lowToHigh().findFirst( to_find.substr() ); }

		// Find first occurrence of a [eon::char_t] Unicode codepoint.
		inline substring findFirst( char_t to_find ) const noexcept { return substr().findFirst( to_find ); }

		// Find first occurrence of a [eon::char_t] Unicode codepoint, but limit
		// search of 'this' to the specified 'sub' [eon::substring].
		inline substring findFirst( char_t to_find, const substring& sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.lowToHigh().findFirst( to_find ); }

		// Find first occurrence of an ASCII character.
		inline substring findFirst( char to_find ) const noexcept {
			return substr().findFirst( static_cast<char_t>( to_find ) ); }

		// Find first occurrence of an ASCII character, but limit search of 'this'
		// to the specified 'sub' [eon::substring].
		inline substring findFirst( char to_find, const substring& sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.lowToHigh().findFirst( static_cast<char_t>( to_find ) ); }


		// Find first occurrence of any character in 'characters'.
		inline iterator findFirstOf( const string& characters ) const noexcept {
			return substr().findFirstOf( characters.substr() ); }
		
		// Find first occurrence of any character in 'characters', but limit
		// search of 'this' to the specified 'sub' [eon::substring].
		inline iterator findFirstOf( const string& characters, substring sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.findFirstOf( characters.substr() ); }


		// Find first occurrence of any character not in 'characters'.
		inline iterator findFirstNotOf( const string& characters ) const noexcept {
			return substr().findFirstNotOf( characters.substr() ); }

		// Find first occurrence of any character not in 'characters', but
		// limit search of 'this' to the specified 'sub' [eon::substring].
		inline iterator findFirstNotOf( const string& characters, substring sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() );  return sub.findFirstNotOf( characters.substr() ); }


		// Find first occurrence of another string, but skip past sections
		// of 'this' enclosed by double quotes. Any back-slashed quote mark in
		// 'this' will be ignored (not counted as quotation mark).
		inline substring findFirstNotDoubleQuoted( const string& other ) const noexcept {
			return substr().findFirstIgnoreSections( other.substr(), DblQuoteChr ); }

		// Find first occurrence of [eon::char_t] Unicode codepoint, but skip past
		// sections of 'this' enclosed by double quotes. Any back-slashed quote
		// mark in 'this' will be ignored (not counted as quotation mark).
		inline substring findFirstNotDoubleQuoted( char_t cp ) const noexcept {
			return substr().findFirstIgnoreSections( cp, DblQuoteChr ); }

		// Find first occurrence of another string, but skip past sections
		// of 'this' enclosed by single quotes. Any back-slashed quote mark in
		// 'this' will be ignored (not counted as quotation mark).
		inline substring findFirstNotSingleQuoted( const string& other ) const noexcept {
			return substr().findFirstIgnoreSections( other.substr(), SglQuoteChr ); }

		// Find first occurrence of [eon::char_t] Unicode codepoint, but skip past
		// sections of 'this' enclosed by single quotes. Any back-slashed quote
		// mark in 'this' will be ignored (not counted as quotation mark).
		inline substring findFirstNotSingleQuoted( char_t cp ) const noexcept {
			return substr().findFirstIgnoreSections( cp, SglQuoteChr ); }

		// Find first occurrence of another string, but skip past sections
		// of 'this' enclosed by '(' and ')', '[' and ']' or '{' and '}'.
		// Nested braces will be accounted for.
		// See [eon::substring::findFirstIgnoreSections] for how to customize
		// searches.
		inline substring findFirstNotBraced( const string& other, char_t brace = '(' ) const noexcept {
			return substr().findFirstIgnoreSections( other.substr(), brace,
				brace == '(' ? ')' : brace == '[' ? ']' : brace == '{' ? '}' : brace ); }

		// Find first occurrence of [eon::char_t] Unicode codepoint, but skip past
		// sections of 'this' enclosed by '(' and ')', '[' and ']' or '{' and '}'.
		// Nested braces will be accounted for.
		// See [eon::substring::findFirstIgnoreSections] for how to customize
		// searches.
		inline substring findFirstNotBraced( char_t cp, char_t brace = '(' ) const noexcept {
			return substr().findFirstIgnoreSections(
				cp, brace, brace == '(' ? ')' : brace == '[' ? ']' : brace == '{' ? '}' : brace ); }


		// Find the first character in 'this' that differs from the 'other'
		// string. If both strings are equal (no diff), then [eon::string::end]
		// is returned.
		inline iterator findFirstDiff( const string& other ) const noexcept {
			return substr().findFirstDiff( other.substr() ); }

		// Find the first character in 'this' that differs from the 'other'
		// [eon::substring]. If both strings are equal (no diff), then
		// [eon::string::end] is returned.
		inline iterator findFirstDiff( const substring& sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return substr().findFirstDiff( sub ); }

		
		// Find first character in 'this' that is from the specified [eon::charcat] 'category'.
		inline iterator findFirst( charcat category ) const noexcept { return substr().findFirst( category ); }

		// Find first character in 'this' that is from the specified [eon::charcat]
		// 'category', limit the search of 'this' to 'sub' [eon::substring].
		inline iterator findFirst( charcat category, const substring& sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.findFirst( category ); }


		// Find last occurrence of another (equal or shorter) string.
		inline substring findLast( const string& to_find ) const noexcept {
			return substr().highToLow().findLast( to_find.substr() ); }

		// Find last occurrence of another string, but limit search of 'this'
		// to the specified 'sub' [eon::substring].
		inline substring findLast( const string& to_find, substring sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.highToLow().findLast( to_find.substr().highToLow() ); }

		// Find last occurrence of a [eon::char_t] Unicode codepoint.
		inline substring findLast( char_t to_find ) const noexcept { return substr().highToLow().findLast( to_find ); }

		// Find last occurrence of a codepoint, but limit search of 'this' to
		// the specified 'sub' [eon::substring].
		inline substring findLast( char_t to_find, substring sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.highToLow().findLast( to_find ); }

		// Find last occurrence of an ASCII character.
		inline substring findLast( char to_find ) const noexcept {
			return substr().highToLow().findLast( static_cast<char_t>( to_find ) ); }

		// Find last occurrence of an ASCII character, but limit search of
		// 'this' to the specified 'sub' [eon::substring].
		inline substring findLast( char to_find, substring sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.highToLow().findLast( static_cast<char_t>( to_find ) ); }


		// Find last occurrence of any character in 'characters'.
		inline iterator findLastOf( const string& characters ) const noexcept {
			return substr().highToLow().findLastOf( characters.substr() ); }
		
		// Find last occurrence of any character in 'characters', but limit
		// search of 'this' to the specified 'sub' [eon::substring].
		inline iterator findLastOf( const string& characters, substring sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.highToLow().findLastOf( characters.substr() ); }
		

		// Find last occurrence of any character not in 'characters'.
		inline iterator findLastNotOf( const string& characters ) const noexcept {
			return substr().highToLow().findLastNotOf( characters.substr() ); }

		// Find last occurrence of any character not in 'characters', but
		// limit search of 'this' to the specified 'sub' [eon::substring].
		inline iterator findLastNotOf( const string& characters, substring sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.highToLow().findLastNotOf( characters.substr() ); }


		// Find last character in 'this' that is from the specified [eon::charcat] 'category'.
		inline iterator findLast( charcat category ) const noexcept {
			return substr().highToLow().findLast( category ); }

		// Find last character in 'this' that is from the specified [eon::charcat]
		// 'category', limit the search to 'sub' [eon::substring].
		inline iterator findLast( charcat category, const substring& sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.highToLow().findLast( category ); }




		///////////////////////////////////////////////////////////////////////
		//
		// Counting
		//
	public:

		// Count number of occurrences of the specified [eon::char_t] Unicode codepoint.
		inline index_t count( char_t to_count ) const noexcept { return substr().count( to_count ); }

		// Count number of occurrences of the specified [eon::char_t] Unicode codepoint,
		// limit the counting to the specified 'sub' [eon::substring] of 'this'.
		inline index_t count( char_t to_count, const substring& sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.count( to_count ); }

		// Count number of occurrences of the specified 'to_count' string.
		// NOTE: All occurrences are counted, even overlaps!
		inline index_t count( const string& to_count ) const noexcept { return substr().count( to_count.substr() ); }

		// Count number of occurrences of the specified string, limit the
		// counting to the specified 'sub' [eon::substring] of 'this'.
		// NOTE: All occurrences are counted, even overlaps!
		inline index_t count( const string& to_count, const substring& sub ) const noexcept {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.count( to_count.substr() ); }




		///////////////////////////////////////////////////////////////////////
		//
		// Modifier Methods
		//
		// These will modify the string object!
		//
	public:

		// Clear the string contents
		inline void clear() noexcept { Bytes.clear(); NumChars = 0; }

		// Reserve memory in order to reduce the number of times the
		// underlying string buffer may have to grow.
		// Does nothing if the buffer size is already >= 'byte_size'.
		inline void reserve( index_t byte_size ) { Bytes.reserve( byte_size ); }


		// Concatenate another string to 'this' by serialization.
		// NOTE: {str << a << b << c ...} is more efficient than {str += a + b + c + ...}.
		inline string& operator<<( const string& other ) { return *this += other; }

		// Concatenate an [eon::substring] sting to 'this' by serialization.
		// NOTE: {str << a << b << c ...} is more efficient than {str += a + b + c + ...}.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string& operator<<( const substring& input ) { return *this += input; }

		// Concatenate a std::string to 'this' by serialization.
		// NOTE: {str << a << b << c ...} is more efficient than {str += a + b + c + ...}.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string& operator<<( const std::string& input ) { return *this += input; }

		// Concatenate C string to 'this' by serialization.
		// NOTE: {str << a << b << c ...} is more efficient than {str += a + b + c + ...}.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		inline string& operator<<( const char* input ) { return *this += input; }

		// Concatenate any object type that can be given as argument to a string constructor to 'this' by serialization.
		// NOTE: {str << a << b << c ...} is more efficient than {str += a + b + c + ...}.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		template<typename T>
		inline string& operator<<( const T& input ) { return *this += string( input ); }


		// Concatenate another string to 'this'.
		inline string& operator+=( const string& other ) { Bytes += other.Bytes; NumChars += other.NumChars; return *this; }

		// Concatenate an [eon::substring] to 'this'.
		// WARNING: Throws [eon::InvalidUTF8] if input is not valid UTF-8!
		string& operator+=( const substring& input );

		// Concatenate a std::string to 'this'.
		// WARNING: Throws [eon::InvalidUTF8] if not a valid UTF-8!
		inline string& operator+=( const std::string& input ) { return *this += substring( input ); }

		// Concatenate a C-string to 'this'.
		// WARNING: Throws [eon::InvalidUTF8] if not a valid UTF-8!
		inline string& operator+=( const char* input ) { return *this += substring( input ); }

		// Concatenate an [eon::char_t] Unicode codpoint to 'this'.
		// WARNING: Throws [eon::InvalidUTF8] if not a valid UTF-8!
		inline string& operator+=( char_t input ) { return *this += string( input ); }

		// Concatenate a signed char to 'this'.
		// WARNING: Throws [eon::InvalidUTF8] if not a valid UTF-8!
		inline string& operator+=( char input ) { return *this += string( input ); }

		// Concatenate an unsigned char to 'this'.
		// WARNING: Throws [eon::InvalidUTF8] if not a valid UTF-8!
		inline string& operator+=( unsigned char input ) { return *this += string( input ); }


		// Insert an [eon::substring] into 'this' at the specified [eon::string::iterator] 'pos'
		// position, moving existing data from (and including) the iterator position outwards.
		// Returns a new iterator for the insert position (as the old will be invalid after insert).
		iterator insert( const iterator& pos, const string& sub );

		// Insert an [eon::char_t] Unicode codepoint into 'this' at the specified [eon::string::iterator]
		// 'pos' position, moving existing data from (and including) the iterator position outwards.
		// Returns a new iterator for the insert position (as the old will be invalid after insert).
		inline iterator insert( const iterator& pos, char_t cp ) { return insert( pos, string( cp ) ); }

		// Erase an [eon::substring] section from the string.
		string& erase( const substring& sub ) noexcept;




		///////////////////////////////////////////////////////////////////////
		//
		// Copy and Transform
		//
		// These methods will not modify the string object, but produce a new
		// one - or an [eon::substring]!
		//
		// For methods returning [eon::substring], you can easily convert the
		// substring into an independent string by using the substring as
		// argument to string constructor or assignment method/operator.
		//
	public:

		// Get a copy of the string with all letters transformed to upper-case.
		inline string upper() const { return upper( substr() ); }

		// Get a copy of the string with only letters in the 'sub'
		// [eon::substring] of 'this' transformed to upper-case.
		string upper( const substring& sub ) const;


		// Get a copy of the string with all letters transformed to lower case.
		inline string lower() const { return lower( substr() ); }

		// Get a copy of the string with only letters in the 'sub'
		// [eon::substring] of 'this' transformed to lower-case.
		string lower( const substring& sub ) const;


		// Get a copy of the string with first letter transformed to upper-case.
		inline string ucFirst() const { return ucFirst( substr() ); }
		
		// Get a copy of the string with first letter of the 'sub'
		// [eon::substring] of 'this' transformed to upper-case.
		string ucFirst( const substring& sub ) const;


		// Get a copy of the string with first letter of every word transformed
		// to upper-case. Other letters will not be modified!
		inline string ucWords() const { return ucWords( substr() ); }
		
		// Get a copy of the string with first letter of every word in 'sub' [eon::substring]
		// of 'this' transformed to upper-case. Other letters will not be modified!
		string ucWords( const substring& sub ) const;


		// Get a copy of the string with first letter of every sentence transformed
		// to upper-case. Other letters will not be modified!
		// NOTE: Only '.', ':', '!' and '?' are considered sentence dividers!
		inline string ucSentences() const { return ucSentences( substr() ); }

		// Get a copy of the string with first letter of every sentence in 'sub' [eon::substring]
		// of 'this' transformed to upper-case. Other letters will not be modified!
		// NOTE: Only '.', ':', '!' and '?' are considered sentence dividers!
		string ucSentences( const substring& sub ) const;


		// Get entire string as an [eon::substring].
		inline substring substr() const { return substring( begin(), end() ); }

		// Get [eon::substring] starting at the 'start' [eon::string::iterator] and running to the end of the string.
		inline substring substr( const iterator& start ) const {
			start.assertSameBuffer( Bytes.c_str() ); return substring( start, end() ); }

		// Get [eon::substring] starting at the 'start' [eon::string::iterator] and running to the 'end' iterator.
		inline substring substr( const iterator& start, const iterator& end ) const {
			start.assertSameBuffer( Bytes.c_str() ); end.assertSameBuffer( Bytes.c_str() ); return substring( start, end ); }


		// Get [eon::substring] based on character position and count.
		// NOTE: This involves counting characters, but will count from the end if closer than start!
		inline substring substr( index_t start, index_t size ) const {
			auto sub = substr(); return substring( sub.iterator( start ), sub.iterator( start + size ) ); }


		// Get an [eon::substring] (slice) based on character position and
		// count from start (positive value) and/or from end (negative value).
		// NOTE: Both 'start' and 'end' are inclusive!
		// A slice {0, -1} will include the entire string, while {2, 2} will
		// return a substring containing a single character.
		// NOTE: If 'start' is less than 'end', a 'high-to-low' ordered substring is returned!
		substring slice( int64_t start, int64_t end ) const;


		// Get the part of the string that is before the first occurrence of 'delimiter' as an [eon::substring].
		inline substring beforeFirst( const string& delimiter ) const noexcept {
			return substr().beforeFirst( delimiter.substr() ); }

		// Get the part of the string that is before the first occurrence of 'delimiter' as an [eon::substring].
		inline substring beforeFirst( char_t delimiter ) const noexcept { return substr().beforeFirst( delimiter ); }

		// Get the part of the string that is before the last occurrence of 'delimiter' as an [eon::substring].
		inline substring beforeLast( const string& delimiter ) const noexcept {
			return substr().beforeLast( delimiter.substr() ); }

		// Get the part of the string that is before the last occurrence of 'delimiter' as an [eon::substring].
		inline substring beforeLast( char_t delimiter ) const noexcept { return substr().beforeLast( delimiter ); }

		// Get the part of the string that is after the first occurrence of 'delimiter' as an [eon::substring].
		inline substring afterFirst( const string& delimiter ) const noexcept {
			return substr().afterFirst( delimiter.substr() ); }

		// Get the part of the string that is after the first occurrence of 'delimiter' as an [eon::substring].
		inline substring afterFirst( char_t delimiter ) const noexcept { return substr().afterFirst( delimiter ); }

		// Get the part of the string that is after the last occurrence of 'delimiter' as an [eon::substring].
		inline substring afterLast( const string& delimiter ) const noexcept {
			return substr().afterLast( delimiter.substr() ); }

		// Get the part of the string that is after the last occurrence of 'delimiter' as an [eon::substring].
		inline substring afterLast( char_t delimiter ) const noexcept { return substr().afterLast( delimiter ); }


		// Get a copy of the entire string with all occurrences of 'find' replaced with 'replacement'.
		inline string replace( const string& find, const string& replacement ) const {
			return replace( find, replacement, substr() ); }

		// Get a copy of the entire string with all occurrences of 'find'
		// within 'sub' [eon::substring] of 'this' replaced with 'replacement'.
		string replace( const string& find, const string& replacement, const substring& sub ) const;

		// Get a copy of the entire string with all occurrences of 'find' replaced with 'replacement'.
		inline string replace( char_t find, char_t replacement ) const { return replace( find, replacement, substr() ); }
		
		// Get a copy of the entire string with all occurrences of 'find'
		// within 'sub' [eon::substring] of 'this' replaced with 'replacement'.
		string replace( char_t find, char_t replacement, const substring& sub ) const;

		// Get a copy of the entire string with all occurrences of 'find' replaced with 'replacement'.
		inline string replace( char find, char replacement ) const {
			return replace( static_cast<char_t>( find ), static_cast<char_t>( replacement ) ); }

		// Get a copy of the entire string with all occurrences of 'find'
		// within 'sub' [eon::substring] of 'this' replaced with 'replacement'.
		inline string replace( char find, char replacement, const substring& sub ) const {
			return replace( static_cast<char_t>( find ), static_cast<char_t>( replacement ), sub ); }
		
		// Get a copy of the entire string with 'sub' [eon::substring] replaced with 'replacement'.
		inline string replace( const substring& sub, const string& replacement ) const {
			return string( substr( begin(), sub.begin() ) ) + replacement + string( substr( sub.end() ) ); }

		// Get a copy of the entire string with 'sub' [eon::substring] replaced with 'replacement'.
		inline string replace( const substring& sub, char_t replacement ) const {
			return string( substr( begin(), sub.begin() ) ) + replacement + string( substr( sub.end() ) ); }


		// Get a copy of the entire string with each occurrence of any map
		// 'key' replaced by the corresponding map 'value'.
		inline string replace( const std::map<string, string>& find_replace ) const {
			return replace( find_replace, substr() ); }

		// Get a copy of the entire string with each occurrence of any map 'key' within
		// 'sub' [eon::substring] of 'this' replaced by the corresponding map 'value'.
		string replace( const std::map<string, string>& find_replace, const substring& sub ) const;

		// Get a copy of the entire string with each occurrence of any map
		// 'key' replaced by the corresponding map 'value'.
		inline string replace( const std::map<char_t, char_t>& find_replace ) const {
			return replace( find_replace, substr() ); }
		
		// Get a copy of the entire string with each occurrence of any map 'key' within
		// 'sub' [eon::substring] of 'this' replaced by the corresponding map 'value'.
		string replace( const std::map<char_t, char_t>& find_replace, const substring& sub ) const;


		// Get a copy of the entire string with all occurrences of 'to_remove' removed.
		inline string remove( const string& to_remove ) const { return replace( to_remove, string::Empty, substr() ); }

		// Get a copy of the entire string with all occurrences of 'to_remove'
		// within 'sub' [eon::substring] of 'this' removed.
		inline string remove( const string& to_remove, const substring& sub ) const {
			return replace( to_remove, string::Empty, sub ); }

		// Get a copy of the entire string with all occurrences of 'to_remove' removed.
		inline string remove( char_t to_remove ) const { return replace( string( to_remove ), string::Empty, substr() ); }

		// Get a copy of the entire string with all occurrences of 'to_remove'
		// within 'sub' [eon::substring] of 'this' removed.
		inline string remove( char_t to_remove, const substring& sub ) const {
			return replace( string( to_remove ), string::Empty, sub ); }

		// Get a copy of the entire string with all occurrences of 'to_remove' removed.
		inline string remove( char to_remove ) const { return replace( string( to_remove ), string::Empty, substr() ); }

		// Get a copy of the entire string with all occurrences of 'to_remove'
		// within 'sub' [eon::substring] of 'this' removed.
		inline string remove( char to_remove, const substring& sub ) const {
			return replace( string( to_remove ), string::Empty, sub ); }


		// Get a copy of the string with standard C single character escapes processed.
		// NOTE: Single quotes, question marks, and tabs are not escaped!
		inline string escape() const { return escape( substr() ); }

		// Get a copy of the string with standard C single character escapes
		// processed only within 'sub' [eon::substring] of 'this'.
		// NOTE: Single quotes, question marks, and tabs are not escaped!
		string escape( const substring& sub ) const;

		// Get a copy of the string with standard C single character escapes processed.
		// NOTE: Single quotes, question marks, and tabs are also escaped!
		inline string escapeAll() const { return escapeAll( substr() ); }

		// Get a copy of the string with standard C single character escapes
		// processed only within 'sub' [eon::substring] of 'this'.
		// NOTE: Single quotes, question marks, and tabs are also escaped!
		string escapeAll( const substring& sub ) const;

		// Get a copy of the string with standard C single character escapes reverse processed.
		inline string unescape() const { return unescape( substr() ); }

		// Get a copy of the string with standard C single character escapes
		// reverse processed only within 'sub' [eon::substring] of 'this'.
		string unescape( const substring& sub ) const;


		// Get a copy of the string with all non-printable ASCII characters
		// and all non-ASCII characters replaced with HTML formatting.
		// Example: "\t" -> "&#9;"
		inline string escapeNonPrintable() const { return escapeNonPrintable( substr() ); }

		// Get a copy of the string with all non-printable ASCII characaters and all non-ASCII
		// characters inside 'sub' [eon::substring] of 'this' replaced with HTML formatting.
		// Example: "\t" -> "&#9;"
		string escapeNonPrintable( const substring& sub ) const;

		// Get a copy of the string with all HTML formatted characters
		// replaced with the actual characaters they represent.
		// Example: "&#9;" -> "\t"
		inline string unescapeNonPrintable() const { return unescapeNonPrintable( substr() ); }

		// Get a copy of the string with all HTML formatted characters inside 'sub'
		// [eon::substring] of 'this' replaced with the actual characaters they represent.
		// Example: "&#9;" -> "\t"
		string unescapeNonPrintable( const substring& sub ) const;


		// Get a copy of the string enclosed in double quotes.
		// NOTE: Will not add quotation marks if already existing at both ends!
		inline string doubleQuote() const { return doubleQuote( substr() ); }

		// Get a copy of the string with the 'sub' [eon::substring] of 'this' enclosed in double quotes.
		// NOTE: Will not add quotation marks if already existing at both ends!
		string doubleQuote( const substring& sub ) const;

		// Get a copy of the string enclosed in single quotes.
		// NOTE: Will not add quotation marks if already existing at both ends!
		inline string singleQuote() const { return singleQuote( substr() ); }

		// Get a copy of the string with the 'sub' [eon::substring] of 'this' enclosed in single quotes
		// NOTE: Will not add quotation marks if already existing at both ends!
		string singleQuote( const substring& sub ) const;

		// Get a copy of the string with double or single quotations marks removed.
		// NOTE: Will only remove if the same quotation character is at both ends!
		inline string unQuote() const { return unQuote( substr() ); }

		// Get a copy of the string with double or single quotations marks on
		// the 'sub' [eon::substring] of 'this' removed.
		// NOTE: Will only remove if the same quotation character is at both ends!
		string unQuote( const substring& sub ) const;


		// Get a copy of the string with all characters in reverse order.
		inline string reverse() const { return reverse( substr() ); }
		
		// Get a copy of the string with all characters in the 'sub' [eon::substring]
		// of 'this' in reverse order. Other characters are not modified!
		string reverse( const substring& sub ) const;


		// Split string on every occurrence of 'delimiter' (delimiter string not included in result)
		// into a sequential container (such as std::list or std::vector). Uses the 'push_back' method!
		// If a 'max_elements' limit is reached, the last element will reference the unsplit remainder.
		// NOTE: All elements will be substring objects so specify the template type as such!
		// Example: {splitSequential<std::list<eon::substring>>( ...}
		template<typename container_t>
		container_t splitSequential( const string& delimiter, index_t max_elements = INDEX_MAX ) const {
			return substr().splitSequential<container_t>( delimiter.substr(), max_elements ); }

		// Split string on every occurrence of 'delimiter' (delimiter char not included in result)
		// into a sequential container (such as std::list or std::vector). Uses the 'push_back' method!
		// If a 'max_elements' limit is reached, the last element will reference the unsplit remainder.
		// NOTE: All elements will be substring objects so specify the template type as such!
		// Example: {splitSequential<std::list<eon::substring>>( ...}
		template<typename container_t>
		container_t splitSequential( char_t delimiter, index_t max_elements = INDEX_MAX ) const {
			return substr().splitSequential<container_t>( delimiter, max_elements ); }

		// Split string on every occurrence of 'delimiter' (delimiter string not included in result)
		// into a non-sequential container (such as std::set or std::multiset). Uses the 'insert' method!
		// NOTE: All elements will be substring objects so specify the template type as such!
		// Example: {splitNonSequential<std::set<eon::substring>>( ...}
		template<typename container_t>
		container_t splitNonSequential( const string& delimiter ) const {
			return substr().splitNonSequential<container_t>( delimiter.substr() ); }

		// Split string on every occurrence of 'delimiter' (delimiter char not included in result)
		// into a non-sequential container (such as std::set or std::multiset). Uses the 'insert' method!
		// NOTE: All elements will be substring objects so specify the template type as such!
		// Example: {splitNonSequential<std::set<eon::substring>>( ...}
		template<typename container_t>
		container_t splitNonSequential( char_t delimiter ) const {
			return substr().splitNonSequential<container_t>( delimiter ); }


		// Get a string that joins the elements from 'start' to 'end' using 'this' string as 'glue' in-between.
		template<typename iterator_t>
		string join( iterator_t start, iterator_t end ) const
		{
			string str;
			for( auto i = start; i != end; ++i )
			{
				if( i != start )
					str += *this;
				str += *i;
			}
			return str;
		}

		// Get a string that joins the elements of the 'source' container using 'this' string as 'glue' in-between.
		template<typename container_t>
		string join( const container_t& source ) const {
			return join<typename container_t::const_iterator>( source.begin(), source.end() ); }


		// Get an [eon::substring] with leading and trailing spaces of 'this' excluded.
		inline substring trim() const { return substr().trim(); }

		// Get an [eon::substring] with leading spaces of 'this' excluded.
		inline substring trimLeading() const { return substr().trimLeading(); }

		// Get an [eon::substring] with trailing spaces of 'this' excluded.
		inline substring trimTrailing() const { return substr().trimTrailing(); }


		// Get a copy of the string, if it is shorter than 'target_size', add enough copies of
		// 'fill' [eon::char_t] Unicode codepoint on the left side to make it that long.
		inline string padLeft( index_t target_size, char_t fill = SpaceChr ) const {
			return NumChars < target_size ? string( target_size - NumChars, fill ) += *this : *this; }

		// Get a copy of the string, if it is shorter than 'target_size', add enough copies of
		// 'fill' [eon::char_t] Unicode codepoint on the right side to make it that long.
		inline string padRight( index_t target_size, char_t fill = SpaceChr ) const {
			return NumChars < target_size ? *this + string( target_size - NumChars, fill ) : *this; }

		// Get a copy of the string, if it is shorter than 'target_size', add enough copies of
		// 'fill' [eon::char_t] Unicode codepoint on the left and right side to make it that long.
		// NOTE: If an uneven number of fills are needed, the left end will be the shorter one!
		// (Centers the string)
		string padLeftAndRight( index_t target_size, char_t fill = SpaceChr ) const;


		// Get a copy of the string with all lines 'indented'.
		// NOTE: If the first line is already at the specified 'indentation_level', it will not be indented further!
		// NOTE: Subsequent lines will be indented fully regardless of existing indentation!
		// NOTE: Empty lines will not be indented!
		string indentLines( index_t indentation_level, char_t indentation_char = TabChr ) const;


		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( const string& a, const string& b ) { return string( a ) += b; }

		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( const string& a, const substring& b ) { return string( a ) += b; }

		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( const substring& a, const string& b ) { return string( a ) += b; }

		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( const string& a, const std::string& b ) { return string( a ) += string( b ); }

		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( const std::string& a, const string& b ) { return string( a ) += b; }

		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( const string& a, const char* b ) { return string( a ) += string( b ); }

		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( const char* a, const string& b ) { return string( a ) += b; }

		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( const string& a, char_t b ) { return string( a ) += b; }

		// Return a new string that is the result of 'a' concatenated with 'b'.
		friend inline string operator+( char_t a, const string& b ) { return string( a ) += b; }




		///////////////////////////////////////////////////////////////////////
		//
		// Strings as Numbers
		//
		// These methods provide various number operations to strings.
		//
	public:

		// Check if all characaters are numerals (including non-ASCII numerals).
		inline bool numeralsOnly() const noexcept { return substr().numeralsOnly(); }

		// Check if all characaters in 'sub' [eon::substring] of 'this' are numerals (including non-ASCII numerals).
		inline bool numeralsOnly( const substring& sub ) const {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.numeralsOnly(); }


		// Check if all characters are ASCII digits (only!).
		inline bool isUInt() const noexcept { return substr().isUInt(); }

		// Check if all characters in 'sub' [eon::substring] of 'this' are ASCII digits (only!).
		inline bool isUInt( const substring& sub ) const { sub.assertSameBuffer( Bytes.c_str() ); return sub.isUInt(); }


		// Check if all characters are ASCII digits (only!), prefixed by a single '+' or '-'.
		inline bool isInt() const noexcept { return substr().isInt(); }

		// Check if all characters in 'sub' [eon::substring] of 'this' are
		// ASCII digits (only!), prefixed by a single '+' or '-'.
		inline bool isInt( const substring& sub ) const { sub.assertSameBuffer( Bytes.c_str() ); return sub.isInt(); }


		// Check if the string makes up an ASCII floating point number, optionally prefixed by a single '+' or '-'.
		// NOTE: The 'decimal_separator' may occur as first or last character!
		inline bool isFloat( char_t decimal_separator = PointChr ) const noexcept {
			return substr().isFloat( decimal_separator ); }

		// Check if the [eon::substring] of 'this' makes up an ASCII floating point number,
		// optionally prefixed by a single '+' or '-'.
		// NOTE: The 'decimal_separator' may occur as first or last character!
		inline bool isFloat( const substring& sub, char_t decimal_separator = PointChr ) const {
			sub.assertSameBuffer( Bytes.c_str() ); return sub.isFloat( decimal_separator ); }


		// Convert String to [eon::int_t].
		inline int_t toInt() const { return substr().toInt(); }

		// Convert String to [eon::short_t].
		inline short_t toShort() const { return substr().toShort(); }

		// Convert String to [eon::long_t].
		inline long_t toLong() const { return substr().toLong(); }

		// Convert String to [eon::flt_t].
		inline flt_t toFloat() const { return substr().toFloat(); }

		// Convert String to [eon::low_t].
		inline low_t toLow() const { return substr().toLow(); }

		// Convert String to [eon::hight_t].
		inline high_t toHigh() const { return substr().toHigh(); }

		// Convert String to [eon::index_t].
		inline index_t toIndex() const { return substr().toIndex(); }


		// Convert String to number.
		inline int32_t toInt32() const { return substr().toInt32(); }

		// Convert String to number.
		inline int32_t toInt32( const substring& area ) const {
			area.assertSameBuffer( Bytes.c_str() ); return area.toInt32(); }

		// Convert String to number.
		inline int64_t toInt64() const { return substr().toInt64(); }

		// Convert String to number.
		inline int64_t toInt64( const substring& area ) const {
			area.assertSameBuffer( Bytes.c_str() ); return area.toInt64(); }

		// Convert String to number.
		inline uint32_t toUInt32() const { return substr().toUInt32(); }

		// Convert String to number.
		inline uint32_t toUInt32( const substring& area ) const {
			area.assertSameBuffer( Bytes.c_str() ); return area.toUInt32(); }

		// Convert String to number.
		inline uint64_t toUInt64() const { return substr().toUInt64(); }

		// Convert String to number.
		inline uint64_t toUInt64( const substring& area ) const {
			area.assertSameBuffer( Bytes.c_str() ); return area.toUInt64(); }

		// Convert String to number.
		inline size_t toSize() const { return substr().toSize(); }

		// Convert String to number.
		inline size_t toSize( const substring& area ) const {
			area.assertSameBuffer( Bytes.c_str() ); return area.toSize(); }

		// Convert String to number.
		inline double toDouble() const { return substr().toDouble(); }

		// Convert String to number.
		inline double toDouble( const substring& area ) const {
			area.assertSameBuffer( Bytes.c_str() ); return area.toDouble(); }

		// Convert String to number.
		inline long double toLongDouble() const { return substr().toLongDouble(); }

		// Convert String to number.
		inline long double toLongDouble( const substring& area ) const {
			area.assertSameBuffer( Bytes.c_str() ); return area.toLongDouble(); }


		// Assuming the entire substring is a number, get a reduced [eon::substring] where leading
		// integer zeros and trailing fractional zeros are removed - including the
		// 'decimal_separator' if all fractional digits are zeros.
		// (If the number ends with the separator, it will be excluded.)
		// NOTE: If the number is signed, leading integer zeros will be included to keep the sign!
		// Example: {0001.0000 -> 1}
		// Example: {+02.10 -> +02.1}
		inline substring trimNumber( char_t decimal_separator = PointChr ) const {
			return substr().trimNumber( decimal_separator ); }

		// Assuming the entire substring is a number, get a reduced [eon::substring] where leading
		// integer zeros and trailing fractional zeros are removed. If all fractional digits are
		// zeros, keep the 'decimal_separator' and one zero.
		// (If the number ends with the separator, it will be included.)
		// NOTE: If the number is signed, leading integer zeros will be included to keep the sign!
		// Example: {0001.0000 -> 1.0}
		// Example: {+02.10 -> +02.1}
		inline substring trimFloat( char_t decimal_separator = PointChr ) const {
			return substr().trimFloat( decimal_separator ); }


		// Get a copy of the number string with the integer part separated into thousands.
		// Example: 34525525.4621 -> 34,525,525.4621
		string separateThousands( char_t thousands_sep = CommaChr, char_t decimal_separator = PointChr ) const;

		// Get a copy of the number string with fractional part reduced to 'max_decimals',
		// rounded up or down to nearest value. If there is no fractional part or it is
		// already at the same or fewer number of decimals, an unaltered copy is returned.
		// Zero 'max_decimals' will result in removal of the fractional part and
		// possibly an increase of the integer part (if rounding up).
		string roundNumber( index_t max_decimals = 0, char_t decimal_separator = PointChr ) const;




		///////////////////////////////////////////////////////////////////////
		//
		// String hashing
		//
		// Uses the FNV-1a hash algorithm
		//
	public:

		// Get a 32-bit hash value.
		inline uint32_t hash32() const noexcept { return substring::hash32( Bytes.c_str(), Bytes.c_str() + Bytes.size() ); }

		// Get a 64-bit hash value.
		inline uint64_t hash64() const noexcept { return substring::hash64( Bytes.c_str(), Bytes.c_str() + Bytes.size() ); }

#if defined(_WIN64) || defined(__x86_64__)
		// Get a 'size_t'size hash value.
		inline size_t hash() const noexcept { return substring::hash64( Bytes.c_str(), Bytes.c_str() + Bytes.size() ); }
#else
		// Get a 'size_t'size hash value.
		inline size_t hash() const noexcept { return substring::hash32( Bytes.c_str(), Bytes.c_str() + Bytes.size() ); }
#endif




		///////////////////////////////////////////////////////////////////////
		//
		// Comparing
		//
	public:

		// Check if the string starts with a specific 'value'.
		inline bool startsWith( const string& value ) const noexcept { return substr().startsWith( value.substr() ); }

		// Check if the string starts with a specific 'value'.
		inline bool startsWith( char_t value ) const noexcept { return *begin() == value; }

		// Check if string ends with a specific 'value'.
		inline bool endsWith( const string& value ) const noexcept { return substr().endsWith( value.substr() ); }

		// Check if string ends with a specific 'value'.
		inline bool endsWith( char_t value ) const noexcept { return *last() == value; }


		// Basic comparison.
		// There are two basic ways to compare strings, which may yield different results
		// (though, not significant for the comparison as such), depending on std implementation.
		// If [eon::CompareType::faster] is specified, the fastest available low-level compare function
		// will be used. Return values are not guaranteed to be anything other then '-1', '0', and '1'.
		// If [eon::CompareType::diff_pos] is specified, return '-pos', '0' or 'pos' (guaranteed!),
		// where pos is the position of the first character that is different between the two.
		// NOTE: You can also use [eon::substring::compare] for comparing sections of strings!
		inline int compare( const string& other, CompareType type = CompareType::faster ) const noexcept {
			return substr().compare( other.substr(), type ); }

		// Check if 'this' sorts before 'other' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator<( const string& other ) const noexcept { return compare( other ) < 0; }

		// Check if 'this' sorts before or same as 'other' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator<=( const string& other ) const noexcept { return compare( other ) <= 0; }

		// Check if 'this' sorts after 'other' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator>( const string& other ) const noexcept { return compare( other ) > 0; }

		// Check if 'this' sorts after or same as 'other' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator>=( const string& other ) const noexcept { return compare( other ) >= 0; }

		// Check if 'this' sorts same as 'other' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator==( const string& other ) const noexcept { return compare( other ) == 0; }

		// Check if 'this' sorts before or after 'other' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator!=( const string& other ) const noexcept { return compare( other ) != 0; }


		// Compare while ignoring case.
		// Returns '-pos', '0' or 'pos' (guaranteed!), where pos is the position of the first character that is different.
		inline int iCompare( const string& other ) const noexcept { return substr().iCompare( other.substr() ); }

		// Compare while ignoring case.
		// Returns '-pos', '0' or 'pos' (guaranteed!), where pos is the position of the first character that is different.
		inline int iCompare( const char* cstr ) const noexcept { return substr().iCompare( substring( cstr ) ); }

		// Compare while ignoring case.
		// Returns '-pos', '0' or 'pos' (guaranteed!), where pos is the position of the first character that is different.
		inline int iCompare( const std::string& stdstr ) const noexcept { return substr().iCompare( substring( stdstr ) ); }


		// Compare against 'C-string' using [eon::CompareType::faster], returning '-1', '0', or '1' (only guarantee).
		inline int compare( const char* cstr, CompareType type = CompareType::faster ) const noexcept {
			return substr().compare( substring( cstr ), type ); }

		// Check if 'this' sorts before 'cstr' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator<( const char* cstr ) const noexcept { return compare( cstr ) < 0; }

		// Check if 'this' sorts before or same as 'cstr' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator<=( const char* cstr ) const noexcept { return compare( cstr ) <= 0; }

		// Check if 'this' sorts after 'cstr' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator>( const char* cstr ) const noexcept { return compare( cstr ) > 0; }

		// Check if 'this' sorts after or same as 'cstr' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator>=( const char* cstr ) const noexcept { return compare( cstr ) >= 0; }

		// Check if 'this' sorts same as 'cstr' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator==( const char* cstr ) const noexcept { return compare( cstr ) == 0; }

		// Check if 'this' sorts before or after 'cstr' using [eon::string::compare] and [eon::CompareType::faster].
		inline bool operator!=( const char* cstr ) const noexcept { return compare( cstr ) != 0; }


		// Compare against 'std::string' using [eon::CompareType::faster], returning '-1', '0', or '1' (only guarantee).
		inline int compare( const std::string& stdstr, CompareType type = CompareType::faster ) const noexcept {
			return substr().compare( substring( stdstr ) ); }

		// Check if 'this' sorts before 'stdstr' using [eon::string::compare] and [eon::CompareType::faster].
		bool operator<( const std::string& stdstr ) const noexcept { return compare( stdstr ) < 0; }

		// Check if 'this' sorts before or same as 'stdstr' using [eon::string::compare] and [eon::CompareType::faster].
		bool operator<=( const std::string& stdstr ) const noexcept { return compare( stdstr ) <= 0; }

		// Check if 'this' sorts after 'stdstr' using [eon::string::compare] and [eon::CompareType::faster].
		bool operator>( const std::string& stdstr ) const noexcept { return compare( stdstr ) > 0; }

		// Check if 'this' sorts after or same as 'stdstr' using [eon::string::compare] and [eon::CompareType::faster].
		bool operator>=( const std::string& stdstr ) const noexcept { return compare( stdstr ) >= 0; }

		// Check if 'this' sorts same as 'stdstr' using [eon::string::compare] and [eon::CompareType::faster].
		bool operator==( const std::string& stdstr ) const noexcept { return compare( stdstr ) == 0; }

		// Check if 'this' sorts before or after 'stdstr' using [eon::string::compare] and [eon::CompareType::faster].
		bool operator!=( const std::string& stdstr ) const noexcept { return compare( stdstr ) != 0; }


		// Comparing against a single [eon::char_t] Unicode codepoint. Returns only '-1', '0', or '1'!
		inline int compare( char_t cp ) const noexcept {
			return NumChars == 0 || *begin() < cp ? -1 : *begin() > cp ? 1 : NumChars == 1 ? 0 : 1; }

		// Check if 'this' compares before 'cp'.
		inline bool operator<( char_t cp ) const noexcept { return compare( cp ) < 0; }

		// Check if 'this' compares before or same as 'cp'.
		inline bool operator<=( char_t cp ) const noexcept { return compare( cp ) <= 0; }

		// Check if 'this' compares after 'cp'.
		inline bool operator>( char_t cp ) const noexcept { return compare( cp ) > 0; }

		// Check if 'this' compares after or same as 'cp'.
		inline bool operator>=( char_t cp ) const noexcept { return compare( cp ) >= 0; }

		// Check if 'this' compares same as 'cp'.
		inline bool operator==( char_t cp ) const noexcept { return compare( cp ) == 0; }

		// Check if 'this' compares before or after 'cp'.
		inline bool operator!=( char_t cp ) const noexcept { return compare( cp ) != 0; }


		// Compare against a single char. Returns only '-1', '0', or '1'!
		inline int compare( char c ) const noexcept { return compare( static_cast<char_t>( c ) ); }

		// Check if 'this' compares before 'c'.
		inline bool operator<( char c ) const noexcept { return compare( static_cast<char_t>( c ) ) < 0; }

		// Check if 'this' compares before or same as 'c'.
		inline bool operator<=( char c ) const noexcept { return compare( static_cast<char_t>( c ) ) <= 0; }

		// Check if 'this' compares after 'c'.
		inline bool operator>( char c ) const noexcept { return compare( static_cast<char_t>( c ) ) > 0; }

		// Check if 'this' compares after or same as 'c'.
		inline bool operator>=( char c ) const noexcept { return compare( static_cast<char_t>( c ) ) >= 0; }

		// Check if 'this' compares same as 'c'.
		inline bool operator==( char c ) const noexcept { return compare( static_cast<char_t>( c ) ) != 0; }

		// Check if 'this' compares before or after 'c'.
		inline bool operator!=( char c ) const noexcept { return compare( static_cast<char_t>( c ) ) != 0; }





		///////////////////////////////////////////////////////////////////////
		//
		// Wide-string Support
		//
		// Note that this support is limited and somewhat costly.
		//
	public:

		// Construct string from 'std::wstring'.
		inline string( const std::wstring& stdwstr ) { *this = stdwstr; }

		// Construct string from C-style wide-string.
		inline string( const wchar_t* cstr ) { *this = cstr; }


		// Discard current details and assign from 'std::wstring'.
		string& operator=( const std::wstring& stdwstr );

		// Discard current details and assign from C-style wide-string.
		string& operator=( const wchar_t* cstr );

		
		// Allow use of 'this' in place of a 'std::wstring' object.
		inline operator std::wstring() const { return wstr(); }

		// Get a 'std::wstring' conversion copy of 'this'.
		std::wstring wstr() const;

		// Concatenate a 'std::wstring' to 'this'.
		inline string& operator+=( const std::wstring& to_add ) { return *this += string( to_add ); }




		///////////////////////////////////////////////////////////////////////
		//
		// String as List
		// The string is considered a list of elements where each element is
		// separated by a common character or sub-string (';' by default). The
		// separator is only between elements, not before the first or after
		// the last.
		//
		// WARNING: No separator checking is done when adding!
		// Adding "alpha" first and then "beta" is the same as adding
		// "alpha;beta" in one go (using default separator).
		//
		// To split the elements into a container, use [eon::string::splitSequential]
		// or [eon::string::splitNonSequential].
		//
	public:

		// Add an element to the end of the string-list.
		string& push_back( const string& element, const string& separator = string( SemiColonChr ) );

		// Add an element to the front of the string-list.
		// NOTE: This is somewhat costly.
		string& push_front( const string& element, const string& separator = string( SemiColonChr ) );

		// Remove duplicates and sort the list.
		// WARNING: This is potentially very costly, depending on the number of elements!
		string& orderList( const string& separator = string( SemiColonChr ) );




		///////////////////////////////////////////////////////////////////////
		//
		// Static utility methods
		//
	public:

		// Set locale (used by various compare methods).
		// The default is set automatically at startup!
		static void setLocale( const std::string& name = "en_US.utf8" );

		// Convert other types into string (using 'std::to_string')
		// This method is limited to whatever is supported by 'std::to_string'!
		template<typename T>
		static inline string toString( T value ) { return string( std::to_string( value ), true ); }

		// Convert 'double' into string (specifically).
		// This provides greater precision than the template version,
		// and removal of trailing decimal zeros.
		static string toString( double value );

		// Convert 'double' into string (specifically).
		// This provides greater precision than the template version,
		// and removal of trailing decimal zeros.
		static string toString( long double value );


		// Get Byte Order Marker (BOM) for UTF-8.
		static const std::string& bom();


		static inline bool isLetter( char_t cp ) noexcept { return Characters::get().isLetter( cp ); }
		static inline bool isLetterUpperCase( char_t cp ) noexcept { return Characters::get().isLetterUpperCase( cp ); }
		static inline bool isLetterLowerCase( char_t cp ) noexcept { return Characters::get().isLetterLowerCase( cp ); }
		static inline bool isLetterTitleCase( char_t cp ) noexcept { return Characters::get().isLetterTitleCase( cp ); }
		static inline bool isLetterModifier( char_t cp ) noexcept { return Characters::get().isLetterModifier( cp ); }
		static inline bool isLetterOther( char_t cp ) noexcept { return Characters::get().isLetterOther( cp ); }
		//
		static inline bool isMarkSpacingCombining( char_t cp ) noexcept {
			return Characters::get().isMarkSpacingCombining( cp ); }
		static inline bool isMarkNonSpacing( char_t cp ) noexcept { return Characters::get().isMarkNonSpacing( cp ); }
		static inline bool isMarkEnclosing( char_t cp ) noexcept { return Characters::get().isMarkEnclosing( cp ); }
		//
		static inline bool isNumber( char_t cp ) noexcept {
			return isNumberDecimalDigit( cp ) || isNumberLetter( cp ) || isNumberOther( cp ); }
		static inline bool isNumberAsciiDigit( char_t cp ) noexcept { return Characters::get().isNumberAsciiDigit( cp ); }
		static inline bool isNumberDecimalDigit( char_t cp ) noexcept {
			return Characters::get().isNumberDecimalDigit( cp ); }
		static inline bool isNumberLetter( char_t cp ) noexcept { return Characters::get().isNumberLetter( cp ); }
		static inline bool isNumberOther( char_t cp ) noexcept { return Characters::get().isNumberOther( cp ); }
		//
		static inline bool isPunctuation( char_t cp ) noexcept { return Characters::get().isPunctuation( cp ); }
		static inline bool isPunctuationConnector( char_t cp ) noexcept {
			return Characters::get().isPunctuationConnector( cp ); }
		static inline bool isPunctuationDash( char_t cp ) noexcept { return Characters::get().isPunctuationDash( cp ); }
		static inline bool isPunctuationOpen( char_t cp ) noexcept { return Characters::get().isPunctuationOpen( cp ); }
		static inline bool isPunctuationClose( char_t cp ) noexcept { return Characters::get().isPunctuationClose( cp ); }
		static inline bool isPunctuationInitialQuote( char_t cp ) noexcept {
			return Characters::get().isPunctuationInitialQuote( cp ); }
		static inline bool isPunctuationFinalQuote( char_t cp ) noexcept {
			return Characters::get().isPunctuationFinalQuote( cp ); }
		static inline bool isPunctuationOther( char_t cp ) noexcept { return Characters::get().isPunctuationOther( cp ); }
		//
		static inline bool isSymbol( char_t cp ) noexcept { return Characters::get().isSymbol( cp ); }
		static inline bool isSymbolCurrency( char_t cp ) noexcept { return Characters::get().isSymbolCurrency( cp ); }
		static inline bool isSymbolModifier( char_t cp ) noexcept { return Characters::get().isSymbolModifier( cp ); }
		static inline bool isSymbolMath( char_t cp ) noexcept { return Characters::get().isSymbolMath( cp ); }
		static inline bool isSymbolOther( char_t cp ) noexcept { return Characters::get().isSymbolOther( cp ); }
		//
		static inline bool isSeparator( char_t cp ) noexcept { return Characters::get().isSeparator( cp ); }
		static inline bool isSeparatorLine( char_t cp ) noexcept { return Characters::get().isSeparatorLine( cp ); }
		static inline bool isSeparatorParagraph( char_t cp ) noexcept {
			return Characters::get().isSeparatorParagraph( cp ); }
		static inline bool isSeparatorSpace( char_t cp ) noexcept { return Characters::get().isSeparatorSpace( cp ); }


		// Check if a [eon::char_t] Unicode codepoint is a word character.
		// NOTE: Word characters are: 'letters', 'number decimal digits' (of any kind) and 'underscore'!
		static inline bool isWordChar( char_t cp ) noexcept {
			return isLetter( cp ) || isNumberDecimalDigit( cp ) || cp == '_'; }

		// Check if a [eon::char_t] Unicode codepoint is a space character.
		// NOTE: Space characters are: 'character tabulation', 'space' and 'separators'
		static inline bool isSpaceChar( char_t cp ) noexcept { return ( cp >= 0x09 && cp <= 0x0D ) || isSeparator( cp ); }





		///////////////////////////////////////////////////////////////////////
		//
		// Helpers
		//
	private:

		static index_t _findDecimalSeparator( std::vector<char_t>& digits, char_t decimal_separator = PointChr ) noexcept;

		// Given an iterator, round up all the digits in the string all the way
		// up to adding a new '1' in the front if all digits are rounded up.
		// Rounding will stop when rounding up a digit results in a new digit
		// less than 5.
		// NOTE: This method assumes (without checking) that all characters
		// from and including 'i' and to and including 'begin()' are digites or
		// a point!
		static void _roundUp( std::vector<char_t>& digits, index_t i ) noexcept;


		// Byte operations:

		// Check if pure ASCII or if we have multi-type characters
		inline bool _ascii() const noexcept { return numBytes() == NumChars; }




		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//

	private:
		std::string Bytes;
		index_t NumChars{ 0 };	// Number of code points

	public:
		static const string Empty;
	};




	// Get a string that is the result of concatenating one [eon::substring] with another.
	inline string operator+( const substring& a, const substring& b ) { return string( a ) += b; }

	// Get a string that is the result of concatenating a [eon::substring] with a 'std::string'.
	inline string operator+( const substring& a, const std::string& b ) { return string( a ) += b; }

	// Get a string that is the result of concatenating a 'std::string' with a [eon::substring].
	inline string operator+( const std::string& a, const substring& b ) { return string( a ) += b; }

	// Get a string that is the result of concatenating a [eon::substring] with a C string.
	inline string operator+( const substring& a, const char* b ) { return string( a ) += b; }

	// Get a string that is the result of concatenating a C string with a [eon::substring].
	inline string operator+( const char* a, const substring& b ) { return string( a ) += b; }


	// Smart-pointer to string.
	using string_ptr = std::shared_ptr<string>;

	// Binary predicate for comparing two [eon::string_ptr] smart-pointers.
	struct string_lt {
		inline bool operator()( const string* lhs, const string* rhs ) const { return lhs->compare( *rhs ) < 0; } };
};


namespace std
{
	// Allow implicit use of [eon::string] as key when used in containers such
	// as 'std::unordered_map' and 'std::unordered_set'.
	template<>
	struct hash<::eon::string> {
		inline size_t operator()( const ::eon::string& rhs ) const {
			return static_cast<size_t>( rhs.hash() ); } };

	// Allow implicit use of [eon::string] as value when used in containers such
	// as 'std::unordered_map' and 'std::unordered_set'.
	template<>
	struct equal_to<::eon::string> {
		inline bool operator()( const ::eon::string& lhs, const ::eon::string& rhs ) const {
			return lhs == rhs; } };


	// Allow implicit use of [eon::string_ptr] as key when used in containers
	// such as 'std::unordered_map' and 'std::unordered_set'.
	template<>
	struct hash<::eon::string_ptr> {
		inline size_t operator()( const ::eon::string_ptr& rhs ) const {
			return static_cast<size_t>( rhs->hash() ); } };

	// Allow implicit use of [eon::string_ptr] as value when used in containers
	// such as 'std::unordered_map' and 'std::unordered_set'.
	template<>
	struct equal_to<::eon::string_ptr> {
		inline bool operator()( const ::eon::string_ptr& lhs, const ::eon::string_ptr& rhs ) const {
			return *lhs == *rhs; } };


	// Allow implicit use of [eon::string_ptr] as key when used in containers
	// such as 'std::map' and 'std::set'.
	struct string_ptr_lt {
		inline bool operator()( const ::eon::string_ptr& lhs, const ::eon::string_ptr& rhs ) const {
			return *lhs < *rhs; } };


	// Allow [eon::string] to be serialized to 'std::ostream' objects.
	inline ostream& operator<<( ostream& o, const ::eon::string& str ) { o << str.stdstr(); return o; }

	// Allow [eon::string] to be de-serialized from 'std::istream' objects.
	inline istream& operator>>( istream& i, ::eon::string& str ) { std::string tmp; i >> tmp; str = tmp; return i; }
};
