#include "EdfParser.h"
#include "ExpressionParser.h"
#include "TypeTupleParser.h"


namespace eon
{
	namespace parser
	{
#ifdef EON_TEST_MODE
		struct TestData
		{
			TestData( string&& source )
			{
				TestState::resetGlobal();
				Report.defaultTargets();
				State = parser::State( std::move( source ), Report );
				Data = std::make_shared<EdfParser::ParserData>( State, scope::global() );
			}
			parser::State State;
			source::Reporter Report;
			std::shared_ptr<EdfParser::ParserData> Data;
		};
#endif


		expression::Node EdfParser::parse( State& state, Tuple& scope )
		{
			ParserData data( state, scope );
			_parse( data, NewlinePolicy::ignore );
			if( !data.hasErrors() )
				return expression::Node::newValue(
					Attribute::newTuple( data.root(), type::Qualifier::_rvalue, data.source() ) );
			else
				return expression::Node();
		}

		Tuple EdfParser::parseRaw( State& state, Tuple& scope )
		{
			ParserData data( state, scope );
			_parse( data, NewlinePolicy::ignore );
			if( !data.hasErrors() )
				return data.root();
			else
				return tuple::newData();
		}




		EdfParser::ParserData::ParserData( parser::State& state, Tuple& scope )
		{
			State = &state;
			Scope = &scope;
			if( !tokens().atEnd() && tokens().viewed().is( name_indentation ) )
				Indentations.push( Indentation::normal( tokens().viewed().str().numChars() ) );
			else
				Indentations.push( Indentation::normal( 0 ) );
		}

		bool EdfParser::ParserData::atIncreasedIndentation( index_t offset ) const noexcept
		{
			if( Indentations.empty() )
				return false;
			if( !State->Tokens.exists( static_cast<int>( offset ) ) )
				return false;
			auto& viewed = State->Tokens.peek( State->Tokens.viewedPos() + offset );
			if( !viewed.is( name_indentation ) )
				return false;
			auto& indent = Indentations.top();
			if( indent.Forced )
				return viewed.source().numChars() >= indent.Value;
			else
				return viewed.source().numChars() > indent.Value;
		}

		bool EdfParser::ParserData::atReducedIndentation( index_t offset ) const noexcept
		{
			return !Indentations.empty() && State->Tokens.exists( static_cast<int>( offset ) )
				&& State->Tokens.peek( State->Tokens.viewedPos() + offset ).is( name_indentation )
				&& State->Tokens.peek( State->Tokens.viewedPos() + offset ).str().numChars() < Indentations.top().Value;
		}

		bool EdfParser::ParserData::haveSequence( IndentationMatch match_indentation ) const noexcept
		{
			auto type = State->Tokens.viewed().type();
			int next{ 1 };
			if( !State->Tokens.exists( next ) )
				return false;
			if( State->Tokens.peekAhead( next ).is( type ) )
				return true;
			if( State->Tokens.peekAhead( next ).is( name_newline ) )
			{
				++next;
				if( State->Tokens.peekAhead( next ).is( name_indentation ) )
				{
					index_t indentation = State->Tokens.peekAhead( next ).source().numChars();
					if( match_indentation == IndentationMatch::same_or_greater && indentation < Indentations.top().Value )
						return false;
					else if( match_indentation == IndentationMatch::indented && indentation <= Indentations.top().Value )
						return false;
					++next;
				}
				else
				{
					if( Indentations.top().Value == 0 )
					{
						if( match_indentation != IndentationMatch::same_or_greater )
							return false;
					}
				}
				return State->Tokens.exists( next ) && State->Tokens.peekAhead( next ).is( type );
			}
			return false;
		}

		void EdfParser::ParserData::recordAttributeName()
		{
			AttributeName = eon::name( tokens().viewed().str() );
			NameSource = tokens().viewed().source();
			tokens().forward();
		}

		void EdfParser::ParserData::recordNameValueSeparator()
		{
			NameSepSource = tokens().viewed().source();
			tokens().forward();
		}


		bool EdfParser::ParserData::ensureNewLine()
		{
			if( State->Tokens.viewed().is( name_newline ) )
				return true;
			error( "Expected tuple attributes to follow on indented line below!" );
			return false;
		}

		void EdfParser::ParserData::error( eon::string&& message, source::Ref* source )
		{
			Errors = true;
			source::Ref src = source != nullptr ? *source
				: !State->Tokens.atEnd() ? State->Tokens.viewed().source() : State->Tokens.last().source();
			State->Report->error( std::move( message ), src );
		}


		void EdfParser::_parse( ParserData& data, NewlinePolicy policy )
		{
			while( !data.atEndOfBlock() && (
				policy == NewlinePolicy::ignore || !data.tokens().viewed().is( name_newline ) ) )
				_parseAttribute( data );
		}


