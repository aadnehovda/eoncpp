#pragma once

#include "Definitions.h"
#include "TypeTuple.h"
#include "TypeMap.h"



///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	class Tuple;




	///////////////////////////////////////////////////////////////////////////
	//
	// Eon Attribute class - eon::Attribute
	//
	// Tuple attribute.
	//
	class Attribute
	{
		///////////////////////////////////////////////////////////////////////
		//
		// Construction
		//
	public:

		// Default constructor for place-holder instance object.
		Attribute() { _prep(); }

//		// Construct for a tuple
//		Attribute( Tuple&& value );

		// Copy another attribute, if there is a value, it will be copied!
		Attribute( const Attribute& other ) { *this = other; }

		// Copy another attribute, if there is a value, it will be referenced!
		Attribute( Attribute& other ) { *this = other; }

		// Take ownership of another attribute's details
		Attribute( Attribute&& other ) noexcept { *this = std::move( other ); }

/*		// Construct for name value
		inline Attribute( name_t name_value ) { _prep(); Type = name_name; Value = new name_t( name_value ); }

		// Construct for a type without value
		inline Attribute( TypeTuple type ) noexcept { _prep(); Type = type; }

		// Construct for a type-tuple value
		inline Attribute( TypeTuple value, name_t dummy ) {
			_prep(); Type = name_typetuple; Value = new TypeTuple( value ); }

		// Construct for ambiguous integer type
		template<typename T>
		inline Attribute( T value, name_t type_name ) { _prep(); Type = type_name; Value = new T( value ); Owned = true; }*/

		// Construct for any supported value type
		// Throws eon::type::AccessDenied if value is not legal for any tuple type!
		template<typename T>
		Attribute( T&& value, type::hint type_hint = type::hint::none )
		{
			_prep();
			auto type_name = type::mapCppType( CPPTYPE( T ), type_hint );
			if( type_name == no_name )
				throw type::AccessDenied();
			if( type_name == name_tuple )
				Type = ((Tuple*)&value)->type();
			else
				Type = type_name;
			Value = new T( std::move( value ) );
		}

		// Construct for any supported value type
		// Throws eon::type::AccessDenied if value is not legal for any tuple type!
		template<typename T>
		Attribute( const T& value, type::hint type_hint = type::hint::none )
		{
			_prep();
			auto type_name = type::mapCppType( CPPTYPE( T ), type_hint );
			if( type_name == no_name )
				throw type::AccessDenied();
			Type = type_name;
			Value = new T( std::move( value ) );
		}

		inline virtual ~Attribute() { _clear(); }




		///////////////////////////////////////////////////////////////////////
		//
		// Factory methods
		//
	public:

		// Create attribute for specified type without value.
		static inline Attribute newVoid( TypeTuple type ) { Attribute a; a.Type = type; return a; }

		// Create tuple attribute.
		static Attribute newTuple( Tuple&& value );

		// Create name attribute.
		static inline Attribute newName( name_t value ) {
			Attribute a; a.Type = name_name; a.Value = new name_t( value ); return a; }

		// Create type-tuple attribute.
		static inline Attribute newTypeTuple( TypeTuple&& value ) {
			Attribute a; a.Type = name_typetuple; a.Value = new TypeTuple( std::move( value ) ); return a; }

		// Create type-tuple attribute.
		static inline Attribute newTypeTuple( const TypeTuple& value ) {
			Attribute a; a.Type = name_typetuple; a.Value = new TypeTuple( value ); return a; }

		// Create attribute for value of specified type (no explicit type-deduction!).
		template<typename T>
		static inline Attribute newExplicit( T&& value, name_t value_type ) {
			Attribute a; a.Type = value_type; a.Value = new T( std::move( value ) ); return a; }

		// Create attribute for value of specified type (no explicit type-deduction!).
		template<typename T>
		static inline Attribute newExplicit( const T& value, name_t value_type ) {
			Attribute a; a.Type = value_type; a.Value = new T( value ); return a; }

		// Create attribute for value, deduce type.
		// Some types are different in Eon but the same in C++. (These can be byte_t vs. b8, index_t vs. b32 or b64.)
		// To ensure correct type, please provide a 'hint'.
		// Throws eon::type::AccessDenied if value is not legal for any tuple type!
		template<typename T>
		static Attribute newImplicit( T&& value, type::hint type_hint = type::hint::none )
		{
			auto type_name = type::mapCppType( std::type_index( typeid( T ) ), type_hint );
			if( type_name == no_name )
				throw type::AccessDenied();
			Attribute a;
			a.Type = type_name;
			a.Value = new T( std::move( value ) );
			return a;
		}

		// Create attribute for value, deduce type.
		// Some types are different in Eon but the same in C++. (These can be byte_t vs. b8, index_t vs. b32 or b64.)
		// To ensure correct type, please provide a 'hint'.
		// Throws eon::type::AccessDenied if value is not legal for any tuple type!
		template<typename T>
		static Attribute newImplicit( const T& value, type::hint type_hint = type::hint::none )
		{
			auto type_name = type::mapCppType( std::type_index( typeid( T ) ), type_hint );
			if( type_name == no_name )
				throw type::AccessDenied();
			Attribute a;
			a.Type = type_name;
			a.Value = new T( value );
			return a;
		}




		///////////////////////////////////////////////////////////////////////
		//
		// General modifiers
		//
	public:

		// Copy another attribute, if there is a value, it will be copied!
		Attribute& operator=( const Attribute& other );

		// Copy another attribute, if there is a value, it will be referenced!
		Attribute& operator=( Attribute& other );

		// Take ownership of another attribute's details.
		inline Attribute& operator=( Attribute&& other ) noexcept {
			_clear(); Type = std::move( other.Type ); Value = other.Value; other.Value = nullptr;
			Owned = other.Owned; other.Owned = true; return *this; }




		///////////////////////////////////////////////////////////////////////
		//
		// Basic access
		//
	public:

		// Check that the attribute is not nothing.
		inline operator bool() const noexcept { return static_cast<bool>( Type ); }

		// Get attribute type
		inline const TypeTuple& type() const noexcept { return Type; }

		// Get attribute value
		// Throws eon::type::WrongType if value has a different type than requested
		template<typename T, type::hint type_hint = type::hint::none>
		T& value()
		{
			if( Type.nameValue() == name_syntax )
			{
				if( typeid( T ) == typeid( name_t ) )
					return *(T*)Value;
				else
					throw type::WrongType( "Trying to get \"" + eon::str( type::mapCppType( CPPTYPE( T ), type_hint ) )
						+ "\" attribute value, which is \"syntax\"" );
			}
			
			TypeTuple type( type::mapCppType( CPPTYPE( T ), type_hint ) );
			if( type.nameValue() == name_typetuple )
				return *(T*)Value;
			else if( Type.nameValue() == name_ttypetuple && type.name() == name_tuple )
				return *(T*)Value;
			else if( type != Type )
				throw type::WrongType( "Trying to get \"" + eon::str( type::mapCppType( CPPTYPE( T ), type_hint ) )
					+ "\" attribute value, which is \"" + Type.str() + "\"" );
			return *(T*)Value;
		}
		template<typename T, type::hint type_hint = type::hint::none>
		inline const T& value() const { return *(const T*)&( (Attribute*)this )->value<T, type_hint>(); }
		
		// Stringify value
		void str( Stringifier& strf ) const;




		///////////////////////////////////////////////////////////////////////
		//
		// Definitions
		//
	private:

		class TypeHandler
		{
		public:
			virtual void destroy( void* value ) noexcept = 0;
			virtual void* copy( const void* value ) = 0;
			virtual void str( Stringifier& strf, const void* value ) const = 0;
		};
		using TypeHandlerPtr = std::shared_ptr<TypeHandler>;

		class TypeHandlerStr : public TypeHandler
		{
		public:
			void destroy( void* value ) noexcept override { delete (string*)value; }
			void* copy( const void* value ) override { return new string( *(string*)value ); }
			void str( Stringifier& strf, const void* value ) const override { strf.word( *(string*)value ); }
		};

		class TypeHandlerName : public TypeHandler
		{
		public:
			void destroy( void* value ) noexcept override { delete (name_t*)value; }
			void* copy( const void* value ) override { return new name_t( *(name_t*)value ); }
			void str( Stringifier& strf, const void* value ) const override { strf.word( eon::str( *(name_t*)value ) ); }
		};

		template<typename T>
		class TypeHandlerStrMethod : public TypeHandler
		{
		public:
			void destroy( void* value ) noexcept override { delete (T*)value; }
			void* copy( const void* value ) override { return new T( *(T*)value ); }
			void str( Stringifier& strf, const void* value ) const override { strf.word( ((T*)value)->str() ); }
		};

		template<typename T>
		class TypeHandlerStrSupported : public TypeHandler
		{
		public:
			void destroy( void* value ) noexcept override { delete (T*)value; }
			void* copy( const void* value ) override { return new T( *(T*)value ); }
			void str( Stringifier& strf, const void* value ) const override { strf.word( string( *(T*)value ) ); }
		};




		///////////////////////////////////////////////////////////////////////
		//
		// Helpers
		//
	private:

		void _prep();
		void _clear() noexcept;




		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	private:

		TypeTuple Type;
		void* Value{ nullptr };
		bool Owned{ true };

		static std::unordered_map<name_t, TypeHandlerPtr> TypeHandlers;
	};
}
