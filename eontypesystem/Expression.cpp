#include "Expression.h"
#include "Tuple.h"


namespace eon
{
	Expression::Expression( string&& infix, Tuple& scope )
	{
		Scope = &scope;

		Tokenizer tokenizer; _prepTokenizer( tokenizer );
		ReTokenizer retokenizer; _prepReTokenizer( retokenizer );

		source::String src( "string", std::move( infix ) );
		TokenParser temp( tokenizer( src ) );
		TokenParser parser( retokenizer( temp ) );
		source::Reporter issues;
		issues.defaultTargets();
		ParseData data( parser, issues, { ";" } );
		_runParse( data );
	}
	Expression::Expression( TokenParser& parser, Tuple& scope, source::Reporter& reporter, std::set<string> terminate_on )
	{
		Scope = &scope;
		ParseData data( parser, reporter, terminate_on );
		_runParse( data );
	}




	Expression& Expression::operator=( const Expression& other )
	{
		Scope = other.Scope;
		Roots = other.Roots;
		return *this;
	}
	Expression& Expression::operator=( Expression&& other ) noexcept
	{
		Scope = other.Scope; other.Scope = nullptr;
		Roots = std::move( other.Roots );
		return *this;
	}

	void Expression::str( Stringifier& strf ) const
	{
		bool first = true;
		for( auto& node : Roots )
		{
			if( first )
				first = false;
			else
				strf.punct( ";" );
			node.str( strf );
		}
	}
	string Expression::str() const
	{
		Stringifier strf;
		str( strf );
		return strf.generateString();
	}




	std::vector<Attribute> Expression::execute()
	{
		if( !Scope )
			throw type::NotFound( "Cannot execute void expression!" );
		std::vector<Attribute> results;
		for( auto& node : Roots )
			results.push_back( node.evaluate() );
		return results;
	}




	Attribute Expression::Node::evaluate()
	{
		if( isValue() )
			return value();

		stack<Attribute> values;
		for( auto& child : Children )
			values.push( child.evaluate() );

		( *Operator )( values );

		if( values.size() == 1 )
		{
			auto value = std::move( values.top() );
			return value;
		}
		else if( values.empty() )
			return Attribute();
		else
		{
			// TODO: Is this even possible?
			auto value = std::move( values.top() );
			return value;
		}
	}

	bool Expression::Node::findOperator( Tuple& scope, source::Reporter& reporter )
	{
		if( Operator != nullptr )
			return true;
		auto req_operands = type::Operators::numOperands( OpName );
		if( Children.size() != req_operands )
		{
			if( req_operands == 1 )
				reporter.error( "Expected one operand, not " + ( Children.empty() ? string( "none" )
					: string( Children.size() ) ) + "!", Source );
			return false;
		}

		// Make sure all the children have their operators (and return types).
		for( auto& child : Children )
		{
			if( child.isOperator() )
			{
				if( !child.findOperator( scope, reporter ) )
					return false;
			}
		}

		if( type::Operators::isTypeImplemented( OpName ) )
		{
			auto child = Children.begin();
			auto type = child->isOperator() || child->isCall() ? child->OpReturnType : child->Value.type();
			TypeTuple args;
			for( ++child; child != Children.end(); ++child )
			{
				if( child->isOperator() || child->isCall() )
				{
					if( child->OpReturnType.isName() )
						args.add( child->OpReturnType.nameValue() );
					else
						args.add( child->OpReturnType );
				}
				else
					args.add( child->Value.type() );
			}
			auto actions = scope.signatures( OpName, type.nameValue(), args );
			if( actions.size() == 1 )
			{
				// We have a match.
				Operator = scope.action( *actions.begin() );
				OpReturnType = Operator->signature().at( name_return );
			}
			else if( actions.size() > 0 )
			{
				// More than one - we must check return value!
				// TODO: Check return value!
			}
			else
			{
				static std::set<name_t> cmp_ops{ symbol_lt, symbol_le, symbol_gt, symbol_ge, symbol_eq, symbol_ne };

				// If we were looking for a comparison operator other than '<=>', we can use '<=>' instead.
				if( cmp_ops.find( OpName ) != cmp_ops.end() )
				{
					actions = scope.signatures( symbol_cmp, type.nameValue(), args );
					if( actions.size() == 1 )
					{
						// Found it. Now we need to do some re-organizing. We need to change from:
						//          .-- <child 1>
						//   <op> --|
						//          '-- <child 2>
						// to:
						//                    .-- <child 1>
						//          .-- cmp --|
						//          |         '-- <child 2>
						//   <op> --|
						//          '-- 0
						Node cmp( symbol_cmp );
						cmp.Operator = scope.action( *actions.begin() );
						OpReturnType = name_int;
						cmp.Children = std::move( Children );
						Children.push_back( std::move( cmp ) );
						Children.push_back( Node( Attribute::newImplicit( int_t( 0 ) ), source::Ref() ) );

						// Now we can use the original comparison operator towards int
						actions = scope.signatures( OpName, name_int, TypeTuple( { { no_name, name_int } } ) );
						Operator = scope.action( *actions.begin() );
						OpReturnType = name_bool;
					}
				}
			}
			if( Operator == nullptr )
				reporter.error( "Operator '" + eon::str( OpName ) + "' is not implemented for " + type.str() + "!",
					Source );
			return Operator != nullptr;
		}
		else
		{
			// TODO: Deal with system-implemented operators ...
			return false;
		}
	}
	
