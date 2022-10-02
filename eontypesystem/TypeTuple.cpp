#include "TypeTuple.h"
#include "TypeMap.h"
#include "Tuple.h"


namespace eon
{
	void TypeTuple::str( Stringifier& str ) const
	{
		str.prepend( "T" );
		_toStr( str, true );
	}

	index_t TypeTuple::sizeOfObject() const noexcept
	{
		if( isName() )
			return type::sizeOf( NameValue );
		else
			return sizeof( Tuple );
	}




	int TypeTuple::compare( const TypeTuple& other ) const noexcept
	{
		if( NameValue == no_name )
			return other.NameValue == no_name ? 0 : -1;
		else if( other.NameValue == no_name )
			return 1;

		if( NameValue < other.NameValue )
			return -1;
		else if( other.NameValue < NameValue )
			return 1;
		if( NameValue == name_ttypetuple )
		{
			for( index_t i = 0; i < TupleValue.size() && i < other.TupleValue.size(); ++i )
			{
				auto cmp = TupleValue[ i ].compare( other.TupleValue[ i ] );
				if( cmp != 0 )
					return cmp;
			}
			if( TupleValue.size() < other.TupleValue.size() )
				return -1;
			else if( other.TupleValue.size() < TupleValue.size() )
				return 1;
		}
		return 0;
	}

	bool TypeTuple::compatibleWith( const TypeTuple& other ) const noexcept
	{
		// Empty is NOT compatible!
		if( empty() || other.empty() )
			return false;

		// If name values then IS compatible if same name value!
		if( isName() && other.isName() )
			return NameValue == other.NameValue;

		// All named attributes must match on type
		std::unordered_set<index_t> named_attribs;
		for( auto& named : NamedAttributes )
		{
			auto found = other.NamedAttributes.find( named.first );
			if( found == other.NamedAttributes.end() )
				return false;
			if( !TupleValue[ named.second ].compatibleWith( other.TupleValue[ found->second ] ) )
				return false;
			named_attribs.insert( named.second );
		}
		
		// All unnamed attributes must match on position and type
		for( size_t i = 0; i < TupleValue.size(); ++i )
		{
			if( named_attribs.find( i ) != named_attribs.end() )
				continue;
			if( i >= other.TupleValue.size() )
				return false;
			if( !TupleValue[ i ].compatibleWith( other.TupleValue[ i ] ) )
				return false;
		}

		return true;
	}




	const name_t name_ttypetuple{ compilerName( "#typetuple" ) };

	void TypeTuple::_toStr( Stringifier& str, bool top_level ) const
	{
		if( top_level || isSubTuple() )
			str.start_grp2( "(" );
		if( isName() )
			str.word( eon::str( NameValue ) );
		else
		{
			std::unordered_map<index_t, name_t> names;
			for( auto& elm : NamedAttributes )
				names[ elm.second ] = elm.first;
			for( index_t i = 0; i < TupleValue.size(); ++i )
			{
				if( i > 0 )
					str.punct( "," );
				auto name = names.find( i );
				if( name != names.end() )
				{
					str.word( eon::str( name->second ) );
					str.op2( "=" );
				}
				TupleValue[ i ]._toStr( str, false );
			}
		}
		if( top_level || isSubTuple() )
			str.end_grp2( ")" );
	}


	std::unordered_set<name_t> TypeTuple::PrimTypes;
	void TypeTuple::_populatePrims()
	{
		if( PrimTypes.empty() )
		{
			PrimTypes = {
				name_bool,
				name_byte, name_char,
				name_int, name_short, name_long,
				name_float, name_low,
				name_index,
				name_handle };
#ifndef _DEBUG
			PrimTypes.insert( name_name );
#endif
		}
	}

	std::unordered_set<name_t> TypeTuple::BasicTypes;
	void TypeTuple::_populateBasics()
	{
		if( BasicTypes.empty() )
		{
			BasicTypes = {
				name_high,
				name_bytes,
				name_string, name_stringchar, name_substring,
				name_chars,
				name_real, name_complex,
				name_regex,
				name_namepath,
				name_path,
				name_time, name_date, name_timezone, name_dst, name_timepoint, name_timespan, name_timer,
				name_expression,
				name_array, name_vector, name_list, name_set, name_heap };
#ifdef _DEBUG
			BasicTypes.insert( name_name );
#endif
		}
	}
}
