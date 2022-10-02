#include "PrimitiveTypes.h"
#include "TupleFactory.h"


namespace eon
{
	namespace type
	{
		sig_t BoolCompare::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<bool>(); values.pop();
			auto a1 = values.top().value<bool>(); values.pop();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) ) );
			return sig_t::norm;
		}
		sig_t BoolAnd::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<bool>(); values.pop();
			auto a1 = values.top().value<bool>(); values.pop();
			values.push( Attribute::newImplicit( a1 && a2 ) );
			return sig_t::norm;
		}
		sig_t BoolOr::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<bool>(); values.pop();
			auto a1 = values.top().value<bool>(); values.pop();
			values.push( Attribute::newImplicit( a1 || a2 ) );
			return sig_t::norm;
		}
		sig_t BoolXor::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<bool>(); values.pop();
			auto a1 = values.top().value<bool>(); values.pop();
			values.push( Attribute::newImplicit( a1 != a2 ) );
			return sig_t::norm;
		}
		void registerBool()
		{
			scope::global().registerType( name_bool );
			scope::global().registerAction( new BoolConstruct() );
			scope::global().registerAction( new BoolCopyConstruct() );
			scope::global().registerAction( new BoolByteConstruct() );
			scope::global().registerAction( new BoolCharConstruct() );
			scope::global().registerAction( new BoolIntConstruct() );
			scope::global().registerAction( new BoolShortConstruct() );
			scope::global().registerAction( new BoolLongConstruct() );
			scope::global().registerAction( new BoolFloatConstruct() );
			scope::global().registerAction( new BoolLowConstruct() );
			scope::global().registerAction( new BoolHighConstruct() );
			scope::global().registerAction( new BoolIndexConstruct() );
			scope::global().registerAction( new BoolCompare() );
			scope::global().registerAction( new BoolNot() );
			scope::global().registerAction( new BoolAnd() );
			scope::global().registerAction( new BoolOr() );
			scope::global().registerAction( new BoolXor() );
			scope::global().registerAction( new BoolReset() );
		}




		sig_t ByteCompare::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<byte_t>(); values.pop();
			auto a1 = values.top().value<byte_t>(); values.pop();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) ) );
			return sig_t::norm;
		}
		void registerByte()
		{
			scope::global().registerType( name_byte );
			scope::global().registerAction( new ByteConstruct() );
			scope::global().registerAction( new ByteCopyConstruct() );
			scope::global().registerAction( new ByteBoolConstruct() );
			scope::global().registerAction( new ByteCharConstruct() );
			scope::global().registerAction( new ByteIntConstruct() );
			scope::global().registerAction( new ByteShortConstruct() );
			scope::global().registerAction( new ByteLongConstruct() );
			scope::global().registerAction( new ByteFloatConstruct() );
			scope::global().registerAction( new ByteLowConstruct() );
			scope::global().registerAction( new ByteHighConstruct() );
			scope::global().registerAction( new ByteIndexConstruct() );
			scope::global().registerAction( new ByteCompare() );
			scope::global().registerAction( new ByteReset() );
		}




		sig_t CharCompare::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<char_t>(); values.pop();
			auto a1 = values.top().value<char_t>(); values.pop();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) ) );
			return sig_t::norm;
		}
		void registerChar()
		{
			scope::global().registerType( name_char );
			scope::global().registerAction( new CharConstruct() );
			scope::global().registerAction( new CharCopyConstruct() );
			scope::global().registerAction( new CharBoolConstruct() );
			scope::global().registerAction( new CharByteConstruct() );
			scope::global().registerAction( new CharIntConstruct() );
			scope::global().registerAction( new CharShortConstruct() );
			scope::global().registerAction( new CharLongConstruct() );
			scope::global().registerAction( new CharFloatConstruct() );
			scope::global().registerAction( new CharLowConstruct() );
			scope::global().registerAction( new CharHighConstruct() );
			scope::global().registerAction( new CharIndexConstruct() );
			scope::global().registerAction( new CharCompare() );
			scope::global().registerAction( new CharReset() );
		}




		sig_t IndexCompare::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			auto a1 = values.top().value<index_t>(); values.pop();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) ) );
			return sig_t::norm;
		}
		sig_t IndexPlusAssign::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			auto& a1 = values.top();
			a1.value<index_t>() += a2;
			return sig_t::norm;
		}
		sig_t IndexMinusAssign::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			auto& a1 = values.top();
			a1.value<index_t>() -= a2;
			return sig_t::norm;
		}
		sig_t IndexMultiplyAssign::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			auto& a1 = values.top();
			a1.value<index_t>() *= a2;
			return sig_t::norm;
		}
		sig_t IndexDivideAssign::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			if( a2 == 0 )
			{
				values.push( Attribute::newImplicit( name( "division_by_zero" ) ) );
				return sig_t::raise;
			}
			auto& a1 = values.top();
			a1.value<index_t>() /= a2;
			return sig_t::norm;
		}
		sig_t IndexPlus::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			auto a1 = values.top().value<index_t>();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 + a2 ) ) );
			return sig_t::norm;
		}
		sig_t IndexMinus::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			auto a1 = values.top().value<index_t>();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 - a2 ) ) );
			return sig_t::norm;
		}
		sig_t IndexMultiply::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			auto a1 = values.top().value<index_t>();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 * a2 ) ) );
			return sig_t::norm;
		}
		sig_t IndexDivide::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<index_t>(); values.pop();
			if( a2 == 0 )
			{
				values.push( Attribute::newImplicit( name( "division_by_zero" ) ) );
				return sig_t::raise;
			}
			auto a1 = values.top().value<index_t>();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 / a2 ) ) );
			return sig_t::norm;
		}
		void registerIndex()
		{
			scope::global().registerType( name_index );
			scope::global().registerAction( new IndexConstruct() );
			scope::global().registerAction( new IndexCopyConstruct() );
			scope::global().registerAction( new IndexBoolConstruct() );
			scope::global().registerAction( new IndexByteConstruct() );
			scope::global().registerAction( new IndexCharConstruct() );
			scope::global().registerAction( new IndexIntConstruct() );
			scope::global().registerAction( new IndexShortConstruct() );
			scope::global().registerAction( new IndexLongConstruct() );
			scope::global().registerAction( new IndexFloatConstruct() );
			scope::global().registerAction( new IndexLowConstruct() );
			scope::global().registerAction( new IndexHighConstruct() );
			scope::global().registerAction( new IndexB8Construct() );
			scope::global().registerAction( new IndexB16Construct() );
			scope::global().registerAction( new IndexB32Construct() );
			scope::global().registerAction( new IndexB64Construct() );
			scope::global().registerAction( new IndexCompare() );
			scope::global().registerAction( new IndexPlusAssign() );
			scope::global().registerAction( new IndexMinusAssign() );
			scope::global().registerAction( new IndexMultiplyAssign() );
			scope::global().registerAction( new IndexDivideAssign() );
			scope::global().registerAction( new IndexPlus() );
			scope::global().registerAction( new IndexMinus() );
			scope::global().registerAction( new IndexMultiply() );
			scope::global().registerAction( new IndexDivide() );
			scope::global().registerAction( new IndexReset() );
		}




