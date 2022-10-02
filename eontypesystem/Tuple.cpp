#include "Tuple.h"
#include <eonregex/RegEx.h>


namespace eon
{
	Tuple::Tuple( name_t tuple_type_name, std::initializer_list<AttributePair> attributes, TuplePerm permissions )
	{
		_populateData();
		if( tuple_type_name == name_plain )
		{
			TypeTuple plain_type;
			for( auto& value : attributes )
			{
				plain_type.add( value.Name, value.Value.type() );
				_add( tuple_type_name, value.Name, std::move( *(Attribute*)&value.Value ) );
			}
			Type = TypeTuple{ { no_name, name_tuple }, { name_type, name_plain }, { name_tuple, std::move( plain_type ) } };
		}
		else
		{
			for( auto& value : attributes )
				_add( tuple_type_name, value.Name, std::move( *(Attribute*)&value.Value ) );
			Type = TypeTuple{ { no_name, name_tuple }, { name_type, tuple_type_name } };
		}
		_perm( tuple_type_name, permissions );
	}
	Tuple::Tuple( name_t tuple_type_name, std::vector<AttributePair>&& attributes, TuplePerm permissions )
	{
		_populateData();
		if( tuple_type_name == name_plain )
		{
			TypeTuple plain;
			for( auto& value : attributes )
			{
				plain.add( value.Name, value.Value.type() );
				_add( tuple_type_name, value.Name, std::move( value.Value ) );
			}
			Type = TypeTuple{ { no_name, name_tuple }, { name_type, name_plain }, { name_tuple, std::move( plain ) } };
		}
		else
		{
			for( auto& value : attributes )
				_add( tuple_type_name, value.Name, std::move( value.Value ) );
			Type = TypeTuple{ { no_name, name_tuple }, { name_type, tuple_type_name } };
		}
		_perm( tuple_type_name, permissions );
	}




	Tuple& Tuple::operator=( const Tuple& other )
	{
		Type = other.Type;
		Permissions = other.Permissions;
		Parent = nullptr;
		Attributes.clear();
		for( auto& attribute : other.Attributes )
		{
			Attributes.push_back( attribute );
			if( attribute.type() == name_tuple )
				Attributes[ Attributes.size() - 1 ].value<Tuple>().Parent = this;
		}
		NamedAttributes = other.NamedAttributes;
		return *this;
	}
	Tuple& Tuple::operator=( Tuple&& other ) noexcept
	{
		Type = std::move( other.Type );
		Permissions = other.Permissions; other.Permissions = TuplePerm::all_normal;
		Parent = nullptr;
		Attributes = std::move( other.Attributes );
		NamedAttributes = std::move( other.NamedAttributes );
		return *this;
	}




	void Tuple::str( Stringifier& strf ) const
	{
		stack<const Tuple*> ancestors;
		_str( strf, ancestors );
	}




	Tuple& Tuple::add( const TypeTuple& type )
	{
		_assertAddPerm( Type.at( name_type ).nameValue(), type );
		Attributes.push_back( Attribute::newTypeTuple( type ) );
		if( type == name_tuple )
			Attributes[ Attributes.size() - 1 ].value<Tuple>().Parent = this;
		return *this;
	}
	Tuple& Tuple::add( name_t name, const TypeTuple& type )
	{
		if( name != no_name )
		{
			if( exists( name ) )
				throw type::DuplicateName( "Cannot add tuple attribute \"" + eon::str( name ) + "\" to tuple!" );
			_assertAddPerm( Type.at( name_type ).nameValue(), type );
			NamedAttributes.insert( name, Attributes.size() );
		}
		Attributes.push_back( Attribute::newTypeTuple( type ) );
		if( type == name_tuple )
			Attributes[ Attributes.size() - 1 ].value<Tuple>().Parent = this;
		return *this;
	}

	Tuple& Tuple::add( Attribute&& value )
	{
		_assertAddPerm( Type.at( name_type ).nameValue(), value.type() );
		if( value.type() == name_tuple )
			value.value<Tuple>().Parent = this;
		Attributes.push_back( std::move( value ) );
		return *this;
	}
	Tuple& Tuple::add( name_t name, Attribute&& value )
	{
		if( name != no_name )
		{
			if( exists( name ) )
				throw type::DuplicateName( "Cannot add attribute \"" + eon::str( name ) + "\" to " + Type.at( name_name ).str()
					+ " tuple!" );
			_assertAddPerm( Type.at( name_type ).nameValue(), value.type() );
			NamedAttributes.insert( name, Attributes.size() );
		}
		if( value.type() == name_tuple )
			value.value<Tuple>().Parent = this;
		Attributes.push_back( std::move( value ) );
		return *this;
	}

