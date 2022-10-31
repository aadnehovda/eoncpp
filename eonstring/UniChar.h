#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include <locale>


// Detect platform
#if defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __NT__ )
#	define EON_WINDOWS
#elif defined( __unix ) || defined( __unix__ )
#	define EON_UNIX
#endif
#if defined( __APPLE__ ) || defined( __MACH__ )
#	define EON_APPLE
#endif
#if defined( sun ) || defined( __sun )
#	define EON_SUN
#endif
#if defined( __linux__ ) || defined( __linux ) || defined( linux ) || defined( __gnu_linux__ )
#	define EON_LINUX
#endif




///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	// An eon::char_t is a 32-bit integer utilizing the C++ [char32_t] type.
	using char_t = char32_t;

	// An eon::byte_t is an 8-bit integer.
	// (Not using char/unsigned char directly to avoid confustion with UTF-8/Unicode characters.)
	using byte_t = unsigned char;


	// For consistency, eon::int_t is defined as plain int. This is (normally) a 32-bit integer type.
	using int_t = int;

	// An eon::short_t is an unsigned 16-bit integer - a short integer.
	using short_t = int16_t;

#ifdef EON_SUN
	// An eon::long_t is a 64-bit signed integer - a long integer.
	using long_t = long long int;

	// An eon::index_t is a 64-bit unsigned integer.
	using index_t = unsigned long long int;

	// Max [eon::index_t] value.
#define INDEX_MAX UINT64_MAX

#else
	// An eon::long_t is a 64-bit signed integer - a long integer.
	using long_t = int64_t;

	// An eon::index_t is equivalent to [std::size_t], normally a 64-bit unsigned integer.
	using index_t = size_t;

	// Max [eon::index_t] value.
#define INDEX_MAX SIZE_MAX

