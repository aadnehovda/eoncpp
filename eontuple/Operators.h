#pragma once
#include <eonstring/String.h>
#include <eonexcept/Exception.h>


/******************************************************************************
  The 'eon' namespace encloses all public functionality
******************************************************************************/
namespace eon
{
	/**************************************************************************
	  The 'eon::tup' namespace is for specific �on Tuple Format elements
	**************************************************************************/
	namespace tup
	{
		//* Exception thrown when an operand is tried evaluated on an
		//* unsupported operand.
		EONEXCEPT( UnsupportedOperand );



		/**********************************************************************
		  Eon Operators Class - eon::operators

		  This class defines the operators (and their precedence) supported by
		  attribute types.
		**********************************************************************/
		class operators
		{
			/**********************************************************************
			  Definitions
			**********************************************************************/
		public:

			//* Operator codes
			enum class code
			{
				undef,				// Undefined
				value,				// Not an operator
				open_brace,			// (
				close_brace,		// )
				_and,				// a and b
				_or ,				// a or b
				_not,				// not a
				if_else,			// a if b else c
				eq,					// a == b
				ne,					// a != b
				lt,					// a < b
				le,					// a <= b
				gt,					// a > b
				ge,					// a >= b
				plus,				// a + b
				minus,				// a - b
				unary_minus,		// -a
				multiply,			// a * b
				divide,				// a / b
				pow,				// a pow b
				mod,				// a mod b
				sqrt,				// sqrt a
				match,				// a match b
				assign,				// a = b

				dummies,
				if_else2,			// The "else" part of if_else
			};
			static code mapCode( const string& str );
			static string mapCode( code op_code );

			//* Details about an operator
			//* Code
			//* Number of operands
			//* Precedence when read from infix expression
			//* Precedence when on the operator stack
			struct OperatorDetails
			{
				OperatorDetails() = default;
				inline OperatorDetails( code op_code, int num_operands,
					int input_precedence, int stack_precedence ) noexcept {
					Type = op_code; NumOperands = num_operands;
					InputPrecedence = input_precedence;
					StackPrecedence = stack_precedence; }

				code Type{ code::undef };
				size_t NumOperands{ 0 };
				int InputPrecedence{ 0 };
				int StackPrecedence{ 0 };
			};




			/**********************************************************************
			  Construction
			**********************************************************************/
		public:

			operators() = delete;




			/**********************************************************************
			  Static Methods
			**********************************************************************/
		public:

			static inline size_t numOperands( code op_code ) {
				return Ops[ static_cast<size_t>( op_code ) ].NumOperands; }

			static inline int inputPrecedence( code op_code ) {
				return Ops[ static_cast<size_t>( op_code ) ].InputPrecedence; }
			static inline int stackPrecedence( code op_code ) {
				return Ops[ static_cast<size_t>( op_code ) ].StackPrecedence; }




			/**********************************************************************
			  Helpers
			**********************************************************************/
		private:

			static inline void add( OperatorDetails&& op ) {
				Ops.push_back( std::move( op ) ); }




			///////////////////////////////////////////////////////////////////////
			//
			// Attributes
			//
		private:
			static std::vector<OperatorDetails> Ops;
		};
	}
}
