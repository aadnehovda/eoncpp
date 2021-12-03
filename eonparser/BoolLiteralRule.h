#pragma once
#include <eontokenizer/ReTokenizer.h>


/******************************************************************************
  The 'eon' namespace encloses all public functionality
******************************************************************************/
namespace eon
{
	namespace parser
	{
		class BoolLiteralRule : public ReTokenizer::RuleDef
		{
		public:
			inline BoolLiteralRule( name_t name ) : ReTokenizer::RuleDef( name ) {}
			bool match( TokenParser& parser, std::vector<Token>& output ) const noexcept override;
		};
	}
}