#endif

	// Defining an [eon::index_t] void value (not zero/null in C++).
	static const index_t no_index{ SIZE_MAX };

	// An eon::flt_t is a 64-bit floating point number.
	// NOTE: float_t is defined/reserved for C++ on some platforms, resulting in a shorter and uglier name for Eon!
	using flt_t = double;

	// An eon::low_t (low precision) is a 32-bit floating pointer number.
	using low_t = float;

	// An eon::high_t (high precision) is a 64-or-more-bit floating point number.
	using high_t = long double;

	
	// While not strictly necessary, these character definitions can help
	// increase clarity and avoid some confusions:
	static const char_t NullChr{ '\0' };
	static const char_t SpaceChr{ ' ' }, TabChr{ '\t' };
	static const char_t PointChr{ '.' }, CommaChr{ ',' };
	static const char_t ColonChr{ ':' }, SemiColonChr{ ';' };
	static const char_t DblQuoteChr{ '"' }, SglQuoteChr{ '\'' };
	static const char_t ZeroChr{ '0' };
	static const char_t NewlineChr{ '\n' }, CReturnChr{ '\r' };
	static const char_t StarChr{ '*' };
	static const char_t EqualChr{ '=' }, PlusChr{ '+' }, MinusChr{ '-' };
	static const char_t RoundBrcOpenChr{ '(' }, RoundBrcCloseChr{ ')' };
	static const char_t CurlyBrcOpenChr{ '{' }, CurlyBrcCloseChr{ '}' };
	static const char_t SquareBrcOpenChr{ '[' }, SquareBrcCloseChr{ ']' };
	static const char_t AngleBrcOpenChr{ '{' }, AngleBrcCloseChr{ '}' };
	static const char_t ForwSlashChr{ '/' }, BackSlashChr{ '\\' };
	static const char_t BarChr{ '|' };

	// Special character that indicates a [eon::char_t] nothing value.
	static const char_t nochar{ UINT32_MAX };

	// Special character used as default value in parameter declarations to
	// indicate that the [eon::char_t] argument in question will have the same
	// value as the previous [eon::char_t] argument.
	// It is the same as [eon::nochar] but with a name that is more appropriate
	// to the context.
	static const char_t same_char = nochar;


	// Check if the given [eon::char_t] is a valid Unicode codepoint.
	static inline bool isValidCodepoint( char_t value ) noexcept {
		return value < 0xC0 || ( value > 0xC1 && value < 0x140000 ); }




	///////////////////////////////////////////////////////////////////////////
	//
	// Character Categories
	//
	// Categories based on
	//   https://www.fileformat.info/info/unicode/category/index.htm
	//
	enum class charcat
	{
		letter_lowercase,
		Ll = letter_lowercase,
		letter_uppercase,
		Lu = letter_uppercase,
		letter_titlecase,
		Lt = letter_titlecase,
		letter_modifier,
		Lm = letter_modifier,
		letter_other,
		Lo = letter_other,

		mark_spacing_combining,
		Mc = mark_spacing_combining,
		mark_enclosing,
		Me = mark_enclosing,
		mark_nonspacing,
		Mn = mark_nonspacing,

		// All ASCII digits [0-9] - Custom Eon character class!
		number_ascii_digit,

		number_decimal_digit,
		Nd = number_decimal_digit,
		number_letter,
		Nl = number_letter,
		number_other,
		No = number_other,

		punctuation_connector,
		Pc = punctuation_connector,
		punctuation_dash,
		Pd = punctuation_dash,
		punctuation_close,
		Pe = punctuation_close,
		punctuation_final_quote,
		Pf = punctuation_final_quote,
		punctuation_initial_quote,
		Pi = punctuation_initial_quote,
		punctuation_other,
		Po = punctuation_other,
		punctuation_open,
		Ps = punctuation_open,

		symbol_currency,
		Sc = symbol_currency,
		symbol_modifier,
		Sk = symbol_modifier,
		symbol_math,
		Sm = symbol_math,
		symbol_other,
		So = symbol_other,

		separator_line,
		Zl = separator_line,
		separator_paragraph,
		Zp = separator_paragraph,
		separator_space,
		Zs = separator_space,

		other_control,
		Cc = other_control,
		other_format,
		Cf = other_format,
		other_private_use,
		Co = other_private_use,
		other_surrogate,
		Cs = other_surrogate,

		// Value used when character category is unknown.
		undef
	};




	///////////////////////////////////////////////////////////////////////////
	//
	// The 'Characaters' class is an implicit global class that can be used to
	// check if a specific [eon::char_t] value belongs to a specific
	// [eon::charcat] character category, including groups of related
	// categories.
	//
	class Characters
	{
	private:

		// Direct construction is not permitted but is done in the background.
		Characters();

	public:

		// Copy-construction is not permitted!
		Characters( const Characters& ) = delete;

		// Move construction is not permitted!
		Characters( Characters&& ) = delete;

		// Destruction is done in the background.
		virtual ~Characters();




		///////////////////////////////////////////////////////////////////////
		//
		// Access
		//
	public:

		// Get access to the single, global [eon::Characters] object.
		static const Characters& get();

		// Get the current locale being used.
		inline const std::locale& locale() const noexcept { return Locale; }


		// Check if a character belongs to any of the "other" categories.
		inline bool isOther( char_t c ) const noexcept {
			return isOtherControl( c ) || isOtherFormat( c ) || isOtherPrivateUse( c ) || isOtherSurrogate( c ); }

		// Check if a character belongs to the "other" "control" category.
		inline bool isOtherControl( char_t c ) const noexcept {
			return ( c >= 0x00 && c <= 0x1F ) || ( c >= 0x7F && c <= 0x9F ); }

		// Check if a character belongs to the "other" "format" category.
		inline bool isOtherFormat( char_t c ) const noexcept {
			return std::binary_search( other_format->begin(), other_format->end(), c ); }

		// Check if a character belongs to the "other" "private use" category.
		inline bool isOtherPrivateUse( char_t c ) const noexcept {
			return c == 0xE000 || c == 0xF8FF || c == 0xF0000 || c == 0xFFFFD || c == 0x100000 || c == 0x10FFFD; }

		// Check if a character belongs to the "other" "surrogate" category.
		inline bool isOtherSurrogate( char_t c ) const noexcept {
			return c == 0xD800 || c == 0xDB7F || c == 0xDB80 || c == 0xDBFF || c == 0xDC00 || c == 0xDFFF; }


		// Check if a character belongs to any of the "letter" categories.
		inline bool isLetter( char_t c ) const noexcept { return isLetterUpperCase( c ) || isLetterLowerCase( c )
			|| isLetterTitleCase( c ) || isLetterModifier( c ) || isLetterOther( c ); }

		// Check if a character belongs to the "letter" "upper-case" category.
		bool isLetterUpperCase( char_t c ) const noexcept;

		// Check if a character belongs to the "letter" "lower-case" catetory.
		bool isLetterLowerCase( char_t c ) const noexcept;

		// Check if a character belongs to the "letter" "title-case" category.
		inline bool isLetterTitleCase( char_t c ) const noexcept {
			return std::binary_search( letter_title_case.begin(), letter_title_case.end(), c ); }

		// Check if a character belongs to the "letter" "modifier" category.
		bool isLetterModifier( char_t c ) const noexcept;

		// Check if a character belongs to the "letter" "other" category.
		inline bool isLetterOther( char_t c ) const noexcept {
			return c >= 0xAA && std::binary_search( letter_other->begin(), letter_other->end(), c ); }


		// Check if a character is an ASCII letter.
		inline bool isAsciiLetter( char_t c ) const noexcept {
			return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ); }


		// Check if a character belongs to any of the "mark" categories.
		inline bool isMarkSpacingCombining( char_t c ) const noexcept {
			return std::binary_search( mark_spacing_combining->begin(), mark_spacing_combining->end(), c ); }

		// Check if a character belongs to the "mark" "non-spacing" category.
		inline bool isMarkNonSpacing( char_t c ) const noexcept {
			return std::binary_search( mark_nonspacing->begin(), mark_nonspacing->end(), c ); }

		// Check if a character belongs to the "mark" "enclosing" category.
		inline bool isMarkEnclosing( char_t c ) const noexcept {
			return std::binary_search( mark_enclosing.begin(), mark_enclosing.end(), c ); }


		// Check if a character is an ASCII digit.
		inline bool isNumberAsciiDigit( char_t c ) const noexcept { return c >= '0' && c <= '9'; }


		// Check if a character belongs to any of the "number" categories.
		inline bool isNumber( char_t c ) const noexcept {
			return isNumberDecimalDigit( c ) || isNumberLetter( c ) || isNumberOther( c ); }

		// Check if a character belongs to the "number" "decimal digit" category.
		inline bool isNumberDecimalDigit( char_t c ) const noexcept { return c <= 0x39 ? c >= 0x30 && c <= 0x39
			: std::binary_search( number_decimal_digit->begin(), number_decimal_digit->end(), c ); }

		// Check if a character belongs to the "number" "letter" category.
		inline bool isNumberLetter( char_t c ) const noexcept { return
			( c >= 0x2160 && c <= 0x2182 ) || std::binary_search( number_letter->begin(), number_letter->end(), c ); }

		// Check if a character belongs to the "number" "other" category.
		inline bool isNumberOther( char_t c ) const noexcept {
			return std::binary_search( number_other->begin(), number_other->end(), c ); }


		// Check if a character belongs to any of the "punctuation" categories.
		inline bool isPunctuation( char_t c ) const noexcept {
			return isPunctuationConnector( c ) || isPunctuationDash( c ) || isPunctuationOpen( c ) || isPunctuationClose( c )
			|| isPunctuationInitialQuote( c ) || isPunctuationFinalQuote( c ) || isPunctuationOther( c ); }

		// Check if a character belongs to the "punctuation" "connector" category.
		inline bool isPunctuationConnector( char_t c ) const noexcept {
			return std::binary_search( punctuation_connector.begin(), punctuation_connector.end(), c ); }

		// Check if a character belongs to the "punctuation" "dash" category.
		inline bool isPunctuationDash( char_t c ) const noexcept {
			return std::binary_search( punctuation_dash.begin(), punctuation_dash.end(), c ); }

		// Check if a character belongs to the "punctuation" "open" category.
		inline bool isPunctuationOpen( char_t c ) const noexcept {
			return std::binary_search( punctuation_open->begin(), punctuation_open->end(), c ); }
		
		// Check if a character belongs to the "punctuation" "close" category.
		inline bool isPunctuationClose( char_t c ) const noexcept {
			return std::binary_search( punctuation_close->begin(), punctuation_close->end(), c ); }

		// Check if a character belongs to the "punctuation" "initial quote" category.
		inline bool isPunctuationInitialQuote( char_t c ) const noexcept {
			return std::binary_search( punctuation_initial_quote.begin(), punctuation_initial_quote.end(), c ); }

		// Check if a character belongs to the "punctuation" "final quote" category.
		inline bool isPunctuationFinalQuote( char_t c ) const noexcept {
			return std::binary_search( punctuation_final_quote.begin(), punctuation_final_quote.end(), c ); }

		// Check if a character belongs to the "punctuation" "other" category.
		inline bool isPunctuationOther( char_t c ) const noexcept {
			return std::binary_search( punctuation_other->begin(), punctuation_other->end(), c ); }


		// Check if a character belongs to any of the "symbol" categories.
		inline bool isSymbol( char_t c ) const noexcept {
			return isSymbolCurrency( c ) || isSymbolModifier( c ) || isSymbolMath( c ) || isSymbolOther( c ); }

		// Check if a character belongs to the "symbol" "currency" category.
		inline bool isSymbolCurrency( char_t c ) const noexcept {
			return std::binary_search( symbol_currency->begin(), symbol_currency->end(), c ); }

		// Check if a character belongs to the "symbol" "modifier" category.
		inline bool isSymbolModifier( char_t c ) const noexcept {
			return std::binary_search( symbol_modifier->begin(), symbol_modifier->end(), c ); }

		// Check if a character belongs to the "symbol" "math" category.
		inline bool isSymbolMath( char_t c ) const noexcept {
			return std::binary_search( symbol_math->begin(), symbol_math->end(), c ); }

		// Check if a character belongs to the "symbol" "other" category.
		inline bool isSymbolOther( char_t c ) const noexcept {
			return std::binary_search( symbol_other->begin(), symbol_other->end(), c ); }


		// Check if a character belongs to any of the "separator" categories.
		inline bool isSeparator( char_t c ) const noexcept {
			return isSeparatorSpace( c ) || isSeparatorLine( 0 ) || isSeparatorParagraph( c ); }

		// Check if a character belongs to the "separator" "line" category.
		inline bool isSeparatorLine( char_t c ) const noexcept { return c == 0x2028; }

		// Check if a character belongs to the "separator" "paragraph" category.
		inline bool isSeparatorParagraph( char_t c ) const noexcept { return c == 0x2029; }

		// Check if a character belongs to the "separator" "space" category.
		inline bool isSeparatorSpace( char_t c ) const noexcept { return ( c >= 0x08 && c <= 0x0D ) || c == 0x20
			|| ( c >= 0x2000 && c <= 0x200A ) || c == 0xA0 || c == 0x1680 || c == 0x202F || c == 0x205F || c == 0x3000; }


		// Check if a character belongs to a named [eon::charcat] character category.
		bool is( char_t codepoint, charcat category ) const noexcept;

		// Get the [eon::charcat] character category for the specified character.
		// Returns [eon::charcat::undef] if unable to categorize.
		charcat category( char_t codepoint ) const noexcept;




		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	private:

		std::locale Locale;
		const std::vector<char_t>* other_format{ nullptr };

		const std::vector<char_t>* letter_upper_case{ nullptr };
		const std::vector<char_t> letter_title_case{ 0x01C5, 0x01C8, 0x01CB, 0x01F2, 0x1F88, 0x1F89, 0x1F8A, 0x1F8B, 0x1F8C,
			0x1F8D, 0x1F8E, 0x1F8F, 0x1F98, 0x1F99, 0x1F9A, 0x1F9B, 0x1F9C, 0x1F9D, 0x1F9E, 0x1F9F, 0x1FA8, 0x1FA9, 0x1FAA,
			0x1FAB, 0x1FAC, 0x1FAD, 0x1FAE, 0x1FAF, 0x1FBC, 0x1FCC, 0x1FFC };
		const std::vector<char_t>* letter_lower_case{ nullptr };
		const std::vector<char_t>* letter_modifier{ nullptr };
		const std::vector<char_t>* letter_other{ nullptr };

		const std::vector<char_t>* mark_spacing_combining{ nullptr };
		const std::vector<char_t>* mark_nonspacing{ nullptr };
		const std::vector<char_t> mark_enclosing{
			0x0488, 0x0489, 0x1ABE, 0x20DD, 0x20DE, 0x20DF, 0x20E0, 0x20E2, 0x20E3, 0x20E4, 0xA670, 0xA671, 0xA672 };

		const std::vector<char_t>* number_decimal_digit{ nullptr };
		const std::vector<char_t>* number_letter{ nullptr };
		const std::vector<char_t>* number_other{ nullptr };

		const std::vector<char_t> punctuation_connector{
			0x005F, 0x203F, 0x2040, 0x2054, 0xFE33, 0xFE34, 0xFE4D, 0xFE4E, 0xFE4F, 0xFF3F };
		const std::vector<char_t> punctuation_dash{
			0x002D, 0x058A, 0x05BE, 0x1400, 0x1806, 0x2010, 0x2011, 0x2012, 0x2013, 0x2014, 0x2015, 0x2E17, 0x2E1A, 0x2E3A,
			0x2E3B, 0x2E40, 0x301C, 0x3030, 0x30A0, 0xFE31, 0xFE32, 0xFE58, 0xFE63, 0xFF0D, 0x10EAD };
		const std::vector<char_t>* punctuation_open{ nullptr };
		const std::vector<char_t>* punctuation_close{ nullptr };
		const std::vector<char_t> punctuation_initial_quote{
			0x00AB, 0x2018, 0x201B, 0x201C, 0x201F, 0x2039, 0x2E02, 0x2E04, 0x2E09, 0x2E0C, 0x2E1C, 0x2E20 };
		const std::vector<char_t> punctuation_final_quote{
			0x00BB, 0x2019, 0x201D, 0x203A, 0x2E03, 0x2E05, 0x2E0A, 0x2E0D, 0x2E1D, 0x2E21 };
		const std::vector<char_t>* punctuation_other{ nullptr };

		const std::vector<char_t>* symbol_currency{ nullptr };
		const std::vector<char_t>* symbol_modifier{ nullptr };
		const std::vector<char_t>* symbol_math{ nullptr };
		const std::vector<char_t>* symbol_other{ nullptr };
	};




	// Check if the given codepoint is an ASCII digit.
	// WARNING: This will not check for any other digit types!
	static inline bool isDigit( char_t codepoint ) noexcept { return codepoint >= '0' && codepoint <= '9'; }

	// Check if the given codepoint is a numeral.
	// NOTE: This is any character in the [eon::charcat::number_decimal_digit] category!
	static inline bool isNumeral( char_t codepoint ) noexcept {
		return Characters::get().isNumberDecimalDigit( codepoint ); }

	// Check if the given codepoint is a hexadecimal digit.
	static inline bool isHexDigit( char_t codepoint ) noexcept {
		return ( codepoint >= '0' && codepoint <= '9' )
			|| ( codepoint >= 'A' && codepoint <= 'F' )
			|| ( codepoint >= 'a' && codepoint <= 'f' ); }

	// Given a hexadecimal digit, return the base-10 numerical value.
	static inline byte_t hexToNum( byte_t digit ) noexcept {
		return ( digit >= '0' && digit <= '9' ) ? ( digit - '0' )
			: ( digit >= 'A' && digit <= 'F' ) ? ( digit - 'A' + 10 )
			: ( digit >= 'a' && digit <= 'f' ) ? ( digit - 'a' + 10 ) : 0; }

	// Given a byte, convert it into two hexadecimal digits stored in a std::string of bytes.
	static inline std::string byteToHex( byte_t byte ) noexcept {
		byte_t hi = byte / 16, lo = byte % 16; std::string str( "00" );
		str[ 0 ] = hi < 10 ? '0' + hi : 'A' + hi - 10; str[ 0 ] = lo < 10 ? '0' + lo : 'A' + lo - 10; return str; }

	// Check if the given codepoint is/can be used as an octal digit.
	static inline bool isOctalDigit( char_t codepoint ) noexcept { return codepoint >= '0' && codepoint <= '7'; }
};