		void EdfParser::_parseAttribute( ParserData& data )
		{
			// Attributes can be one of the following:
			// 1. Singleton (including parenthesized subtuples), possibly followed by comma and more singletons.
			// 2. Dash + singleton, one per line of same indentation.
			// 3. Named attributes, one per line of same indentation.

			if( data.tokens().viewed().is( symbol_minus ) )
				_parseDashAttribute( data );
			else if( data.isNamedAttribute() )
				_parseNamedAttribute( data );
			else
				_parseUnnamedAttributes( data );
		}

		void EdfParser::_parseDashAttribute( ParserData& data )
		{
			data.tokens().forward();	// Skip '-'

			// If this is a named item, we have a sub-tuple and not a plain value
			if( data.isNamedAttribute() )
			{
				data.addSubtuple();
				data.pushLastSubtuple();
				data.forceIndent();
				while( !data.tokens().atEnd() )
				{
					_parse( data, NewlinePolicy::stop );
					if( data.tokens().atEnd() )
						break;
					if( !data.tokens().viewed().is( name_newline ) )
					{
						data.error( "Expected a newline here!" );
						return;
					}

					if( data.tokens().exists() && !data.tokens().peekAhead().is( name_indentation ) )
					{
						data.tokens().forward();
						break;
					}
					if( data.atReducedIndentation( 1 ) )
						break;
					if( !data.tokens().forward( 2 ) )	// Skip newline and indentation
						break;
				}
				data.unindent();
				data.popSubtuple();
			}
			else
				_parseValue( data );
		}

		void EdfParser::_parseNamedAttribute( ParserData& data )
		{
			data.recordAttributeName();

			// Named attributes will either have '=' followed by singleton value (including parenthesized sub-tuple)
			// or ':' followed by newline before values, which must occur indented one per line.
			if( data.tokens().viewed().is( symbol_assign ) )
				_parseNamedSingleton( data );
			else
				_parseNamedSubtuple( data );
		}

