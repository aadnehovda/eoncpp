#include "FixedValue.h"


namespace eon
{
	namespace rx
	{
		bool FixedValue::_match( RxData& data, size_t steps )
		{
			RxData param_b{ data };

			for( auto c : Value )
			{
				if( param_b() == c )
					param_b.advance();
				else
					return false;
			}
			data = std::move( param_b );
			return true;
		}

		Node* FixedValue::_exposeLiterals()
		{
			if( Quant.minQ() == 1 && Quant.maxQ() == SIZE_MAX )
			{
				auto exposed = copy();
				exposed->resetQuantifier();
				Quant = Quantifier( 0, SIZE_MAX, Quant.greedy() );
				exposed->Next = this;
				return exposed;
			}
			else
				return this;
		}
	}
}
