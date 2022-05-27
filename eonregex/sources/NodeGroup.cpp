#include "../NodeGroup.h"


namespace eon
{
	namespace rx
	{
		bool NodeGroup::_match( RxData& data, size_t steps )
		{
			if( Head != nullptr )
			{
				RxData param_b{ data };
				if( Head->match( param_b, steps ) )
				{
					data = std::move( param_b );
					return true;
				}
				else
					return false;
			}
			else		// Empty group (should be optimized away)
				return true;
		}

		Node* NodeGroup::_removeSuperfluousGroups() noexcept
		{
			// Recurse first
			if( Head )
				Head = Head->_removeSuperfluousGroups();
			if( Next )
				Next = Next->_removeSuperfluousGroups();

			// If we only have one element, then grouping is not necessary!
			if( Head && !Head->Next )
			{
				auto node = Head; Head = nullptr;
				node->Next = Next; Next = nullptr;
				if( Quant.Min > 1 )
					node->Quant.Min += ( Quant.Min - 1 );
				if( Quant.Max == SIZE_MAX )
					node->Quant.Max = SIZE_MAX;
				else if( node->Quant.Max != SIZE_MAX && Quant.Max > 1 )
					node->Quant.Max += ( Quant.Max - 1 );
				node->Name = Name;
				node->Open = Open;
				node->PreAnchoring |= PreAnchoring;
				delete this;
				return node;
			}
			return this;
		}
		void NodeGroup::_failFastFixedEnd( Node& head )
		{
			if( Head )
				Head->_failFastFixedEnd( *Head );
			if( Next )
				Next->_failFastFixedEnd( head );
		}

		void NodeGroup::_append( Node* node ) noexcept
		{
			if( !_locked() )
			{
				if( Head != nullptr )
				{
					Head = node;
					_Cur = Head;
				}
				else
				{
					_Cur->Next = node;
					_Cur = _Cur->Next;
				}
			}
		}
	}
}
