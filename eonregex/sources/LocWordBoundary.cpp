#include "LocWordBoundary.h"


namespace eon
{
	namespace rx
	{
		bool LocWordEnd::_match( RxData& data, index_t steps )
		{
			if( data.bounds() )
			{
				return !string::isSeparatorSpace( data.prev() )
					&& ( !data || string::isSeparatorSpace( data() ) ) ? true : false;
			}
			else
			{
				return string::isWordChar( data.prev() )
					&& ( !data || string::isSeparatorSpace( data() ) || string::isPunctuation( data() ) ) ? true : false;
			}
		}
	}
}