		void EdfParser::_parseNamedSingleton( ParserData& data )
		{
			data.recordNameValueSeparator();
			if( data.tokens().atEnd() )
			{
				data.reportMissingSingletonValue();
				return;
			}

			_skipLegalNamedSingletonWhitespaces( data );
			if( data.tokens().atEnd() )
				return;

			bool indented = data.tokens().viewed().is( name_indentation );
			if( indented )
				data.indent();
			_parseValue( data );
			if( indented )
				data.unindent();
		}
		EON_TEST_2STEP( EdfParser, parse, named_simple,
			TestState state( string( "one=true" ) ),
			EON_EQ( "data(one=true)", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_simple_twoline,
			TestState state( string( "one=\n  true" ) ),
			EON_EQ( "data(one=true)", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_simple_illegal1,
			TestState state( string( "one=" ) ),
			EON_FALSE( EdfParser().parse( state, scope::global() ).value() ) );
		EON_TEST_2STEP( EdfParser, parse, named_simple_illegal2,
			TestState state( string( "one=\ntrue" ) ),
			EON_FALSE( EdfParser().parse( state, scope::global() ).value() ) );
		EON_TEST_2STEP( EdfParser, parse, named_simple_illegal3,
			TestState state( string( "    one=\n  true" ) ),
			EON_FALSE( EdfParser().parse( state, scope::global() ).value() ) );
		EON_TEST_2STEP( EdfParser, parse, named_simple_illegal4,
			TestState state( string( "one=\n\n  true" ) ),
			EON_FALSE( EdfParser().parse( state, scope::global() ).value() ) );

		void EdfParser::_skipLegalNamedSingletonWhitespaces( ParserData& data )
		{
			// Beyond the '=' of a named singleton attribute we can have the following:
			// 1. <value>
			// 2. <newline> + <indentation> + <value>
			// Anything else if an error!
			if( data.tokens().viewed().is( name_newline ) )
			{
				if( !data.atIncreasedIndentation( 1 ) )
				{
					data.error( "Named singleton attribute value must follow on the same line as the name "
						"or on the next line with indentation!" );
					return;
				}
				data.tokens().forward();	// Skip the newline
			}
		}

		void EdfParser::_parseNamedSubtuple( ParserData& data )
		{
			data.recordNameValueSeparator();
			data.addSubtuple();
			if( data.tokens().atEnd() )	// Legal, empty tuple attribute!
				return;

			data.pushLastSubtuple();
			while( !data.tokens().atEnd() )
			{
				if( !data.ensureNewLine() )				// Must be on a separate line!
					return;
				if( !data.atIncreasedIndentation( 1 ) )	// Must be indented
					break;
				data.tokens().forward();				// Skip newline

				if( !_parseSubtupleValue( data ) )
					return;
			}
			data.popSubtuple();
		}
		EON_TEST_2STEP( EdfParser, parse, named_tuple_empty,
			TestState state( string( "one:" ) ),
			EON_EQ( "data(one=())", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_tuple_list_single,
			TestState state( string( "one:\n  - true" ) ),
			EON_EQ( "data(one=(true))",
				EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_tuple_list_multi,
			TestState state( string( "one:\n  - true\n  - false" ) ),
			EON_EQ( "data(one=(true, false))",
				EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_tuple_list_subtuple,
			TestState state( string( "one:\n  - a=1\n    b=2\n  - A=10\n    B=11\n  - x=0" ) ),
			EON_EQ( "data(one=((a=1, b=2), (A=10, B=11), (x=0)))",
				EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_tuple_subtuple1,
			TestState state( string( "one:\n  two=true\n  three=false" ) ),
			EON_EQ( "data(one=(two=true, three=false))",
				EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_tuple_subtuple2,
			TestState state( string( "one:\n  two:\n    true\n  three:\n    false" ) ),
			EON_EQ( "data(one=(two=(true), three=(false)))",
				EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_tuple_x2,
			TestState state( string( "one:\n  alpha\ntwo:\n  beta" ) ),
			EON_EQ( "data(one=(alpha), two=(beta))",
				EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_tuple_x2_subtuples,
			TestState state( string( "s1:\n  s11:\n    v11\n  s12:\n    v12\ns2:\n  s21:\n    v21\n  s22:\n    v22" ) ),
			EON_EQ( "data(s1=(s11=(v11), s12=(v12)), s2=(s21=(v21), s22=(v22)))",
				EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, named_tuple_plaintuples,
			TestState state( string( "one:\n  alpha=(x, y)\n  beta=(1, 2)" ) ),
			EON_EQ( "data(one=(alpha=(x, y), beta=(1, 2)))",
				EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		bool EdfParser::_parseSubtupleValue( ParserData& data )
		{
			data.indent();
			_parseAttribute( data );
			data.unindent();
			return true;
		}


		void EdfParser::_parseUnnamedAttributes( ParserData& data )
		{
			while( true )
			{
				_parseValue( data );
				if( data.tokens().atEnd() || !data.tokens().viewed().is( name_comma ) )
					break;
				data.tokens().forward();	// Skip comma
			}
		}

		void EdfParser::_parseValue( ParserData& data )
		{
			auto type = data.tokens().viewed().type();
			if( type == name_name )
				_parseNameValue( data );
			else if( type == name_bool )
				_parseBoolValue( data );
			else if( type == name_byte )
				_parseByteValue( data );
			else if( type == name_char )
				_parseCharValue( data );
			else if( type == name_int || type == name_digits )
				_parseIntValue( data );
			else if( type == name_float )
				_parseFloatValue( data );
			else if( type == name_bytes )
				_parseBytesValue( data );
			else if( type == name_string )
				_parseStringValue( data );
			else if( type == name_regex )
				_parseRegexValue( data );
			else if( type == name_namepath )
				_parseNamepathValue( data );
			else if( type == name_path )
				_parsePathValue( data );
			else if( type == name_expression )
				_parseExpressionValue( data );
			else if( type == name_typetuple )
				_parseTypeTuple( data );
			else if( type == symbol_open_round )
				_parseSubtuple( data );

			data.tokens().forward();
			data.clearAttributeName();
		}

		EON_TEST_2STEP( EdfParser, parse, plain_bool,
			TestState state( string( "true" ) ),
			EON_EQ( "data(true)", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, plain_byte,
			TestState state( string( "B'x'" ) ),
			EON_EQ( "data(B'x')", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, plain_char,
			TestState state( string( "'x'" ) ),
			EON_EQ( "data('x')", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		void EdfParser::_parseIntValue( ParserData& data )
		{
			const long_t value = data.tokens().viewed().str().toLongT();
			if( value <= EON_INT_MAX )
				data.tuple().add( data.attributeName(), static_cast<int_t>( value ) );
			else
				data.tuple().add( data.attributeName(), value );
		}
		EON_TEST_2STEP( EdfParser, parse, plain_int,
			TestState state( string( "450" ) ),
			EON_EQ( "data(450)", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
		EON_TEST_2STEP( EdfParser, parse, plain_long,
			TestState state( string( "99999999999" ) ),
			EON_EQ( "data(99999999999)", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		void EdfParser::_parseFloatValue( ParserData& data )
		{
			const high_t value = data.tokens().viewed().str().toHighT();
			if( value <= EON_FLOAT_MAX )
				data.tuple().add( data.attributeName(), static_cast<flt_t>( value ) );
			else
				data.tuple().add( data.attributeName(), value );
		}
		EON_TEST_2STEP( EdfParser, parse, plain_float,
			TestState state( string( "98.76" ) ),
			EON_EQ( "data(98.76)", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		void EdfParser::_parseNameValue( ParserData& data )
		{
			if( data.attributeName() == no_name )
				data.tuple().addName( eon::name( data.tokens().viewed().str() ) );
			else
				data.tuple().add( data.attributeName(), eon::name( data.tokens().viewed().str() ) );
		}
		EON_TEST_2STEP( EdfParser, parse, plain_name,
			TestState state( string( "hello" ) ),
			EON_EQ( "data(hello)", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		void EdfParser::_parseBytesValue( ParserData& data )
		{
			auto value = data.tokens().viewed().str().stdstr();
			if( data.haveSequence( IndentationMatch::indented ) )
			{
				do
				{
					for( data.tokens().forward(); !data.tokens().viewed().is( name_bytes ); data.tokens().forward() )
						;
					value += data.tokens().viewed().str().stdstr();
				} while( data.haveSequence( IndentationMatch::same_or_greater ) );
			}
			data.tuple().add( data.attributeName(), std::move( value ) );
		}

		EON_TEST_2STEP( EdfParser, parse, plain_bytes,
			TestState state( string( "B\"something\"" ) ),
			EON_EQ( "data(B\"something\")", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		void EdfParser::_parseStringValue( ParserData& data )
		{
			auto value = data.tokens().viewed().str();
			if( data.haveSequence( IndentationMatch::indented ) )
			{
				do
				{
					for( data.tokens().forward(); !data.tokens().viewed().is( name_string ); data.tokens().forward() )
						;
					value += data.tokens().viewed().str();
				} while( data.haveSequence( IndentationMatch::same_or_greater ) );
			}
			data.tuple().add( data.attributeName(), std::move( value ) );
		}

		EON_TEST_2STEP( EdfParser, parse, plain_string,
			TestState state( string( "\"nothing\"" ) ),
			EON_EQ( "data(\"nothing\")", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		EON_TEST_2STEP( EdfParser, parse, plain_namepath,
			TestState state( string( "@one/two/three" ) ),
			EON_EQ( "data(@one/two/three)", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		EON_TEST_2STEP( EdfParser, parse, plain_path,
			TestState state( string( "p\"one/two/three\"" ) ),
			EON_EQ( "data(p\"one/two/three\")", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		void EdfParser::_parseExpressionValue( ParserData& data )
		{
			static ExpressionParser parser;
			data.tuple().add( data.attributeName(), parser.parse( data.state(), data.scope(), { ")" } ) );
		}
		EON_TEST_2STEP( EdfParser, parse, plain_expression,
			TestState state( string( "ex(1  +2)" ) ),
			EON_EQ( "data(ex(1 + 2))", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		void EdfParser::_parseTypeTuple( ParserData& data )
		{
			static TypeTupleParser parser;
			data.tuple().add( data.attributeName(), parser.parseRaw( data.state(), data.scope() ) );
		}
		EON_TEST_2STEP( EdfParser, parse, plain_typetuple,
			TestState state( string( "T(int)" ) ),
			EON_EQ( "data(T(int))", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );

		void EdfParser::_parseSubtuple( ParserData& data )
		{
			source::Ref source = data.tokens().viewed().source();
			data.tokens().forward();	// Skip leading '('
			data.addSubtuple();
			data.pushLastSubtuple();
			while( !data.tokens().atEnd() )
			{
				if( data.tokens().viewed().is( symbol_close_round ) )
				{
					data.popSubtuple();
					return;
				}

				if( data.isNamedAttribute() )
				{
					data.error( "Cannot have named attributes inside here!" );
					return;
				}

				_parseValue( data );

				// Must have ',' or ')' here!
				if( !data.tokens().atEnd() )
				{
					if( data.tokens().viewed().is( symbol_comma ) )
					{
						data.tokens().forward();
						continue;
					}
					else if( !data.tokens().viewed().is( symbol_close_round ) )
					{
						data.error( "Expected ',' or ')' here!" );
						return;
					}
				}
			}
			data.popSubtuple();
			data.error( "Expected a ')' to complement '(' at " + source.str() + "!" );
		}
		EON_TEST_2STEP( EdfParser, parse, plain_subtuple,
			TestState state( string( "(one, two)" ) ),
			EON_EQ( "data((one, two))", EdfParser().parse( state, scope::global() ).value().value<Tuple>().str() ) );
	}
}