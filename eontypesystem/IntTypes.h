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
		struct IntConstruct : public Action {
			inline IntConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ) ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( static_cast<T>( 0 ) ); return sig_t::norm; } };

		template<typename T>
		struct IntCopyConstruct : public Action {
			inline IntCopyConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, type::mapCppType(
					CPPTYPE( T ) ) } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				const auto a1 = values.top().value<T>(); values.pop(); values.push( a1 ); return sig_t::norm; } };

		template<typename T>
		struct IntBoolConstruct : public Action {
			inline IntBoolConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_bool } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<bool>(); values.pop(); values.push( static_cast<T>( a1 ? 1 : 0 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntByteConstruct : public Action {
			inline IntByteConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_byte } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<byte_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; } };

		template<typename T>
		struct IntCharConstruct : public Action {
			inline IntCharConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_char } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<char_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; } };

		struct IntShortConstruct : public Action {
			inline IntShortConstruct() : Action( TypeTuple::action( name_int, name_constructor, name_operator, name_int, {
				{ no_name, name_short } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<short_t>(); values.pop(); values.push( Attribute::newImplicit( (int_t)a1 ) );
				return sig_t::norm; } };
		struct IntLongConstruct : public Action {
			inline IntLongConstruct() : Action( TypeTuple::action( name_int, name_constructor, name_operator, name_int, {
				{ no_name, name_long } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<long_t>(); values.pop(); values.push( Attribute::newImplicit( (int_t)a1 ) );
				return sig_t::norm; } };
		struct ShortIntConstruct : public Action {
			inline ShortIntConstruct() : Action( TypeTuple::action( name_short, name_constructor, name_operator,
				name_short, { { no_name, name_int } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push( Attribute::newImplicit( (short_t)a1 ) );
				return sig_t::norm; } };
		struct ShortLongConstruct : public Action {
			inline ShortLongConstruct() : Action( TypeTuple::action( name_short, name_constructor, name_operator,
				name_short, { { no_name, name_long } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<long_t>(); values.pop(); values.push( Attribute::newImplicit( (short_t)a1 ) );
				return sig_t::norm; } };
		struct LongIntConstruct : public Action {
			inline LongIntConstruct() : Action( TypeTuple::action( name_long, name_constructor, name_operator, name_long, {
				{ no_name, name_int } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push( Attribute::newImplicit( (long_t)a1 ) );
				return sig_t::norm; } };
		struct LongShortConstruct : public Action {
			inline LongShortConstruct() : Action( TypeTuple::action( name_long, name_constructor, name_operator, name_long,
				{ { no_name, name_short } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<short_t>(); values.pop(); values.push( Attribute::newImplicit( (long_t)a1 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntFloatConstruct : public Action {
			inline IntFloatConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_float } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; } };

		template<typename T>
		struct IntLowConstruct : public Action {
			inline IntLowConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_low } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<low_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; } };

		template<typename T>
		struct IntHighConstruct : public Action {
			inline IntHighConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_high } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<high_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; } };

		template<typename T>
		struct IntIndexConstruct : public Action {
			inline IntIndexConstruct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ), { { no_name, name_index } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<index_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; } };


		template<typename T>
		struct IntB8Construct : public Action {
			inline IntB8Construct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ),
				{ { no_name, name_b8 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b8_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; }
		};
		template<typename T>
		struct IntB16Construct : public Action {
			inline IntB16Construct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ),
				{ { no_name, name_b16 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b16_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; }
		};
		template<typename T>
		struct IntB32Construct : public Action {
			inline IntB32Construct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ),
				{ { no_name, name_b32 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b32_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; }
		};
		template<typename T>
		struct IntB64Construct : public Action {
			inline IntB64Construct() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_constructor, name_operator, type::mapCppType( CPPTYPE( T ) ),
				{ { no_name, name_b64 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b64_t>(); values.pop(); values.push( (T)a1 ); return sig_t::norm; }
		};


		template<typename T>
		struct IntCompare : public Action {
			inline IntCompare() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ), symbol_cmp, name_operator,
				name_int, { { no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntLt : public Action {
			inline IntLt() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ), symbol_lt, name_operator,
				name_int, { { no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<bool>( a1 < a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntLe : public Action {
			inline IntLe() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ), symbol_le, name_operator,
				name_int, { { no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<bool>( a1 <= a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntGt : public Action {
			inline IntGt() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ), symbol_gt, name_operator,
				name_int, { { no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<bool>( a1 > a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntGe : public Action {
			inline IntGe() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ), symbol_ge, name_operator,
				name_int, { { no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<bool>( a1 >= a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntEq : public Action {
			inline IntEq() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ), symbol_eq, name_operator,
				name_int, { { no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<bool>( a1 == a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntNe : public Action {
			inline IntNe() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ), symbol_ne, name_operator,
				name_int, { { no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<bool>( a1 != a2 ) );
				return sig_t::norm; } };
		

		template<typename T>
		struct IntUnaryMinus : public Action {
			inline IntUnaryMinus() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_unary_minus, name_operator, type::mapCppType( CPPTYPE( T ) ), {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<T>(); values.pop();
				values.push( -a1 );
				return sig_t::norm; } };


		template<typename T>
		struct IntPlusAssign : public Action {
			inline IntPlusAssign() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_plus_assign, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto& a1 = values.top();
				a1.value<T>() += a2;
				return sig_t::norm; } };

		template<typename T>
		struct IntMinusAssign : public Action {
			inline IntMinusAssign() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_minus_assign, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto& a1 = values.top();
				a1.value<T>() -= a2;
				return sig_t::norm; } };

		template<typename T>
		struct IntMultiplyAssign : public Action {
			inline IntMultiplyAssign() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_multiply_assign, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto& a1 = values.top();
				a1.value<T>() *= a2;
				return sig_t::norm; } };

		template<typename T>
		struct IntDivideAssign : public Action {
			inline IntDivideAssign() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_divide_assign, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				if( a2 == 0 )
				{
					values.push( name( "division_by_zero" ) );
					return sig_t::raise;
				}
				auto& a1 = values.top();
				a1.value<T>() /= a2;
				return sig_t::norm; } };

		template<typename T>
		struct IntPlus : public Action {
			inline IntPlus() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_plus, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<T>( a1 + a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntMinus : public Action {
			inline IntMinus() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_minus, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<T>( a1 - a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntMultiply : public Action {
			inline IntMultiply() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_multiply, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<T>( a1 * a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntDivide : public Action {
			inline IntDivide() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				symbol_divide, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				if( a2 == 0 )
				{
					values.push( name( "division_by_zero" ) );
					return sig_t::raise;
				}
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<T>( a1 / a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntMod: public Action {
			inline IntMod() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_mod, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				if( a2 == 0 )
				{
					values.push( name( "division_by_zero" ) );
					return sig_t::raise;
				}
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<T>( a1 % a2 ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntPow : public Action {
			inline IntPow() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_pow, name_operator, type::mapCppType( CPPTYPE( T ) ), {
					{ no_name, type::mapCppType( CPPTYPE( T ) ) } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto a2 = values.top().value<T>(); values.pop();
				auto a1 = values.top().value<T>(); values.pop();
				values.push( static_cast<T>( std::pow( a1, a2 ) ) );
				return sig_t::norm; } };

		template<typename T>
		struct IntReset : public Action {
			inline IntReset() : Action( TypeTuple::action( type::mapCppType( CPPTYPE( T ) ),
				name_reset, name_operator, type::mapCppType( CPPTYPE( T ) ), {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<T>(); a1 = 0; return sig_t::norm; } };


		template<typename T>
		void registerInt()
		{
			scope::global().registerType( type::mapCppType( CPPTYPE( T ) ) );
			scope::global().registerAction( new IntConstruct<T>() );
			scope::global().registerAction( new IntCopyConstruct<T>() );
			scope::global().registerAction( new IntBoolConstruct<T>() );
			scope::global().registerAction( new IntByteConstruct<T>() );
			scope::global().registerAction( new IntCharConstruct<T>() );
			if( typeid( T ) == typeid( int_t ) )
			{
				scope::global().registerAction( new IntShortConstruct() );
				scope::global().registerAction( new IntLongConstruct() );
			}
			else if( typeid( T ) == typeid( short_t ) )
			{
				scope::global().registerAction( new ShortIntConstruct() );
				scope::global().registerAction( new ShortLongConstruct() );
			}
			else if( typeid( T ) == typeid( long_t ) )
			{
				scope::global().registerAction( new LongIntConstruct() );
				scope::global().registerAction( new LongShortConstruct() );
			}
			scope::global().registerAction( new IntFloatConstruct<T>() );
			scope::global().registerAction( new IntLowConstruct<T>() );
			scope::global().registerAction( new IntHighConstruct<T>() );
			scope::global().registerAction( new IntIndexConstruct<T>() );
			scope::global().registerAction( new IntB8Construct<T>() );
			scope::global().registerAction( new IntB16Construct<T>() );
			scope::global().registerAction( new IntB32Construct<T>() );
			scope::global().registerAction( new IntB64Construct<T>() );
			scope::global().registerAction( new IntCompare<T>() );
			scope::global().registerAction( new IntLt<T>() );
			scope::global().registerAction( new IntLe<T>() );
			scope::global().registerAction( new IntGt<T>() );
			scope::global().registerAction( new IntGe<T>() );
			scope::global().registerAction( new IntEq<T>() );
			scope::global().registerAction( new IntNe<T>() );
			scope::global().registerAction( new IntUnaryMinus<T>() );
			scope::global().registerAction( new IntPlusAssign<T>() );
			scope::global().registerAction( new IntMinusAssign<T>() );
			scope::global().registerAction( new IntMultiplyAssign<T>() );
			scope::global().registerAction( new IntDivideAssign<T>() );
			scope::global().registerAction( new IntPlus<T>() );
			scope::global().registerAction( new IntMinus<T>() );
			scope::global().registerAction( new IntMultiply<T>() );
			scope::global().registerAction( new IntDivide<T>() );
			scope::global().registerAction( new IntMod<T>() );
			scope::global().registerAction( new IntPow<T>() );
			scope::global().registerAction( new IntReset<T>() );
		}
	}
}