	void Expression::Node::str( Stringifier& strf ) const
	{
		if( isOperator() )
		{
			auto& sequence = type::Operators::sequence( OpName );
			auto child = Children.begin();
			for( auto& elm : sequence )
			{
				if( elm.Prefix )
					strf.op1( eon::str( elm.Op ) );
				else
				{
					child->str( strf );
					strf.op1( eon::str( elm.Op ) );
					++child;
				}
			}
			for( ; child != Children.end(); ++child )
				child->str( strf );
		}
		else if( Value )
			Value.str( strf );
	}




	void Expression::_prepTokenizer( Tokenizer& tokenizer )
	{
		tokenizer.registerEonNameTokens( true );
		tokenizer.registerTokenChar( name_space, ' ', Tokenizer::Match::sequence );
		tokenizer.registerTokenChar( name_doublequote, '"', Tokenizer::Match::single );
		tokenizer.registerTokenChar( name_singlequote, '\'', Tokenizer::Match::single );
		tokenizer.registerTokenCharcat( name_digits, charcat::number_ascii_digit, Tokenizer::Match::sequence );
		tokenizer.registerTokenChar( name_underscore, '_', Tokenizer::Match::sequence );
		tokenizer.registerTokenCharcat( name_letters, charcat::letter_lowercase, Tokenizer::Match::sequence );
		tokenizer.registerTokenCharcat( name_letters, charcat::letter_uppercase, Tokenizer::Match::sequence );
		tokenizer.registerTokenCharcat( name_letters, charcat::letter_titlecase, Tokenizer::Match::sequence );
		tokenizer.registerTokenCharcat( name_letters, charcat::letter_modifier, Tokenizer::Match::sequence );
		tokenizer.registerTokenCharcat( name_letters, charcat::letter_other, Tokenizer::Match::sequence );
		tokenizer.registerTokenChar( symbol_open_round, '(', Tokenizer::Match::single );
		tokenizer.registerTokenChar( symbol_close_round, ')', Tokenizer::Match::single );
		tokenizer.registerTokenChar( name_at, '@', Tokenizer::Match::single );
		tokenizer.registerTokenChar( name_backslash, '\\', Tokenizer::Match::single );
		tokenizer.registerTokenChar( name_assign, '=', Tokenizer::Match::single );
		tokenizer.registerTokenChar( symbol_minus, '-', Tokenizer::Match::single );
		tokenizer.registerTokenChar( symbol_point, '.', Tokenizer::Match::sequence );
		tokenizer.registerTokenChar( symbol_colon, ':', Tokenizer::Match::sequence );
		tokenizer.registerTokenChar( symbol_comma, ',', Tokenizer::Match::single );
		tokenizer.registerSequenceToken( name( "line_comment" ), "//" );
		tokenizer.registerSequenceToken( name( "comment_start" ), "/-" );
		tokenizer.registerSequenceToken( name( "comment_end" ), "-/" );
		tokenizer.registerTokenChar( symbol_divide, '/', Tokenizer::Match::single );
		tokenizer.registerTokenChar( symbol_semicolon, ';', Tokenizer::Match::single );
		tokenizer.registerTokenChars( name_operator, "+*~&|^<>[]", Tokenizer::Match::single );
		tokenizer.registerSequenceToken( symbol_eq, "==" );
		tokenizer.registerSequenceToken( symbol_ne, "!=" );
		tokenizer.registerSequenceToken( symbol_ge, ">=" );
		tokenizer.registerSequenceToken( symbol_le, "<=" );
		tokenizer.registerSequenceToken( symbol_plus_assign, "+=" );
		tokenizer.registerSequenceToken( symbol_minus_assign, "-=" );
		tokenizer.registerSequenceToken( symbol_multiply_assign, "*=" );
		tokenizer.registerSequenceToken( symbol_divide_assign, "/=" );
		tokenizer.registerSequenceToken( symbol_member, "'s" );
		tokenizer.registerSequenceToken( symbol_lshift, "<<" );
		tokenizer.registerSequenceToken( symbol_rshift, ">>" );
		tokenizer.registerSequenceToken( symbol_cmp, "<=>" );
		tokenizer.registerSequenceToken( name_typetuple, "T(" );
		tokenizer.registerSequenceToken( name_plain, "p(" );
		tokenizer.registerSequenceToken( name_dynamic, "dynamic(" );
		tokenizer.registerSequenceToken( name_data, "data(" );
		tokenizer.registerSequenceToken( name_expression, "e(" );
		tokenizer.registerTokenChar( name_hash, '#', Tokenizer::Match::single );
	}
	void Expression::_prepReTokenizer( ReTokenizer& retokenizer )
	{
		retokenizer.addRule( new ReTokenizer::EncloseRule( name_string, name_doublequote, name_backslash ) );
		retokenizer.addRule( new ReTokenizer::PrefixEncloseRule( name_bytes, "B", name_doublequote, name_backslash ) );
		retokenizer.addRule( new ReTokenizer::PrefixEncloseRule( name_path, "p", name_doublequote, name_backslash ) );
		retokenizer.addRule( new ReTokenizer::EncloseRule( name_char, name_singlequote, name_backslash ) );
		retokenizer.addRule( new ReTokenizer::PrefixEncloseRule( name_byte, "B", name_singlequote, name_backslash ) );
		retokenizer.addRule( new ReTokenizer::PrefixEncloseRule( name_regex, "r", name_doublequote, name_backslash ) );
		retokenizer.addRule( new ReTokenizer::LinestartRule( name_indentation, name_space ) );
		retokenizer.addRule( new ReTokenizer::LiteralNameRule( name_bool, { "true", "false" } ) );
		retokenizer.addRule( new ReTokenizer::SequenceRule( name_float, { name_digits, symbol_point, name_digits } ) );
		retokenizer.addRule( new ReTokenizer::SequenceRule( name_literal, { name_hash, name_name } ) );
		retokenizer.addRule( new ReTokenizer::PrefixAlternatingRule( name_namepath, "@", name_name, symbol_divide ) );
		retokenizer.addRule( new ReTokenizer::EncloseRule( name_comment, name( "linecomment" ), name_newline ) );
		retokenizer.addRule(
			new ReTokenizer::EncloseRule( name_comment, name( "commentstart" ), name( "commentend" ) ) );
		retokenizer.addRule( new ReTokenizer::RemoveRule( { name_space } ) );
	}