	Tuple& Tuple::remove( index_t attribute_num )
	{
		_assertRemPerm();
		_assertInRange( attribute_num );
		Attributes.erase( Attributes.begin() + attribute_num );
		NamedAttributes.removeTwo( attribute_num );
		return *this;
	}
	Tuple& Tuple::remove( int_t attribute_num )
	{
		index_t pos = attribute_num >= 0 ? static_cast<index_t>( attribute_num ) : static_cast<index_t>( -attribute_num );
		if( attribute_num < 0 )
		{
			_assertInRange( pos );
			pos = Attributes.size() - pos;
		}
		return remove( pos );
	}

	bool Tuple::flag( name_t name ) const noexcept
	{
		for( index_t i = 0; i < Attributes.size(); ++i )
		{
			if( Attributes[ i ].type() == name_name
				&& !NamedAttributes.containsTwo( i )
				&& Attributes[ i ].value<name_t, type::hint::name>() == name )
				return true;
		}
		return false;
	}




	Tuple& Tuple::registerType( name_t type )
	{
		_assertTypPerm();
		if( !exists( name_dottypes ) )
			add( name_dottypes, Tuple( name_data ) );
		auto& types = at<Tuple>( name_dottypes );
		if( types.exists( type ) )
			throw type::DuplicateName( "Cannot register type: " + eon::str( type ) );
		types.add( Attribute::newName( type ) );
		return *this;
	}

	bool Tuple::isType( name_t name ) const noexcept
	{
		if( ( Permissions & TuplePerm::types ) )
		{
			if( exists( name_dottypes ) )
			{
				if( at<Tuple, type::hint::name>( name_dottypes ).flag( name ) )
					return true;
			}
		}
		return Parent ? Parent->isType( name ) : false;
	}




	Tuple& Tuple::registerAction( type::Action* action )
	{
		_assertActPerm();
		if( Actions.find( action->signature() ) != Actions.end() )
			throw type::DuplicateName( "Cannot register action: " + action->signature().str() );
		Actions[ action->signature() ] = action;
		auto found = ActionsByName.find( action->signature().at( name_name ).nameValue() );
		if( found == ActionsByName.end() )
			ActionsByName[ action->signature().at( name_name ).nameValue() ] = { action->signature() };
		else
			found->second.insert( action->signature() );
		return *this;
	}

	const std::set<TypeTuple>& Tuple::signatures( name_t action_name ) const
	{
		if( Parent )
			return Parent->signatures( action_name );
		static std::set<TypeTuple> none;
		auto found = ActionsByName.find( action_name );
		return found != ActionsByName.end() ? found->second : none;
	}
	std::set<TypeTuple> Tuple::signatures( name_t action_name, name_t type_name, const TypeTuple& args ) const
	{
		// All actions must be in the global scope!
		if( Parent )
			return Parent->signatures( action_name, type_name, args );

		std::set<TypeTuple> actions;
		auto found = ActionsByName.find( action_name );
		if( found != ActionsByName.end() )
		{
			for( auto& signature : found->second )
			{
				if( signature.at( name_type ) == type_name && signature.at( name_args ) == args )
					actions.insert( signature );
			}
		}
		return actions;
	}




	void Tuple::_perm( name_t tuple_type, TuplePerm permissions )
	{
		if( tuple_type == name_plain )
			Permissions = TuplePerm::modify;
		else if( tuple_type == name_dynamic || tuple_type == name_data )
			Permissions = TuplePerm::all_normal;
		else
			Permissions = permissions;
	}

	void Tuple::_assertAddPerm( name_t tuple_type, const TypeTuple& type ) const
	{
		if( !( Permissions & TuplePerm::add ) )
			throw type::AccessDenied( "Tuple does not permit adding attributes: " + eon::str( tuple_type ) );
		if( tuple_type == name_data
			&& ( !type.isName() || LegalForDataTuple.find( type.nameValue() ) == LegalForDataTuple.end() ) )
			throw type::AccessDenied( "Tuple does not permit adding attribute: " + eon::str( tuple_type ) + " cannot add "
				+ type.str() + "!" );
	}
	void Tuple::_assertRemPerm() const
	{
		if( !(Permissions & TuplePerm::remove ) )
			throw type::AccessDenied( "Tuple does not permit removing attributes: " + Type.at( name_type ).str() );
	}
	void Tuple::_assertModPerm() const
	{
		if( !(Permissions & TuplePerm::modify ) )
			throw type::AccessDenied( "Tuple does not permit modifying attributes: " + Type.at( name_type ).str() );
	}
	void Tuple::_assertTypPerm() const
	{
		if( !( Permissions & TuplePerm::types ) )
			throw type::AccessDenied( "Tuple does not permit types: " + Type.at( name_type ).str() );
	}
	void Tuple::_assertActPerm() const
	{
		if( !( Permissions & TuplePerm::actions ) )
			throw type::AccessDenied( "Tuple does not permit actions: " + Type.at( name_type ).str() );
	}

