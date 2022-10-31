#pragma once
#include "String.h"
#include <eonexcept/Exception.h>
#include <set>
#include <mutex>


///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// Eon Hex Class - eon::hex
	//
	// This class can convert bytes to and from hex digits, but also store
	// binary in the form of hex digits.
	//
	class hex
	{
		///////////////////////////////////////////////////////////////////////
		//
		// Definitions
		//
	public:
		
		// Exception thrown when trying to set an invalid hex value
		EONEXCEPT( Invalid );


		// Pair of hex digits
		class digits
		{
		public:
			digits() = default;
			inline digits( byte_t first, byte_t second ) { First = first; Second = second; }
			inline digits( byte_t byte ) {
				First = byte / 16; Second = byte % 16; First += First < 10 ? 0x30 : 0x37;
				Second += Second < 10 ? 0x30 : 0x37; }
			inline std::string stdstr() const { std::string s; s += First; s += Second; return s; }
			inline byte_t first() const noexcept { return First; }
			inline byte_t second() const noexcept { return Second; }
			inline void appendToHex( std::string& hex_output ) const {
				hex_output += static_cast<char>( First ); hex_output += static_cast<char>( Second ); }
			inline byte_t toByte() const noexcept {
				return ( First - ( First >= 0x41 ? 0x37 : 0x30 ) ) * 16 + ( Second - ( Second >= 0x41 ? 0x37 : 0x30 ) ); }
		private:
			byte_t First{ '0' }, Second{ '0' };
		};




		///////////////////////////////////////////////////////////////////////
		//
		// Construction
		//
	public:

		hex() = default;
		inline hex( const hex& other ) { Value = other.Value; }
		inline hex( hex&& other ) noexcept { Value = std::move( other.Value ); }

		inline hex( const std::string& binary_data ) { *this = binary_data; }
		inline hex( const char* binary_data ) { *this = binary_data; }

		virtual ~hex() = default;




		///////////////////////////////////////////////////////////////////////
		//
		// Modifier Methods
		//
	public:

		inline hex& operator=( const hex& other ) { Value = other.Value; return *this; }
		inline hex& operator=( hex&& other ) noexcept { Value = std::move( other.Value ); return *this; }

		hex& operator=( const std::string& binary_data );
		hex& operator=( const char* binary_data );

		inline hex& operator+=( const hex& other ) { Value += other.Value; return *this; }
		inline friend hex operator+( const hex& a, const hex& b ) { hex c{ a }; return c += b; }

		//* Set hex value
		void setHex( const std::string& hex );

		//* Clear value
		void clear() noexcept { Value.clear(); }




		///////////////////////////////////////////////////////////////////////
		//
		// Read-only Methods
		//
	public:

		// Check if empty
		inline bool empty() const noexcept { return Value.empty(); }

		// Use as boolean
		// Returns true if not empty!
		inline operator bool() const noexcept { return !Value.empty(); }

		// Get number of hex digits
		// (Number of bytes can be obtained by dividing by 2!)
		inline index_t size() const noexcept { return Value.size(); }


		// Get value as binary data
		// This involves converting from hex to binary!
		std::string binary() const noexcept;

		// Access the hex data
		inline const std::string& value() const noexcept { return Value; }

		// Access byte data for given position
		inline byte_t byte( index_t pos ) const { return toByte( digits( Value[ pos * 2 ], Value[ pos * 2 + 1 ] ) ); }


		// Comparison
		inline int compare( const hex& other ) const noexcept { return Value.compare( other.Value ); }
		inline bool operator<( const hex& other ) const noexcept { return Value < other.Value; }




		///////////////////////////////////////////////////////////////////////
		//
		// Static Methods
		//
	public:

		// Convert a byte into two hex digits (represented by a 16 bit
		// unsigned integer)
		static inline digits toHex( byte_t byte ) noexcept { return digits( byte ); }

		// Convert two hex digits (represented by a 16 bit unsigned integer)
		// into a byte
		static inline byte_t toByte( digits hex ) noexcept { return hex.toByte(); }

		// Check if the given byte is a hex digit
		static inline bool isDigit( byte_t byte ) noexcept {
			return ( byte >= '0' && byte <= '9' ) || ( byte >= 'A' && byte <= 'F' ) || ( byte >= 'a' && byte <= 'f' ); }




		///////////////////////////////////////////////////////////////////////
		//
		// Helpers
		//
	public:

		inline void _disallowOddNumberOfDigits( const std::string& hex ) const {
			if( hex.size() % 2 == 1 ) throw Invalid( "Odd number of digits" ); }
		
		void _setConfirmedHexDigits( const std::string& hex );

		void _appendToHex( const char* c, std::string& hex_output ) const;



		
		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	private:

		std::string Value;
	};
}