	void Expression::_runParse( ParseData& data )
	{
		while( true )
		{
			data.lastSeenOperator();
			if( _parse( data ) )
			{
				Roots.push_back( std::move( data.operands().top() ) );
				data.operands().pop();
			}
			if( !data.parser() )
				break;
			if( data.terminateOn( data.parser().current().str() ) )
				break;
		}

		if( data.errors() )
			throw InvalidExpression();
	}

	bool Expression::_parse( ParseData& data )
	{
		while( data.parser()
			&& !data.terminateOn( data.parser().current().str() )
			&& !data.parser().current().is( symbol_semicolon ) )
		{
			if( _parseLiteral( data ) )
			{
				data.lastSeenOperand();
				data.parser().forward();
			}
			else if( _parseOperator( data ) )
				data.parser().forward();
			else if( _parseTuple( data ) )
				data.parser().forward();
			else if( _parseName( data ) )
				data.parser().forward();
			else if( _parseExpression( data ) )
				data.parser().forward();
			else
			{
				data.error();
				data.reporter().error(
					"Unknown element \"" + data.parser().current().str() + "\"", data.parser().current().source() );
				data.parser().forward();
				break;
			}
		}
		if( data.parser() && data.parser().current().is( symbol_semicolon ) )
			data.parser().forward();
		while( !data.operators().empty() )
			_bindOperator( data );
		return !data.operands().empty();
	}

