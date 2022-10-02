#pragma once

#include "Action.h"
#include "TupleFactory.h"



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
		template<typename T>
		struct FloatConstruct : public Action {
			inline FloatConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ) ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( Attribute::newImplicit( static_cast<T>( 0 ) ) ); return sig_t::norm; } };
		 
		template<typename T>
		struct FloatCopyConstruct : public Action {
			inline FloatCopyConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, type::mapCppType(
					CPPTYPE( T ) ) } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				const auto a1 = values.top().value<T>(); values.pop(); values.push( Attribute::newImplicit( a1 ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatBoolConstruct : public Action {
			inline FloatBoolConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_bool } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<bool>(); values.pop(); values.push(
					Attribute::newImplicit( static_cast<T>( a1 ? 1.0 : 0.0 ) ) ); return sig_t::norm; } };

		template<typename T>
		struct FloatByteConstruct : public Action {
			inline FloatByteConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_byte } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<byte_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatCharConstruct : public Action {
			inline FloatCharConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_char } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<char_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatIntConstruct : public Action {
			inline FloatIntConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_int } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<int_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatShortConstruct : public Action {
			inline FloatShortConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_short } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<short_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatLongConstruct : public Action {
			inline FloatLongConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_long } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<long_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; } };

		struct FloatLowConstruct : public Action {
			inline FloatLowConstruct() : Action( TypeTuple::action( name_float, name_constructor, name_operator,
				name_float, { { no_name, name_low } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<low_t>(); values.pop(); values.push(
					Attribute::newExplicit( (flt_t)a1, name_float ) ); return sig_t::norm; } };
		struct FloatHighConstruct : public Action {
			inline FloatHighConstruct() : Action( TypeTuple::action( name_float, name_constructor, name_operator,
				name_float, { { no_name, name_high } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<high_t>(); values.pop(); values.push(
					Attribute::newExplicit( (flt_t)a1, name_float ) ); return sig_t::norm; } };
		struct LowFloatConstruct : public Action {
			inline LowFloatConstruct() : Action( TypeTuple::action( name_low, name_constructor, name_operator, name_low, {
				{ no_name, name_float } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push(
					Attribute::newExplicit( (low_t)a1, name_low ) ); return sig_t::norm; } };
		struct LowHighConstruct : public Action {
			inline LowHighConstruct() : Action( TypeTuple::action( name_low, name_constructor, name_operator, name_low, {
				{ no_name, name_high } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<high_t>(); values.pop(); values.push(
					Attribute::newExplicit( (low_t)a1, name_low ) ); return sig_t::norm; } };
		struct HighFloatConstruct : public Action {
			inline HighFloatConstruct() : Action( TypeTuple::action( name_high, name_constructor, name_operator, name_high,
				{ { no_name, name_float } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push(
					Attribute::newExplicit( (high_t)a1, name_high ) ); return sig_t::norm; } };
		struct HighLowConstruct : public Action {
			inline HighLowConstruct() : Action( TypeTuple::action( name_high, name_constructor, name_operator, name_high,
				{ { no_name, name_low } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<low_t>(); values.pop(); values.push(
					Attribute::newExplicit( (high_t)a1, name_high ) ); return sig_t::norm; } };

		template<typename T>
		struct FloatIndexConstruct : public Action {
			inline FloatIndexConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_index } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<index_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatB8Construct : public Action {
			inline FloatB8Construct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ),
				{ { no_name, name_b8 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b8_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; }
		};
		template<typename T>
		struct FloatB16Construct : public Action {
			inline FloatB16Construct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ),
				{ { no_name, name_b16 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b16_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; }
		};
		template<typename T>
		struct FloatB32Construct : public Action {
			inline FloatB32Construct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ),
				{ { no_name, name_b32 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b32_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; }
		};
		template<typename T>
		struct FloatB64Construct : public Action {
			inline FloatB64Construct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ),
				{ { no_name, name_b64 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b64_t>(); values.pop(); values.push( Attribute::newImplicit( (T)a1 ) );
				return sig_t::norm; }
		};

		template<typename T>
		struct FloatCompare : public Action {
			inline FloatCompare() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_cmp, name_operator, name_int, { { no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( Attribute::newImplicit( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) ) );
				return sig_t::norm; } };
		

		template<typename T>
		struct FloatUnaryMinus : public Action {
			inline FloatUnaryMinus() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_unary_minus, name_operator, type::mapCppType( CPPTYPE( T ) ), {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<T>(); values.pop(); values.push( Attribute::newImplicit( -a1 ) );
				return sig_t::norm; } };


		template<typename T>
		struct FloatPlusAssign : public Action {
			inline FloatPlusAssign() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_plus_assign, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto& a1 = values.top();
				a1.value<T>() += a2;
				return sig_t::norm; } };

		template<typename T>
		struct FloatMinusAssign : public Action {
			inline FloatMinusAssign() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_minus_assign, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto& a1 = values.top();
				a1.value<T>() -= a2;
				return sig_t::norm; } };

		template<typename T>
		struct FloatMultiplyAssign : public Action {
			inline FloatMultiplyAssign() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_multiply_assign, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto& a1 = values.top();
				a1.value<T>() *= a2;
				return sig_t::norm; } };

		template<typename T>
		struct FloatDivideAssign : public Action {
			inline FloatDivideAssign() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_divide_assign, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				if( a2 == 0.0 )
				{
					values.push( Attribute::newExplicit( name( "division_by_zero" ), name_name ) );
					return sig_t::raise;
				}
				auto& a1 = values.top();
				a1.value<T>() /= a2;
				return sig_t::norm; } };

		template<typename T>
		struct FloatPlus : public Action {
			inline FloatPlus() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_plus, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( Attribute::newImplicit( static_cast<T>( a1 + a2 ) ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatMinus : public Action {
			inline FloatMinus() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_minus, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( Attribute::newImplicit( static_cast<T>( a1 - a2 ) ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatMultiply : public Action {
			inline FloatMultiply() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_multiply, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( Attribute::newImplicit( static_cast<T>( a1 * a2 ) ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatDivide : public Action {
			inline FloatDivide() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_divide, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				if( a2 == 0.0 )
				{
					values.push( Attribute::newExplicit( name( "division_by_zero" ), name_name ) );
					return sig_t::raise;
				}
				auto a1 = values.top().value<T>(); values.pop();
				values.push( Attribute::newImplicit( static_cast<T>( a1 / a2 ) ) );
				return sig_t::norm; } };

		template<typename T>
		struct FloatReset : public Action {
			inline FloatReset() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_reset, name_operator, type::mapCppType( CPPTYPE( T ) ), {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<T>(); a1 = 0.0; return sig_t::norm; } };


		template<typename T>
		void registerFloat()
		{
			scope::global().registerType( type::mapCppType( CPPTYPE( T ) ) );
			scope::global().registerAction( new FloatConstruct<T>() );
			scope::global().registerAction( new FloatCopyConstruct<T>() );
			scope::global().registerAction( new FloatBoolConstruct<T>() );
			scope::global().registerAction( new FloatByteConstruct<T>() );
			scope::global().registerAction( new FloatCharConstruct<T>() );
			scope::global().registerAction( new FloatIntConstruct<T>() );
			scope::global().registerAction( new FloatShortConstruct<T>() );
			scope::global().registerAction( new FloatLongConstruct<T>() );
			if( typeid( T ) == typeid( flt_t ) )
			{
				scope::global().registerAction( new FloatLowConstruct() );
				scope::global().registerAction( new FloatHighConstruct() );
			}
			else if( typeid( T ) == typeid( low_t ) )
			{
				scope::global().registerAction( new LowFloatConstruct() );
				scope::global().registerAction( new LowHighConstruct() );
			}
			else if( typeid( T ) == typeid( high_t ) )
			{
				scope::global().registerAction( new HighFloatConstruct() );
				scope::global().registerAction( new HighLowConstruct() );
			}
			scope::global().registerAction( new FloatIndexConstruct<T>() );
			scope::global().registerAction( new FloatB8Construct<T>() );
			scope::global().registerAction( new FloatB16Construct<T>() );
			scope::global().registerAction( new FloatB32Construct<T>() );
			scope::global().registerAction( new FloatB64Construct<T>() );
			scope::global().registerAction( new FloatCompare<T>() );
			scope::global().registerAction( new FloatUnaryMinus<T>() );
			scope::global().registerAction( new FloatPlusAssign<T>() );
			scope::global().registerAction( new FloatMinusAssign<T>() );
			scope::global().registerAction( new FloatMultiplyAssign<T>() );
			scope::global().registerAction( new FloatDivideAssign<T>() );
			scope::global().registerAction( new FloatPlus<T>() );
			scope::global().registerAction( new FloatMinus<T>() );
			scope::global().registerAction( new FloatMultiply<T>() );
			scope::global().registerAction( new FloatDivide<T>() );
			scope::global().registerAction( new FloatReset<T>() );
		}
	}
}
