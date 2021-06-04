#pragma once

#include "Node.h"


/******************************************************************************
  The 'eon' namespace encloses all public functionality
******************************************************************************/
namespace eon
{
	/**************************************************************************
	  The 'eon::rx' namespace enclosed special elements for Eon regular
	  expressions
	**************************************************************************/
	namespace rx
	{
		/*
		* Operator node
		* Matches only if next node doesn't match
		*/
		class OpNot : public Node
		{
		public:
			OpNot( const substring& source ) : Node( source ) {
				Type = NodeType::op_not; }
			OpNot( const OpNot& other ) : Node( other ) { *this = other; }
			OpNot( OpNot&& other ) noexcept : Node( std::move( other ) ) {
				*this = std::move( other ); }
			OpNot( Node* value, const substring& source ) : Node( source ) {
				Type = NodeType::op_not; Value = value; }
			virtual ~OpNot() { if( Value != nullptr ) delete Value; }

			inline Node* copy() const override { return new OpNot( *this ); }

			inline OpNot& operator=( const OpNot& other ) {
				*static_cast<Node*>( this ) = other;
				Value = other.Value != nullptr ? other.Value->copy() : nullptr;
				return *this; }
			inline OpNot& operator=( OpNot&& other ) noexcept {
				*static_cast<Node*>( this ) = std::move( other );
				Value = other.Value; other.Value = nullptr; return *this; }

		private:
			inline bool _match( RxData& data, size_t steps ) override {
				return !Value->match( data, steps );
			}

		private:
			Node* Value{ nullptr };

			friend class Graph;
		};
	}
}