	void Tuple::_assertInRange( index_t attribute_num ) const
	{
		if( attribute_num >= Attributes.size() )
			throw type::NotFound();
	}


	void Tuple::_add( name_t tuple_type, Attribute&& value )
	{
		_assertAddPerm( tuple_type, value.type() );
		if( value.type() == name_tuple )
			value.value<Tuple>().Parent = this;
		Attributes.push_back( std::move( value ) );
	}
	void Tuple::_add( name_t tuple_type, name_t attribute_name, Attribute&& value )
	{
		if( attribute_name != no_name )
		{
			if( exists( attribute_name ) )
				throw type::DuplicateName( "Cannot add tuple attribute \"" + eon::str( attribute_name ) + "\" to tuple!" );
			_assertAddPerm( tuple_type, value.type() );
			NamedAttributes.insert( attribute_name, Attributes.size() );
		}
		if( value.type() == name_tuple )
			value.value<Tuple>().Parent = this;
		Attributes.push_back( std::move( value ) );
	}

	void Tuple::_str( Stringifier& strf, stack<const Tuple*>& ancestors, bool named_tuple ) const
	{
		// Tuple formatting rules
		//   1. Start out with formal type (e.g., "p(", "dynamic(", ...)
		//   2. Non-tuple attributes to be listed on the same line
		//   3. Unnamed tuple attributes to be listed on new lines
		//   4. Named tuple attributes to be listed with name + colon on new line, then tuple value on new line after that.
		//   5. Tuple values of same type as root will only start with "(" - no parenthesis if named value,
		//      otherwise include type
		//   6. Non-tuple attributes following a tuple attribute to be listed on same new line after tuple value
		//   7. End with closing ")"

		// 1. Start out with formal type (e.g., "p(", "dynamic(", ...)
		auto must_close = _strTupleStart( strf, ancestors, named_tuple );

		bool prev_was_tuple = false;
		bool first = true;
		bool indented = named_tuple;
		for( index_t i = 0; i < Attributes.size(); ++i )
		{
			auto& attribute = Attributes[ i ];
			if( first )
				first = false;
			else
			{
				strf.punct( ",", stringify::Type::end_block );
				strf.tert_split();
			}
			bool named = NamedAttributes.containsTwo( i );

			if( attribute.type().isTuple() )
			{
				if( !indented )
				{
					strf.start_block();
					indented = true;
				}
				else if( named )
					strf.hard_lf();

				if( named )
				{
					strf.word( eon::str( NamedAttributes.two( i ) ) );
					strf.punct( ":" );
					strf.start_block();
				}
				attribute.value<Tuple>()._str( strf, ancestors, named );
				if( named )
					strf.end_block();
				prev_was_tuple = true;
			}
			else
			{
				if( prev_was_tuple )
				{
					prev_was_tuple = false;
					if( !indented )
					{
						strf.start_block();
						indented = true;
					}
				}

				if( named )
				{
					strf.word( eon::str( NamedAttributes.two( i ) ) );
					strf.op2( "=" );
				}
				_strValue( strf , attribute );
			}
		}

		if( indented && !named_tuple )
			strf.end_block();
		ancestors.pop();
		if( must_close )
			strf.end_grp2( ")", stringify::Type::end_block );
	}
	bool Tuple::_strTupleStart( Stringifier& strf, stack<const Tuple*>& ancestors, bool named_tuple ) const
	{
		ancestors.push( this );
		name_t tuple_type = type().at( name_type ).nameValue();
		if( ancestors.size() == 1 || ancestors.bottom()->type().at( name_type ).nameValue() != tuple_type )
		{
			if( tuple_type == name_plain )
			{
				strf.prepend( "p" );
				strf.start_grp2( "(" );
			}
			else if( tuple_type == name_dynamic )
			{
				strf.prepend( "dynamic" );
				strf.start_grp2( "(" );
			}
			else if( tuple_type == name_data )
			{
				strf.prepend( "data" );
				strf.start_grp2( "(" );
			}
			return true;
		}
		else if( !named_tuple )
		{
			strf.start_grp2( "(" );
			return true;
		}
		else
			return false;
	}
	void Tuple::_strValue( Stringifier& strf, const Attribute& attribute ) const
	{
		name_t type = attribute.type().nameValue();
		auto prefix = StrPrefixPostfixForTypes.find( type );
		if( prefix != StrPrefixPostfixForTypes.end() )
			strf.prepend( prefix->second.first );

		if( type == name_bool )
			strf.word( string( attribute.value<bool>() ) );
		else if( type == name_byte )
			strf.word( string( attribute.value<byte_t, type::hint::byte>() ) );
		else if( type == name_char )
			strf.word( string( attribute.value<char_t>() ) );
		else if( type == name_int )
			strf.word( string( attribute.value<int_t>() ) );
		else if( type == name_short )
			strf.word( string( attribute.value<short_t>() ) );
		else if( type == name_long )
			strf.word( string( attribute.value<long_t>() ) );
		else if( type == name_float )
			strf.word( string( attribute.value<flt_t>() ) );
		else if( type == name_low )
			strf.word( string( attribute.value<low_t>() ) );
		else if( type == name_high )
			strf.word( string( attribute.value<high_t>() ) );
		else if( type == name_index )
			strf.word( string( attribute.value<index_t, type::hint::index>() ) );
		else if( type == name_name )
			strf.word( eon::str( attribute.value<name_t, type::hint::name>() ) );
		else if( type == name_b8 )
			strf.word( string( attribute.value<b8_t, type::hint::bits>() ) );
		else if( type == name_b16 )
			strf.word( string( attribute.value<b16_t, type::hint::bits>() ) );
		else if( type == name_b32 )
			strf.word( string( attribute.value<b32_t, type::hint::bits>() ) );
		else if( type == name_b64 )
			strf.word( string( attribute.value<b64_t, type::hint::bits>() ) );
		else if( type == name_bytes )
			strf.word( string( attribute.value<std::string>() ) );
		else if( type == name_string )
			strf.word( attribute.value<string>() );
//		else if( type == name_real )
//			strf.word( string( attribute.value<real_t>() ) );
//		else if( type == name_complex )
//			strf.word( string( attribute.value<complex_t>() ) );
		else if( type == name_regex )
			strf.word( attribute.value<regex>().str() );
//		else if( type == name_namepath )
//			strf.word( string( attribute.value<namepath_t>() ) );
//		else if( type == name_path )
//			strf.word( string( attribute.value<path_t>() ) );
//		else if( type == name_time )
//			strf.word( string( attribute.value<time_t>() ) );
//		else if( type == name_date )
//			strf.word( string( attribute.value<date_t>() ) );
//		else if( type == name_timezone )
//			strf.word( string( attribute.value<timezone_t>() ) );
//		else if( type == name_dst )
//			strf.word( string( attribute.value<dst_t>() ) );
//		else if( type == name_timespan )
//			strf.word( string( attribute.value<timespan_t>() ) );
//		else if( type == name_expression )
//			strf.word( string( attribute.value<expression_t>() ) );

		if( prefix != StrPrefixPostfixForTypes.end() )
			strf.append( prefix->second.second );
	}