	bool Expression::_parseLiteral( ParseData& data )
	{
		auto type = data.parser().current().type();
		if( type == name_bool )
		{
			data.operands().push( Node( Attribute::newExplicit( data.parser().current().str() == "true", name_bool ),
				data.parser().current().source() ) );
			return true;
		}
		else if( type == name_byte )
		{
			data.operands().push( Node( Attribute::newExplicit( static_cast<byte_t>(
				*data.parser().current().str().begin() ), name_byte ), data.parser().current().source() ) );
			return true;
		}
		else if( type == name_char )
		{
			data.operands().push( Node( Attribute::newExplicit( *data.parser().current().str().begin(), name_char ),
				data.parser().current().source() ) );
			return true;
		}
		else if( type == name_int )
		{
			data.operands().push( Node( Attribute::newExplicit( data.parser().current().str().toInt(), name_int ),
				data.parser().current().source() ) );
			return true;
		}
		else if( type == name_float )
		{
			data.operands().push( Node( Attribute::newExplicit( data.parser().current().str().toFloat(), name_float ),
				data.parser().current().source() ) );
			return true;
		}
		else if( type == name_digits )
		{
			data.operands().push( Node( Attribute::newExplicit( data.parser().current().str().toInt(), name_int ),
				data.parser().current().source() ) );
			return true;
		}
		else if( type == name_bytes )
		{
			data.operands().push( Node( Attribute::newExplicit( data.parser().current().bytes(), name_bytes ),
				data.parser().current().source() ) );
			return true;
		}
		else if( type == name_string )
		{
			data.operands().push( Node( Attribute::newExplicit( data.parser().current().str(), name_string ),
				data.parser().current().source() ) );
			return true;
		}
		else if( type == name_regex )
		{
			data.operands().push( Node( Attribute::newExplicit( regex( data.parser().current().str() ), name_regex ),
				data.parser().current().source() ) );
			return true;
		}
		else if( type == name_namepath )
		{
			data.operands().push( Node( Attribute::newExplicit( namepath( data.parser().current().str().slice( 1, -1 ) ),
				name_namepath ), data.parser().current().source() ) );
			return true;
		}
		else if( type == name_path )
		{
			data.operands().push( Node( Attribute::newExplicit( path( data.parser().current().str() ), name_path ),
				data.parser().current().source() ) );
			return true;
		}
		return false;
	}

