#include <eoninlinetest/InlineTest.h>


namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// String Test Functions
	//

#ifdef EON_TEST_MODE

	// Construct a C-array with initial elements values, as a self-deleting
	// Smart-pointer
	template<typename T>
	std::shared_ptr<T[]> _initialize( std::initializer_list<T> values )
	{
		std::shared_ptr<T[]> value( new T[ values.size() ] );
		int i = 0;
		for( auto c : values )
			value[ i++ ] = c;
		return value;
	}

	// G++ doesn't like "unsigned char" as type in macros!
	using _uchar_t = unsigned char;

#endif
}