#ifndef _DEBUG
		sig_t NameCompare::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<name_t>(); values.pop();
			auto a1 = values.top().value<name_t>(); values.pop();
			values.push( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) );
			return sig_t::norm;
		}
#else
		sig_t NameCompare::operator()( stack<Attribute>& values ) const
		{
			auto& a2 = values.top().value<name_t>(); values.pop();
			auto& a1 = values.top().value<name_t>(); values.pop();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) ) );
			return sig_t::norm;
		}
#endif
		void registerName()
		{
			scope::global().registerType( name_name );
			scope::global().registerAction( new NameConstruct() );
			scope::global().registerAction( new NameCopyConstruct() );
			scope::global().registerAction( new NameCompare() );
			scope::global().registerAction( new NameReset() );
		}




		sig_t HandleCompare::operator()( stack<Attribute>& values ) const
		{
			auto a2 = values.top().value<handle_t>(); values.pop();
			auto a1 = values.top().value<handle_t>(); values.pop();
			values.push( Attribute::newImplicit( static_cast<int_t>( a1 < a2 ? -1 : a2 < a1 ? 1 : 0 ) ) );
			return sig_t::norm;
		}
		void registerHandle()
		{
			scope::global().registerType( name_handle );
			scope::global().registerAction( new HandleConstruct() );
			scope::global().registerAction( new HandleCopyConstruct() );
			scope::global().registerAction( new HandleCompare() );
		}
	}
}
