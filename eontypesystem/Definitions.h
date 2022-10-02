#pragma once

#include <eonexcept/Exception.h>
#include <eoncontainers/Stack.h>
#include <eoncontainers/ReflexiveMap.h>
#include <eonstring/Name.h>
#include <eonstring/Stringifier.h>
#include <typeindex>
#include <vector>
#include <unordered_set>



///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	namespace type
	{
		EONEXCEPT( IncompatibleType );
		EONEXCEPT( AccessDenied );
		EONEXCEPT( WrongType );
		EONEXCEPT( NotFound );
		EONEXCEPT( DuplicateName );
	}

#define ptr_t void*
#if SIZE_MAX == UINT64_MAX
#	define val_t(type) (type)(uint64_t)
#	define to_ptr (ptr_t)(uint64_t)
#elif SIZE_MAX == UINT32_MAX
#	define val_t(type) (type)(int32_t)
#	define to_ptr (ptr_t)(uint32_t)
#else
#	error Unknown pointer size
#endif


	extern const eon::name_t name_ttypetuple;


	using b8_t = uint8_t;
	using b16_t = uint16_t;
	using b32_t = uint32_t;
	using b64_t = uint64_t;


	using handle_t = void*;
	static const handle_t no_handle{ nullptr };


#define CPPTYPE( x ) std::type_index( typeid( x ) )
}