	bool Expression::_parseOperator( ParseData& data )
	{
		auto type = data.parser().current().type();
		if( type == name_operator )
			return _processOperator( data );
		else if( type::Operators::isOperator( type ) )
		{
			if( compilerName( data.parser().current().str() ) == symbol_minus && data.lastSeen() == Seen::_operator )
				type = name_unary_minus;
			return _processOperator( data, type, data.parser().current().source() );
		}
		else if( type == symbol_semicolon )
			return _processOperator( data, type, data.parser().current().source() );

		return false;
	}
	bool Expression::_processOperator( ParseData& data )
	{
		// This is one of: +*=~&|^<>[]
		name_t op_name = compilerName( data.parser().current().str() );
		return _processOperator( data, op_name, data.parser().current().source() );
	}
	bool Expression::_processOperator( ParseData& data, name_t op_name, source::Ref op_source )
	{
		if( op_name == symbol_semicolon )
		{
			while( !data.operators().empty() )
				_bindOperator( data );
			data.lastSeenOperator();
			return true;
		}
		else if( op_name == symbol_close_round )
			return _processClose( data );
		else
			return _processPlainOperator( data, op_name, op_source );
		return false;
	}
	bool Expression::_processClose( ParseData& data )
	{
		while( !data.operators().empty() && data.operators().top().Op != symbol_open_round )
			_bindOperator( data );
		if( !data.operators().empty() )
			data.operators().pop();
		else
		{
			data.error();
			data.reporter().error( "Unbalanced parenthesis!", data.parser().current().source() );
		}
		data.lastSeenOperator();
		return true;
	}
	bool Expression::_processPlainOperator( ParseData& data, name_t new_op_name, source::Ref new_op_source )
	{
		while( !data.operators().empty() && type::Operators::inputPrecedence( new_op_name )
			<= type::Operators::stackPrecedence( data.operators().top().Op ) )
		{
			if( new_op_name == symbol_open_round )
			{
				data.error();
				data.reporter().error( "Unbalanced parenthesis!", data.parser().current().source() );
			}
			else
				_bindOperator( data );
		}
		if( type::Operators::isFirstInSequence( new_op_name ) )
		{
			// If this is a sequence operator, then parse whatever is inbetween as if parenthesis
			name_t op_name = _resolveOperator( data, new_op_name );
			if( op_name != no_name )
				data.operators().push( Operator( op_name, new_op_source ) );
		}
		data.lastSeenOperator();
		return true;
	}
	bool Expression::_processNamedOperator( ParseData& data, name_t op_name )
	{
		auto& seq = type::Operators::sequence( op_name );
		// TODO: Finish!
		return false;
	}
	void Expression::_bindOperator( ParseData& data )
	{
		Node op( data.operators().top().Op, data.operators().top().Source );
		for( size_t i = 0; i < type::Operators::numOperands( op.opName() ); ++i )
		{
			if( data.operands().empty() )
			{
				data.error();
				data.reporter().error( "Operator is missing operand!", op.source() );
				break;
			}
			op.addOperand( std::move( data.operands().top() ) );
			data.operands().pop();
		}
		if( !op.findOperator( *Scope, data.reporter() ) )
			data.error();
		data.operands().push( std::move( op ) );
		data.operators().pop();
	}
	name_t Expression::_resolveOperator( ParseData& data, name_t op_name )
	{
		std::vector<name_t> all_options = type::Operators::mapOperator( op_name );
		if( all_options.empty() )
		{
			data.reporter().error( "Internal issue with this operator!", data.parser().current().source() );
			throw InvalidExpression();	// Impossible!?
		}

		// A single option without sequence (most operators) need no special resloving.
		if( all_options.size() == 1 && type::Operators::sequence( all_options[ 0 ] ).size() == 1 )
			return all_options[ 0 ];

		// We have a single option with a sequence or multiple options (which means all have sequences)!

		std::vector<Node> operands;
		for( index_t seq_no = 1; ; ++seq_no )
		{
			// If we have an option with a following prefix operator in the sequence (meaning no operand inbetween),
			// and the next token is that operator, we know that option must be the one!
			name_t solid_op_name = _findSolidOpName( data, all_options, op_name, seq_no );
			if( solid_op_name != no_name )
				return _processSequenceOperator( data, solid_op_name, std::move( operands ) );

			// We can now discard any option where the next operator in squence is a prefix operator
			// (since we have found that it didn't match the next token).
			std::vector<name_t> options = _discardPrefixOperatorOptions( all_options, seq_no );

			// If we are left with none, then we have a syntax error
			if( options.empty() )
			{
				data.error();
				data.reporter().error( "This operator requires an additional element!", data.parser().current().source() );
				for( auto& option : all_options )
				{
					auto& sequence = type::Operators::sequence( option );
					if( seq_no < sequence.size() )
					{
						string lead = option == all_options[ 0 ] ? "  Perhaps" : "  or perhaps";
						data.reporter().note( lead + " this one? : " + eon::str( sequence[ seq_no ].Op ), source::Ref() );
					}
				}
				return no_name;
			}

			// If we have only one option left, we can continue with that.
			if( options.size() == 1 )
				return _processSequenceOperator( data, options[ 0 ], std::move( operands ) );

			// We still have multiple options left and must parse the next operand before we can get to the next operator name
			source::Ref orig_source = data.parser().current().source();
			data.parser().forward();	// Must move past the operator!
			ParseData data2( data, _findStopOn( options, seq_no ) );
			if( _parse( data2 ) )
				operands.push_back( std::move( data2.operands().top() ) );

			// If at the end of the tokens, we have a syntax error
			if( !data.parser() )
			{
				data.error();
				data.reporter().error( "Incomplete operator!", orig_source );
				return no_name;
			}

			// Eliminate options for which the next token does not match the next operator in sequence
			all_options = _matchNextSequenceOperator( data, options, seq_no );
		}
	}
	name_t Expression::_findSolidOpName( ParseData& data, const std::vector<name_t>& options, name_t op_name, index_t seq_no )
	{
		for( auto& option : options )
		{
			auto& sequence = type::Operators::sequence( option );
			if( seq_no >= sequence.size() )
				return option;	// All operators in the sequence have been matched!
			if( !sequence[ seq_no ].Prefix )
				continue;

			auto next_type = data.parser().ahead().type();
			if( next_type == sequence[ seq_no ].Op
				|| ( next_type == name_name && name( data.parser().ahead().str() ) == sequence[ seq_no ].Op )
				|| ( next_type == name_operator && compilerName( data.parser().ahead().str() ) == sequence[ 1 ].Op ) )
			{
				data.parser().forward();
				return option;
			}
		}
		return no_name;
	}
	std::vector<name_t> Expression::_discardPrefixOperatorOptions( const std::vector<name_t>& options, index_t seq_no )
	{
		std::vector<name_t> remaining;
		for( auto& option : options )
		{
			auto& sequence = type::Operators::sequence( option );
			if( seq_no < sequence.size() && !sequence[ seq_no ].Prefix )
				remaining.push_back( option );
		}
		return remaining;
	}
	std::set<string> Expression::_findStopOn( const std::vector<name_t>& options, index_t seq_no )
	{
		std::set<string> stop_on;
		for( auto& option : options )
			stop_on.insert( eon::str( type::Operators::sequence( option )[ seq_no ].Op ) );
		return stop_on;
	}
	std::vector<name_t> Expression::_matchNextSequenceOperator( ParseData& data, const std::vector<name_t>& options, index_t seq_no )
	{
		std::vector<name_t> remaining;
		for( auto& option : options )
		{
			auto& sequence = type::Operators::sequence( option );
			if( seq_no >= sequence.size() )	// Not possible!?
				continue;
			auto next_type = data.parser().current().type();
			if( next_type == sequence[ seq_no ].Op
				|| ( next_type == name_name && name( data.parser().current().str() ) == sequence[ seq_no ].Op )
				|| ( next_type == name_operator && compilerName( data.parser().current().str() ) == sequence[ seq_no ].Op ) )
				remaining.push_back( option );
		}
		return remaining;
	}
	name_t Expression::_processSequenceOperator( ParseData& data, name_t op_name, std::vector<Node>&& operands )
	{
		// Get remaining operands - if any
		auto& sequence = type::Operators::sequence( op_name );
		for( index_t seq_no = operands.size() + 1; seq_no < sequence.size(); ++seq_no )
		{
			data.parser().forward();	// Must move past the operator!
			ParseData data2( data, { eon::str( sequence[ seq_no ].Op ) } );
			if( _parse( data2 ) )
				operands.push_back( std::move( data2.operands().top() ) );
			else
			{
				data.error();
				return no_name;
			}
		}

		// Push all that we have and we're done!
		for( auto& operand : operands )
			data.operands().push( std::move( operand ) );
		return op_name;
	}

