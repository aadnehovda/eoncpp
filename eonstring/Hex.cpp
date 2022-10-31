#include "Hex.h"


namespace eon
{
	hex& hex::operator=( const std::string& binary_data )
	{
		clear();
		Value.reserve( binary_data.size() * 2 );
		for( auto c : binary_data )
		{
			auto digits = toHex( c );
			digits.appendToHex( Value );
		}
		return *this;
	}
	hex& hex::operator=( const char* binary_data )
	{
		clear();
		for( auto c = binary_data; *c != '\0'; ++c )
		{
			auto digits = toHex( *c );
			digits.appendToHex( Value );
		}
		return *this;
	}

	void hex::setHex( const std::string& hex )
	{
		_disallowOddNumberOfDigits( hex );
		if( hex.size() == 0 )
			clear();
		else
			_setConfirmedHexDigits( hex );
	}




	std::string hex::binary() const noexcept
	{
		std::string bin;
		for( auto c = Value.c_str(), end = c + Value.size(); c != end; c += 2 )
		{
			digits hx( *c, *( c + 1 ) );
			bin += static_cast<char>( toByte( hx ) );
		}
		return bin;
	}




	void hex::_setConfirmedHexDigits( const std::string& hex )
	{
		std::string hex_output;
		for( auto c = hex.c_str(), end = c + hex.size(); c != end; c += 2 )
			_appendToHex( c, hex_output );
		Value = std::move( hex_output );
	}
	void hex::_appendToHex( const char* c, std::string& hex_output ) const
	{
		digits hx( *c, *( c + 1 ) );
		if( !isDigit( hx.first() ) || !isDigit( hx.second() ) )
			throw Invalid( "Contains non-hex digits" );
		hx.appendToHex( hex_output );
	}
}
