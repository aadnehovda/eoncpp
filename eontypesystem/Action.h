#pragma once

#include "Definitions.h"
#include "TypeTuple.h"
#include "Attribute.h"



///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// The 'type' namespace enclosing all background type functionality
	//
	namespace type
	{
		// Action return types
		enum class sig_t : uint8_t
		{
			undef,
			norm,
			brk,
			cont,
			ret,
			raise
		};



		///////////////////////////////////////////////////////////////////////
		//
		// Super-class for action objects
		//
		class Action
		{
		public:
			Action() = delete;
			Action( TypeTuple signature ) { Signature = signature; }
			virtual ~Action() = default;

			inline const TypeTuple& signature() const noexcept { return Signature; }

			virtual sig_t operator()( stack<Attribute>& values ) const = 0;

		private:
			TypeTuple Signature;
		};
	}
}