	bool Expression::_parseTuple( ParseData& data )
	{
		auto type = data.parser().current().type();
		if( type == name_plain )
			return _parseTuple( data, type );
		else if( type == name_typetuple )
			return _parseTypeTuple( data );
		else if( type == name_dynamic || type == name_data )
		{
			data.error();
			data.reporter().error( "Cannot declare " + eon::str( type ) + " tuple inside expression! (Only plain tuple!)",
				data.parser().current().source() );
			data.parser().forward();
		}
		return false;
	}
	bool Expression::_parseTuple( ParseData& data, name_t tupletype )
	{
		auto source = data.parser().current().source();
		data.parser().forward();	// Skip tuple identifier

		std::vector<AttributePair> attributes;

		// Each attribute is an expression we must evaluate
		// Read attributes until we see a ')'!
		ParseData data2( data, { ",", ")" } );
		while( data2.parser() && !data2.parser().current().is( symbol_close_round ) )
		{
			// Is the attribute named?
			name_t attrib_name = no_name;
			if( data2.parser().current().is( name_name )
				&& data2.parser().exists() && data2.parser().ahead().type() == name_assign )
			{
				attrib_name = eon::name( data2.parser().current().str() );
				data2.parser().forward( 2 );
			}

			if( _parse( data2 ) )
			{
				attributes.push_back( AttributePair( attrib_name, std::move( data2.operands().top().value() ) ) );
				data2.operands().pop();
			}

			if( data.parser().current().is( symbol_comma ) )
				data.parser().forward();
		}
		if( !data.parser() )
		{
			data.error();
			data.reporter().error( "Tuple is not properly ended!", source );
			return false;
		}

		Tuple tuple( tupletype, std::move( attributes ) );
		data.operands().push( Node( Attribute::newTuple( std::move( tuple ) ), source ) );
		return true;
	}
	bool Expression::_parseTypeTuple( ParseData& data )
	{
		auto source = data.parser().current().source();
		auto ttuple = _processTypeTuple( data );
		if( !ttuple )
			return false;
		data.operands().push( Node( Attribute::newTypeTuple( std::move( ttuple ) ), source ) );
		return true;
	}
	TypeTuple Expression::_processTypeTuple( ParseData& data )
	{
		auto source = data.parser().current().source();
		data.parser().forward();	// Skip tuple identifier

		// A type tuple consists of attributes that are either names or parenthesized sub-tuples, and can be either
		// unnamed or named.
		TypeTuple ttuple;
		while( data.parser() && !data.parser().current().is( symbol_close_round ) )
		{
			// Is the attribute named?
			name_t attrib_name = no_name;
			if( data.parser().current().is( name_name )
				&& data.parser().exists() && data.parser().ahead().type() == name_assign )
			{
				attrib_name = eon::name( data.parser().current().str() );
				data.parser().forward( 2 );
			}

			// Do we have a sub-tuple value?
			if( data.parser().current().is( symbol_open_round ) )
			{
				auto sub = _processTypeTuple( data );
				if( sub )
				{
					ttuple.add( attrib_name, std::move( sub ) );
					data.parser().forward();
				}
			}

			// Must be a name
			else if( data.parser().current().is( name_name ) )
			{
				ttuple.add( attrib_name, eon::name( data.parser().current().str() ) );
				data.parser().forward();
			}

			else
			{
				data.error();
				data.reporter().error( "Invalid type tuple attribute!", data.parser().current().source() );
				data.parser().forward();
			}

			if( !data.parser() )
				break;
			if( data.parser().current().is( symbol_comma ) )
				data.parser().forward();
			else if( !data.parser().current().is( symbol_close_round ) )
			{
				data.error();
				data.reporter().error( "Expected comma or ')' here!", data.parser().current().source() );
			}
		}

		if( !data.parser() )
		{
			data.error();
			data.reporter().error( "Type tuple is not properly ended!", source );
			return TypeTuple();
		}

		return ttuple;
	}