	std::unordered_set<name_t> Tuple::LegalForDataTuple;
	void Tuple::_populateLegalForDataTuple()
	{
		if( LegalForDataTuple.empty() )
		{
			LegalForDataTuple = {
				name_bool,
				name_byte, name_char,
				name_int, name_short, name_long,
				name_float, name_low, name_high,
				name_index,
				name_name,
				name_string,
				name_real, name_complex,
				name_regex,
				name_namepath,
				name_path,
				name_time, name_date, name_timezone, name_dst, name_timespan,
				name_expression};
		}
	}

	std::unordered_map<name_t, std::pair<string, string>> Tuple::StrPrefixPostfixForTypes;
	void Tuple::_populateStrPrefixPostfixForTypes()
	{
		if( StrPrefixPostfixForTypes.empty() )
		{
			StrPrefixPostfixForTypes = {
				{ name_bool, { "", "" } },
				{ name_byte, { "B'", "'" } },
				{ name_char, { "'", "'" } },
				{ name_int, { "", "" } },
				{ name_short, { "", "" } },
				{ name_long, { "", "" } },
				{ name_float, { "", "" } },
				{ name_low, { "", "" } },
				{ name_high, { "", "" } },
				{ name_index, { "", "" } },
				{ name_name, { "", "" } },
				{ name_bytes, { "B\"", "\"" } },
				{ name_string, { "\"", "\"" } },
				{ name_real, { "", "r" } },
				{ name_complex, { "", "i" } },
				{ name_regex, { "r\"", "\"" } },
				{ name_namepath, { "@", "" } },
				{ name_path, { "p\"", "\"" } },
				{ name_time, { "", "" } },
				{ name_date, { "", "" } },
				{ name_timezone, { "", "" } },
				{ name_dst, { "", "" } },
				{ name_timespan, { "", "" } },
				{ name_expression, { "e(", ")" } } };
		}
	}
}
