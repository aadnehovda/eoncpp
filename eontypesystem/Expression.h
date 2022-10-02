#pragma once

#include "Definitions.h"
#include "Action.h"
#include "Operators.h"
#include <eonsource/File.h>
#include <eonsource/String.h>
#include <eonsource/SourceRef.h>
#include <eonsource/SourceReporter.h>
#include <eontokenizer/Tokenizer.h>
#include <eontokenizer/ReTokenizer.h>
#include <eontokenizer/TokenParser.h>
#include <eonstring/NamePath.h>
#include <eonfilesys/Path.h>


///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	EONEXCEPT( InvalidExpression );




	///////////////////////////////////////////////////////////////////////////
	//
	// Expression class - eon::Expression
	//
	class Expression
	{
		///////////////////////////////////////////////////////////////////////
		//
		// Construction
		//
	public:

		// Construct a void expression - a place-holder
		Expression() = default;

		// Construct for an infix expression
		// Specify the fully formed scope in which the expression will execute!
		// Issues will be reported to stdout and stderr!
		// Throws eon::InvalidExpression if not a valid expression!
		Expression( string&& infix, Tuple& scope );

		// Construct for an infix expression ready at current parser position
		// NOTE: For expression variables, the leading '(' must be the current token!
		// Specify the fully formed scope in which the expression will execute!
		// If there are issues, they will be reported using the specified
		// eon::source::Reporter.
		// Throws eon::InvalidExpression if not a valid expression
		Expression( TokenParser& parser, Tuple& scope, source::Reporter& reporter, std::set<string> terminate_on );

		// Copy another expression
		inline Expression( const Expression& other ) { *this = other; }

		// Take ownership of an other expression's data
		inline Expression( Expression&& other ) noexcept { *this = std::move( other ); }

		~Expression() = default;




		///////////////////////////////////////////////////////////////////////
		//
		// Modifiers
		//
	public:

		// Copy another expression
		Expression& operator=( const Expression& other );

		// Take ownership of another expression's data
		Expression& operator=( Expression&& other ) noexcept;




		///////////////////////////////////////////////////////////////////////
		//
		// Read-only Methods
		//
	public:

		// Check if the expression is empty or not
		inline operator bool() const noexcept { return !Roots.empty(); }

		// Get object as infix string representation
		void str( Stringifier& strf ) const;

		// Get object as infix string representation
		string str() const;




		///////////////////////////////////////////////////////////////////////
		//
		// Execution
		//
	public:

		// Run expression and get zero or more values in return.
		// Throws:
		//   - eon::type::NotFound : Expression is void!
		std::vector<Attribute> execute();




		///////////////////////////////////////////////////////////////////
		//
		// Definitions
		//
	private:

		enum class Seen : uint8_t
		{
			_operand,
			_operator,
		};

		struct Operator
		{
			Operator() = default;
			inline Operator( name_t op, source::Ref source ) { Op = op; Source = source; }
			name_t Op{ no_name };
			source::Ref Source;
		};

		class Node;

		class ParseData
		{
		public:
			ParseData() = delete;
			inline ParseData( TokenParser& parser, source::Reporter& reporter, std::set<string> terminate_on ) {
				Parser = &parser; Reporter = &reporter; TerminateOn = terminate_on; }
			inline ParseData( ParseData& other, std::set<string> terminate_on ) {
				Parser = other.Parser; Reporter = other.Reporter; TerminateOn = terminate_on; }

			inline TokenParser& parser() noexcept { return *Parser; }
			inline source::Reporter& reporter() noexcept { return *Reporter; }

			inline void error() noexcept { Errors = true; }
			inline bool errors() const noexcept { return Errors; }
			
			inline stack<Node>& operands() noexcept { return Operands; }
			inline stack<Operator>& operators() noexcept { return Operators; }

			inline void lastSeenOperand() noexcept { LastSeen = Seen::_operand; }
			inline void lastSeenOperator() noexcept { LastSeen = Seen::_operator; }
			inline Seen lastSeen() const noexcept { return LastSeen; }

			inline bool terminateOn( string type ) const noexcept { return TerminateOn.find( type ) != TerminateOn.end(); }

		private:
			TokenParser* Parser{ nullptr };
			source::Reporter* Reporter{ nullptr };
			Seen LastSeen{ Seen::_operator };
			std::set<string> TerminateOn;
			stack<Node> Operands;
			stack<Operator> Operators;
			bool Errors{ false };
		};

		class Node
		{
		public:
			Node() = default;
			inline Node( Attribute&& value, source::Ref source ) { Value = std::move( value ); Source = source; }
			inline Node( name_t op_name, source::Ref source = source::Ref() ) { OpName = op_name; Source = source; }
			inline Node( const type::Action& action, source::Ref call_source ) {
				OpName = name_call; Operator = &action; Source = Source;
				OpReturnType = action.signature().at( name_return ); }
			inline Node( const Node& other ) { *this = other; }
			inline Node( Node&& other ) noexcept { *this = std::move( other ); }
			~Node() = default;

			inline Node& operator=( const Node& other ) {
				OpName = other.OpName; OpReturnType = other.OpReturnType; Value = other.Value; Source = other.Source;
				Operator = other.Operator; Children = other.Children; return *this; }
			inline Node& operator=( Node&& other ) noexcept {
				OpName = other.OpName; other.OpName = no_name; OpReturnType = std::move( other.OpReturnType );
				Value = std::move( other.Value ); Source = other.Source;
				Operator = other.Operator; other.Operator = nullptr; Children = std::move( other.Children ); return *this; }

			inline operator bool() const noexcept { return OpName || Value.type(); }
			inline bool isOperator() const noexcept { return OpName != no_name && OpName != name_call; }
			inline bool isCall() const noexcept { return OpName == name_call; }
			inline bool isValue() const noexcept { return ( OpName == no_name || OpName == name_call ) && Value.type(); }

			inline name_t opName() const noexcept { return OpName; }
			inline TypeTuple opReturnType() const noexcept { return OpReturnType; }
			inline Attribute& value() noexcept { return Value; }
			inline const source::Ref& source() const noexcept { return Source; }

			inline void addOperand( Node&& opr ) { Children.push_front( std::move( opr ) ); }

			bool findOperator( Tuple& scope, source::Reporter& reporter );

			Attribute evaluate();

			void str( Stringifier& strf ) const;

		private:
			name_t OpName{ no_name };
			TypeTuple OpReturnType;
			Attribute Value;
			source::Ref Source;
			const type::Action* Operator{ nullptr };
			std::list<Node> Children;
		};




		///////////////////////////////////////////////////////////////////////
		//
		// Helpers
		//
	private:

		void _prepTokenizer( Tokenizer& tokenizer );
		void _prepReTokenizer( ReTokenizer& retokenizer );

		void _runParse( ParseData& data );

		bool _parse( ParseData& data );

		bool _parseLiteral( ParseData& data );

		bool _parseOperator( ParseData& data );
		bool _processOperator( ParseData& data );
		bool _processOperator( ParseData& data, name_t op_name, source::Ref op_source );
		bool _processClose( ParseData& data );
		bool _processPlainOperator( ParseData& data, name_t op_name, source::Ref new_op_source );
		bool _processNamedOperator( ParseData& data, name_t op_name );
		void _bindOperator( ParseData& data );
		name_t _resolveOperator( ParseData& data, name_t op_name );
		name_t _findSolidOpName( ParseData& data, const std::vector<name_t>& options, name_t op_name, index_t op_no );
		std::vector<name_t> _discardPrefixOperatorOptions( const std::vector<name_t>& options, index_t op_no );
		std::set<string> _findStopOn( const std::vector<name_t>& options, index_t op_no );
		std::vector<name_t> _matchNextSequenceOperator( ParseData& data, const std::vector<name_t>& options, index_t op_no );
		name_t _processSequenceOperator( ParseData& data, name_t op_name, std::vector<Node>&& operands );

		bool _parseTuple( ParseData& data );
		bool _parseTuple( ParseData& data, name_t tupletype );
		bool _parseTypeTuple( ParseData& data );
		TypeTuple _processTypeTuple( ParseData& data );

		bool _parseName( ParseData& data );

		bool _parseActionCall( ParseData& data, name_t action_name );

		bool _parseExpression( ParseData& data );




		///////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	private:

		Tuple* Scope{ nullptr };
		std::list<Node> Roots;
	};
}