	bool Expression::_parseName( ParseData& data )
	{
		if( data.parser().current().type() == name_literal )
		{
			data.operands().push( Node( Attribute::newExplicit( eon::name( data.parser().current().str().slice( 1, -1 ) ),
				name_name ), data.parser().current().source() ) );
			return true;
		}

		if( data.parser().current().type() != name_name )
			return false;

		// NOTE: The name can be: a variable, a name value, an operator or part of an operator, or an action call.
		
		name_t name = eon::name( data.parser().current().str() );
		if( type::Operators::isOperator( name ) )
			return _processOperator( data, name, data.parser().current().source() );

		// If the name is followed by a '(' it must be an action call!
		if( data.parser().exists() && data.parser().ahead().is( symbol_open_round ) )
			return _parseActionCall( data, name );

		// Can be a variable
		for( Tuple* scop = Scope; scop != nullptr; scop = scop->parent() )
		{
			if( scop->exists( name ) )
			{
				data.operands().push( Node( Attribute( scop->attribute( name ) ), source::Ref() ) );
				return true;
			}
		}

		// Assuming name value!
		data.operands().push( Node( Attribute::newExplicit<name_t>( name, name_name ), data.parser().current().source() ) );
		return true;
	}

	bool Expression::_parseActionCall( ParseData& data, name_t action_name )
	{
		auto source = data.parser().current().source();
		data.parser().forward( 2 );		// Skip name + '('

		std::vector<Node> args;
		TypeTuple argtypes;

		// Each argument is an expression we must evaluate.
		// Read arguments until we see a ')'!
		ParseData data2( data, { ",", ")" } );
		while( data2.parser() && !data2.parser().current().is( symbol_close_round ) )
		{
			// Is the argument named?
			name_t arg_name = no_name;
			if( data2.parser().current().is( name_name )
				&& data2.parser().exists() && data2.parser().ahead().type() == name_assign )
			{
				arg_name = eon::name( data2.parser().current().str() );
				data2.parser().forward( 2 );
			}

			if( _parse( data2 ) )
			{
				args.push_back( std::move( data2.operands().top() ) );
				data2.operands().pop();
				if( args[ args.size() - 1 ].isOperator() )
					argtypes.add( arg_name, args[ args.size() - 1 ].opReturnType() );
				else
					argtypes.add( arg_name, args[ args.size() - 1 ].value().type() );
			}

			if( data.parser().current().is( symbol_comma ) )
				data.parser().forward();
		}
		if( !data.parser() )
		{
			data.error();
			data.reporter().error( "Action call is not properly ended!", source );
			return false;
		}

		// If the action name is a type, we have a constructor
		if( Scope->isType( action_name ) )
		{
			auto actions = Scope->signatures( name_constructor, action_name, argtypes );
			if( actions.size() == 1 )
			{
				// We have a match.
				Node call( *Scope->action( *actions.begin() ), source );
				for( auto& arg : args )
					call.addOperand( std::move( arg ) );
				data.operands().push( std::move( call ) );
			}
		}
		else
		{
			// TODO: Add support for other action types!
			return false;
		}
		return true;
	}

	bool Expression::_parseExpression( ParseData& data )
	{
		if( !data.parser().current().is( name_expression ) )
			return false;

		auto source = data.parser().current().source();
		data.parser().forward();	// Skip 'e('
		try
		{
			Expression expr( data.parser(), *Scope, data.reporter(), { ")" } );
			data.operands().push( Node( Attribute::newExplicit( std::move( expr ), name_expression ), source ) );
			return true;
		}
		catch( ... )
		{
			return false;
		}
	}
}
