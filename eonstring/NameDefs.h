#pragma once
#include <cstdint>
#include <cstdlib>
#include "UniChar.h"


///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// Eon Name Type Class - eon::name_t
	//
	class name_t
	{
		///////////////////////////////////////////////////////////////////////
		//
		// Constructors
		//
	public:

		// Construct a null-name (no_name)
		name_t() = default;

		// Construct a name from an unsigned 32 bit integer.
		inline name_t( uint32_t value ) noexcept { Value = value; }

		// Construct a name from an int - which will be converted into an unsigned 32 bit integer.
		explicit inline name_t( int value ) noexcept { Value = static_cast<uint32_t>( value ); }

		// Construct a name from a size_t - which will be converted into an unsigned 32 bit integer.
		explicit inline name_t( size_t value ) noexcept { Value = static_cast<uint32_t>( value ); }

		// Copy another name
		inline name_t( const name_t& other ) noexcept { Value = other.Value; }




		///////////////////////////////////////////////////////////////////////
		//
		// Basic access
		//
	public:

		// Copy another name
		inline name_t& operator=( const name_t& other ) noexcept { Value = other.Value; return *this; }


		// Get the name as an unsigned 32 bit integer.
		inline uint32_t value() const noexcept { return Value; }

		// Allow the use of name_t as index.
		inline operator size_t() const noexcept { return static_cast<size_t>( Value ); }




		///////////////////////////////////////////////////////////////////////
		//
		// Comparison
		//
	public:

		// Check if 'this' name sorts before 'other'.
		inline bool operator<( const name_t& other ) const noexcept { return Value < other.Value; }

		// Check if 'this' name sorts before or same as 'other'.
		inline bool operator<=( const name_t& other ) const noexcept { return Value <= other.Value; }

		// Check if 'this' name sorts after 'other'.
		inline bool operator>( const name_t& other ) const noexcept { return Value > other.Value; }

		// Check if 'this' name sorts after or same as 'other'.
		inline bool operator>=( const name_t& other ) const noexcept { return Value >= other.Value; }

		// Check if 'this' name sorts same as 'other'.
		inline bool operator==( const name_t& other ) const noexcept { return Value == other.Value; }

		// Check if 'this' name sorts before or after 'other'.
		inline bool operator!=( const name_t& other ) const noexcept { return Value != other.Value; }




		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	private:

		std::uint32_t Value{ 0 };
	};

	// The null name
	static const name_t no_name{ 0 };
}




namespace std
{
	template<>
	struct hash<::eon::name_t> {
		std::hash<uint32_t> Hasher;
		inline size_t operator()( const ::eon::name_t& a ) const { return Hasher( a.value() ); } };
	template<>
	struct equal_to<::eon::name_t> {
		inline bool operator()( const ::eon::name_t& a, const ::eon::name_t& b ) const { return a == b; } };
}
