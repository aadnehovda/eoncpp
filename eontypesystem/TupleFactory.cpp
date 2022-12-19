#include "TupleFactory.h"


namespace eon
{
	namespace scope
	{
		Tuple& global()
		{
			static Tuple scope;
			if( !scope )
				scope = Tuple( name_scope, TuplePerm::all_normal | TuplePerm::types | TuplePerm::actions );
			return scope;
		}

		Tuple& package( name_t package_name )
		{
			auto scope_name = compilerName( "package#" + str( package_name ) );
			if( global().exists( scope_name ) )
				return global().at<Tuple>( scope_name );
			else
				return global().add( scope_name, Tuple( name_scope ) );
		}

		Tuple scope( std::initializer_list<AttributePair> attributes )
		{
			Tuple scope( name_scope, attributes, TuplePerm::modify | TuplePerm::actions );
			return scope;
		}
	}



	namespace cache
	{
		Tuple& cache( name_t name, TuplePerm permissions )
		{
			auto cache_name = compilerName( "cache#" + str( name ) );
			if( scope::global().exists( cache_name ) )
				throw type::DuplicateName( "Cannot create cache \"" + str( name ) + "\"" );
			else
				return scope::global().add( cache_name, Tuple( name_cache, permissions ) );
		}

		Tuple& cache( name_t name, std::initializer_list<AttributePair> attributes, TuplePerm permissions )
		{
			auto cache_name = compilerName( "cache#" + str( name ) );
			if( scope::global().exists( cache_name ) )
				throw type::DuplicateName( "Cannot create cache \"" + str( name ) + "\"" );
			else
				return scope::global().add( cache_name, Tuple( name_cache, attributes, permissions ) );
		}

		Tuple& cache( name_t name )
		{
			auto cache_name = compilerName( "cache#" + str( name ) );
			if( !scope::global().exists( cache_name ) )
				throw type::NotFound( "Cannot access cache \"" + str( name ) + "\"" );
			else
				return scope::global().at<Tuple>( cache_name );
		}
	}



	namespace construct
	{
		Tuple& unit( name_t name, std::initializer_list<AttributePair> attributes )
		{
			auto unit_name = compilerName( "unit#" + str( name ) );
			if( scope::global().exists( unit_name ) )
				throw type::DuplicateName( "Cannot create unit \"" + str( name ) + "\"" );
			else
				return scope::global().add( unit_name,
					Tuple( name_unit, attributes, TuplePerm::all_normal | TuplePerm::types ) );
		}

		Tuple& unit( name_t name )
		{
			auto unit_name = compilerName( "unit#" + str( name ) );
			if( !scope::global().exists( unit_name ) )
				throw type::NotFound( "Cannot access unit \"" + str( name ) + "\"" );
			else
				return scope::global().at<Tuple>( unit_name );
		}
	}
}
