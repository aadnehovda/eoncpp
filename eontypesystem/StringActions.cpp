#include "StringActions.h"
#include "TupleFactory.h"


namespace eon
{
	namespace type
	{
		sig_t StringCompare::operator()( stack<Attribute>& values ) const
		{
			auto& a2 = values.top().value<string>(); values.pop();
			auto& a1 = values.top().value<string>(); values.pop();
			values.push( Attribute::newExplicit( a1.compare( a2 ), name_int ) );
			return sig_t::norm;
		}

		void registerString()
		{
			scope::global().registerType( name_string );
			scope::global().registerAction( new StringConstruct() );
			scope::global().registerAction( new StringCopyConstruct() );
			scope::global().registerAction( new StringByteConstruct() );
			scope::global().registerAction( new StringCharConstruct() );
			scope::global().registerAction( new StringIntConstruct() );
			scope::global().registerAction( new StringShortConstruct() );
			scope::global().registerAction( new StringLongConstruct() );
			scope::global().registerAction( new StringFloatConstruct() );
			scope::global().registerAction( new StringLowConstruct() );
			scope::global().registerAction( new StringHighConstruct() );
			scope::global().registerAction( new StringIndexConstruct() );
			scope::global().registerAction( new StringNameConstruct() );
			scope::global().registerAction( new StringBytesConstruct() );
			scope::global().registerAction( new StringCompare() );
			scope::global().registerAction( new StringReset() );
		}
	}
}
