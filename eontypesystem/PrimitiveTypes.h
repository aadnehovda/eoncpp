#pragma once

#include "Action.h"
#include "Operators.h"
#include "Tuple.h"
#include "TypeTuple.h"



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
		struct BoolConstruct : public Action {
			inline BoolConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator,
				name_bool ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( Attribute::newExplicit( false, name_bool ) ); return sig_t::norm; }
		};
		struct BoolCopyConstruct : public Action {
			inline BoolCopyConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_bool } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				const bool a1 = values.top().value<bool>(); values.pop(); values.push(
					Attribute::newExplicit( a1, name_bool ) ); return sig_t::norm; }
		};
		struct BoolByteConstruct : public Action {
			inline BoolByteConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_byte } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<byte_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1 != 0, name_bool ) ); return sig_t::norm; }
		};
		struct BoolCharConstruct : public Action {
			inline BoolCharConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_char } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<char_t>(); values.pop(); values.push( Attribute::newExplicit( a1 != 0, name_bool ) );
				return sig_t::norm; }
		};
		struct BoolIntConstruct : public Action {
			inline BoolIntConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_int } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<int_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1 != 0, name_bool ) ); return sig_t::norm; }
		};
		struct BoolShortConstruct : public Action {
			inline BoolShortConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_short } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<short_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1 != 0, name_bool ) ); return sig_t::norm; }
		};
		struct BoolLongConstruct : public Action {
			inline BoolLongConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_long } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<long_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1 != 0, name_bool ) ); return sig_t::norm; }
		};
		struct BoolFloatConstruct : public Action {
			inline BoolFloatConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_float } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1 != 0, name_bool ) ); return sig_t::norm; }
		};
		struct BoolLowConstruct : public Action {
			inline BoolLowConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_low } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<low_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1 != 0, name_bool ) ); return sig_t::norm; }
		};
		struct BoolHighConstruct : public Action {
			inline BoolHighConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_high } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<high_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1 != 0, name_bool ) ); return sig_t::norm; }
		};
		struct BoolIndexConstruct : public Action {
			inline BoolIndexConstruct() : Action( TypeTuple::action( name_bool, name_constructor, name_operator, name_bool,
				{ { no_name, name_index } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<index_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1 != 0, name_bool ) ); return sig_t::norm; }
		};
		struct BoolCompare : public Action {
			inline BoolCompare() : Action( TypeTuple::action( name_bool, symbol_cmp, name_operator, name_int,
				{ { no_name, name_bool } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct BoolNot : public Action {
			inline BoolNot() : Action( TypeTuple::action( name_bool, name_not, name_operator, name_bool ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<bool>(); values.pop(); values.push( Attribute::newExplicit( !a1, name_bool ) );
				return sig_t::norm; }
		};
		struct BoolAnd : public Action {
			inline BoolAnd() : Action( TypeTuple::action( name_bool, name_and, name_operator, name_bool,
				{ { no_name, name_bool } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct BoolOr : public Action {
			inline BoolOr() : Action( TypeTuple::action( name_bool, name_or, name_operator, name_bool, { { no_name, name_bool } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct BoolXor : public Action {
			inline BoolXor() : Action( TypeTuple::action( name_bool, name_xor, name_operator, name_bool,
				{ { no_name, name_bool } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct BoolReset : public Action {
			inline BoolReset() : Action( TypeTuple::action( name_bool, name_reset, name_operator, name_bool, {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<bool>(); a1 = false; return sig_t::norm; }
		};
		void registerBool();




		struct ByteConstruct : public Action {
			inline ByteConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( Attribute::newExplicit( byte_t( 0 ), name_byte ) ); return sig_t::norm; }
		};
		struct ByteCopyConstruct : public Action {
			inline ByteCopyConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_byte } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				const auto a1 = values.top().value<byte_t>(); values.pop(); values.push(
					Attribute::newExplicit( a1, name_byte ) ); return sig_t::norm; }
		};
		struct ByteBoolConstruct : public Action {
			inline ByteBoolConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_bool } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<bool>(); values.pop(); values.push(
					Attribute::newImplicit( static_cast<byte_t>( a1 ? 1 : 0 ) ) ); return sig_t::norm; }
		};
		struct ByteCharConstruct : public Action {
			inline ByteCharConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_char } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<char_t>(); values.pop(); values.push( Attribute::newImplicit( (byte_t)a1 ) );
				return sig_t::norm; }
		};
		struct ByteIntConstruct : public Action {
			inline ByteIntConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_int } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<int_t>(); values.pop(); values.push( Attribute::newImplicit( (byte_t)a1 ) );
				return sig_t::norm; }
		};
		struct ByteShortConstruct : public Action {
			inline ByteShortConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_short } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<short_t>(); values.pop(); values.push( Attribute::newImplicit( (byte_t)a1 ) );
				return sig_t::norm; }
		};
		struct ByteLongConstruct : public Action {
			inline ByteLongConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_long } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<long_t>(); values.pop(); values.push( Attribute::newImplicit( (byte_t)a1 ) );
				return sig_t::norm; }
		};
		struct ByteFloatConstruct : public Action {
			inline ByteFloatConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_float } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push( Attribute::newImplicit( (byte_t)a1 ) );
				return sig_t::norm; }
		};
		struct ByteLowConstruct : public Action {
			inline ByteLowConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_low } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<low_t>(); values.pop(); values.push( Attribute::newImplicit( (byte_t)a1 ) );
				return sig_t::norm; }
		};
		struct ByteHighConstruct : public Action {
			inline ByteHighConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_high } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<high_t>(); values.pop(); values.push( Attribute::newImplicit( (byte_t)a1 ) );
				return sig_t::norm; }
		};
		struct ByteIndexConstruct : public Action {
			inline ByteIndexConstruct() : Action( TypeTuple::action( name_byte, name_constructor, name_operator, name_byte,
				{ { no_name, name_index } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<index_t>(); values.pop(); values.push( Attribute::newImplicit( (byte_t)a1 ) );
				return sig_t::norm; }
		};
		struct ByteCompare : public Action {
			inline ByteCompare() : Action( TypeTuple::action( name_byte, symbol_cmp, name_operator, name_int,
				{ { no_name, name_byte } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct ByteReset : public Action {
			inline ByteReset() : Action( TypeTuple::action( name_byte, name_reset, name_operator, name_byte, {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<byte_t>(); a1 = 0; return sig_t::norm; }
		};
		void registerByte();




		struct CharConstruct : public Action {
			inline CharConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( Attribute::newImplicit( char_t( 0 ) ) ); return sig_t::norm; }
		};
		struct CharCopyConstruct : public Action {
			inline CharCopyConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_char } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				const auto a1 = values.top().value<char_t>(); values.pop(); values.push( Attribute::newImplicit( a1 ) );
				return sig_t::norm; }
		};
		struct CharBoolConstruct : public Action {
			inline CharBoolConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_bool } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<bool>(); values.pop(); values.push(
					Attribute::newImplicit( static_cast<char_t>( a1 ? 1 : 0 ) ) ); return sig_t::norm; }
		};
		struct CharByteConstruct : public Action {
			inline CharByteConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_byte } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<byte_t>(); values.pop(); values.push( Attribute::newImplicit( (char_t)a1 ) );
				return sig_t::norm; }
		};
		struct CharIntConstruct : public Action {
			inline CharIntConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_int } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<int_t>(); values.pop(); values.push( Attribute::newImplicit( (char_t)a1 ) );
				return sig_t::norm; }
		};
		struct CharShortConstruct : public Action {
			inline CharShortConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_short } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<short_t>(); values.pop(); values.push( Attribute::newImplicit( (char_t)a1 ) );
				return sig_t::norm; }
		};
		struct CharLongConstruct : public Action {
			inline CharLongConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_long } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<long_t>(); values.pop(); values.push( Attribute::newImplicit( (char_t)a1 ) );
				return sig_t::norm; }
		};
		struct CharFloatConstruct : public Action {
			inline CharFloatConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_float } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push( Attribute::newImplicit( (char_t)a1 ) );
				return sig_t::norm; }
		};
		struct CharLowConstruct : public Action {
			inline CharLowConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_low } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<low_t>(); values.pop(); values.push( Attribute::newImplicit( (char_t)a1 ) );
				return sig_t::norm; }
		};
		struct CharHighConstruct : public Action {
			inline CharHighConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_high } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<high_t>(); values.pop(); values.push( Attribute::newImplicit( (char_t)a1 ) );
				return sig_t::norm; }
		};
		struct CharIndexConstruct : public Action {
			inline CharIndexConstruct() : Action( TypeTuple::action( name_char, name_constructor, name_operator, name_char,
				{ { no_name, name_index } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<index_t>(); values.pop(); values.push( Attribute::newImplicit( (char_t)a1 ) );
				return sig_t::norm; }
		};
		struct CharCompare : public Action {
			inline CharCompare() : Action( TypeTuple::action( name_char, symbol_cmp, name_operator, name_int,
				{ { no_name, name_char } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct CharReset : public Action {
			inline CharReset() : Action( TypeTuple::action( name_char, name_reset, name_operator, name_char, {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<char_t>(); a1 = 0; return sig_t::norm; }
		};
		void registerChar();




		struct IndexConstruct : public Action {
			inline IndexConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( Attribute::newImplicit( index_t( 0 ) ) ); return sig_t::norm; }
		};
		struct IndexCopyConstruct : public Action {
			inline IndexCopyConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				const auto a1 = values.top().value<index_t>(); values.pop(); values.push( Attribute::newImplicit( a1 ) );
				return sig_t::norm; }
		};
		struct IndexBoolConstruct : public Action {
			inline IndexBoolConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_bool } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<bool>(); values.pop(); values.push(
					Attribute::newImplicit( static_cast<index_t>( a1 ? 1 : 0 ) ) ); return sig_t::norm; }
		};
		struct IndexByteConstruct : public Action {
			inline IndexByteConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_byte } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<byte_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexCharConstruct : public Action {
			inline IndexCharConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_char } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<char_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexIntConstruct : public Action {
			inline IndexIntConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_int } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<int_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexShortConstruct : public Action {
			inline IndexShortConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_short } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<short_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexLongConstruct : public Action {
			inline IndexLongConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_long } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<long_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexFloatConstruct : public Action {
			inline IndexFloatConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_float } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexLowConstruct : public Action {
			inline IndexLowConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_low } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<low_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexHighConstruct : public Action {
			inline IndexHighConstruct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_high } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<high_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexB8Construct : public Action {
			inline IndexB8Construct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_b8 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b8_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexB16Construct : public Action {
			inline IndexB16Construct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_b16 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b16_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexB32Construct : public Action {
			inline IndexB32Construct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_b32 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b32_t>(); values.pop(); values.push( Attribute::newImplicit( (index_t)a1 ) );
				return sig_t::norm; }
		};
		struct IndexB64Construct : public Action {
			inline IndexB64Construct() : Action( TypeTuple::action( name_index, name_constructor, name_operator, name_index,
				{ { no_name, name_b64 } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<b64_t>(); values.pop(); values.push(
					Attribute::newExplicit<index_t>( (index_t)a1, name_index ) ); return sig_t::norm; }
		};
		struct IndexCompare : public Action {
			inline IndexCompare() : Action( TypeTuple::action( name_index, symbol_cmp, name_operator, name_int,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexPlusAssign : public Action {
			inline IndexPlusAssign() : Action( TypeTuple::action( name_index, symbol_plus_assign, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexMinusAssign : public Action {
			inline IndexMinusAssign() : Action( TypeTuple::action( name_index, symbol_minus_assign, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexMultiplyAssign : public Action {
			inline IndexMultiplyAssign() : Action( TypeTuple::action( name_index, symbol_multiply_assign, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexDivideAssign : public Action {
			inline IndexDivideAssign() : Action( TypeTuple::action( name_index, symbol_divide_assign, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexPlus : public Action {
			inline IndexPlus() : Action( TypeTuple::action( name_index, symbol_plus, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexMinus : public Action {
			inline IndexMinus() : Action( TypeTuple::action( name_index, symbol_minus, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexMultiply : public Action {
			inline IndexMultiply() : Action( TypeTuple::action( name_index, symbol_multiply, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexDivide : public Action {
			inline IndexDivide() : Action( TypeTuple::action( name_index, symbol_divide, name_operator, name_index,
				{ { no_name, name_index } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct IndexReset : public Action {
			inline IndexReset() : Action( TypeTuple::action( name_index, name_reset, name_operator, name_index, {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<index_t>(); a1 = 0; return sig_t::norm; }
		};
		void registerIndex();



#ifndef _DEBUG
		struct NameConstruct : public Action {
			inline NameConstruct() : Action( TypeTuple::action( name_name, name_constructor, name_operator, name_name ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( no_name ); return sig_t::norm; }
		};
		struct NameCopyConstruct : public Action {
			inline NameCopyConstruct() : Action( TypeTuple::action( name_name, name_constructor, name_operator, name_name,
				{ { no_name, name_name } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<name_t>(); values.pop(); values.push( Attribute::newName( a1 ) );
				return sig_t::norm; }
		};
		struct NameCompare : public Action {
			inline NameCompare() : Action( TypeTuple::action( name_name, symbol_cmp, name_operator, name_name,
				{ { no_name, name_int } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct NameReset : public Action {
			inline NameReset() : Action( TypeTuple::action( name_name, name_reset, name_operator, name_name, {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<name_t>(); a1 = no_name; return sig_t::norm; }
		};
#else
		struct NameConstruct : public Action {
			inline NameConstruct() : Action( TypeTuple::action( name_name, name_constructor, name_operator, name_name ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( Attribute::newImplicit( no_name ) ); return sig_t::norm; }
		};
		struct NameCopyConstruct : public Action {
			inline NameCopyConstruct() : Action( TypeTuple::action( name_name, name_constructor, name_operator, name_name,
				{ { no_name, name_name } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<name_t>(); values.pop(); values.push(
					Attribute::newExplicit<name_t>( a1, name_name ) ); return sig_t::norm; }
		};
		struct NameCompare : public Action {
			inline NameCompare() : Action( TypeTuple::action( name_name, symbol_cmp, name_operator, name_name,
				{ { no_name, name_int } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		struct NameReset : public Action {
			inline NameReset() : Action( TypeTuple::action( name_name, name_reset, name_operator, name_name, {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<name_t>(); a1 = no_name; return sig_t::norm; }
		};
#endif
		void registerName();




		struct HandleConstruct : public Action {
			inline HandleConstruct() : Action( TypeTuple::action( name_handle, name_constructor, name_operator, name_handle ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( Attribute::newImplicit( handle_t( nullptr ) ) ); return sig_t::norm; }
		};
		struct HandleCopyConstruct : public Action {
			inline HandleCopyConstruct() : Action( TypeTuple::action( name_handle, name_constructor, name_operator, name_handle,
				{ { no_name, name_handle } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				const auto a1 = values.top().value<handle_t>(); values.pop(); values.push( Attribute::newImplicit( a1 ) );
				return sig_t::norm; }
		};
		struct HandleCompare : public Action {
			inline HandleCompare() : Action( TypeTuple::action( name_handle, symbol_cmp, name_operator, name_handle,
				{ { no_name, name_int } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};
		void registerHandle();
	}
}
