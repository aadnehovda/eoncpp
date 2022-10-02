#pragma once

#include "Definitions.h"



///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// Eon Type Tuple class - eon::TypeTuple
	//
	// A type object identifies the type of an Eon object. It can be a single
	// name or a large tree structure.
	//
	// Primitives and custom types have the following type format: T(<name>).
	// Examples: T(bool), T(byte), T(string), etc. I.e., single name types.
	//
	// Tuples have the following type format:
	//   T(tuple, type=<tuple type>, details=(<details>))
	// Example: T(tuple, type=plain, details=((tuple, name=plain, details=(int, int, bool)), (short, short)))
	//
	// Actions have the following type format:
	//   T(action, type=<type-object's name>, cat=<category>, name=<name>, return=<return-type>, args=(<arguments>))
	// Example: T(action, type=int, name=add, return=int, args=(a=int, b=int))
	//
	class TypeTuple
	{
	public:

		// Default constructor for place-holder object
		TypeTuple() = default;

		// Construct a single-name type
		inline TypeTuple( name_t type_name ) noexcept { NameValue = type_name; }

		// Construct a names-only type
		inline TypeTuple( std::initializer_list<name_t> value ) { for( auto& elm : value ) add( elm ); }

		// Construct a multi-value type
		inline TypeTuple( std::initializer_list<std::pair<name_t, TypeTuple>> value ) {
			for( auto& elm : value ) add( elm.first, elm.second ); }

		// Copy another type
		inline TypeTuple( const TypeTuple& other ) { *this = other; }

		// Take ownership of another type's details
		inline TypeTuple( TypeTuple&& other ) noexcept { *this = std::move( other ); }

		virtual ~TypeTuple() = default;




		///////////////////////////////////////////////////////////////////////
		//
		// Factory methods
		//
	public:

		// Generate for a tuple
		static inline TypeTuple tuple( name_t tuple_type ) {
			return TypeTuple{ { no_name, name_tuple }, { name_type, tuple_type } }; }

		// Generate for an expression
		static inline TypeTuple expression( TypeTuple&& return_type ) {
			return TypeTuple{ { no_name, name_expression }, { name_return, std::move( return_type ) } }; }

		// Generate for an action
		static inline TypeTuple action( name_t type_name, name_t action_name, name_t category, TypeTuple&& return_type,
			std::initializer_list<std::pair<name_t, TypeTuple>> arguments
			= std::initializer_list<std::pair<name_t, TypeTuple>>() ) {
			return TypeTuple{ { no_name, name_action }, { name_name, action_name }, { name_type, type_name },
				{ name_category, category }, { name_return, return_type }, { name_args, TypeTuple( arguments ) } };
		}




		///////////////////////////////////////////////////////////////////////
		//
		// Modifiers
		//
	public:

		// Copy another type
		inline TypeTuple& operator=( const TypeTuple& other ) {
			NameValue = other.NameValue; TupleValue = other.TupleValue;
			NamedAttributes = other.NamedAttributes; return *this; }

		// Take ownership of another type's details
		inline TypeTuple& operator=( TypeTuple&& other ) noexcept {
			NameValue = other.NameValue; other.NameValue = no_name; TupleValue = std::move( other.TupleValue );
			NamedAttributes = std::move( other.NamedAttributes ); return *this; }

		// Add an attribute
		// Note that if this is a single-name attribute, it will lose that name and become a sub-tuple attribute!
		inline TypeTuple& add( name_t type_name ) { _ensureSubTuple(); TupleValue.push_back( type_name ); return *this; }
		inline TypeTuple& add( name_t name, name_t type_name ) {
			if( name != no_name ) NamedAttributes[ name ] = TupleValue.size(); return add( type_name ); }
		inline TypeTuple& add( TypeTuple&& sub_tuple ) {
			_ensureSubTuple(); TupleValue.push_back( std::move( sub_tuple ) ); return *this; }
		inline TypeTuple& add( const TypeTuple& sub_tuple ) {
			_ensureSubTuple(); TupleValue.push_back( sub_tuple ); return *this; }
		inline TypeTuple& add( name_t name, TypeTuple&& sub_tuple ) {
			_ensureSubTuple(); if( name != no_name ) NamedAttributes[ name ] = TupleValue.size();
			return add( std::move( sub_tuple ) ); }
		inline TypeTuple& add( name_t name, const TypeTuple& sub_tuple ) {
			_ensureSubTuple(); if( name != no_name ) NamedAttributes[ name ] = TupleValue.size(); return add( sub_tuple); }




		///////////////////////////////////////////////////////////////////////
		//
		// Read-only Methods
		//
	public:

		// Check if an actual type or just a place-holder
		inline operator bool() const noexcept { return NameValue != no_name; }
		inline bool empty() const noexcept { return NameValue == no_name; }

		// Get number of attributes
		inline index_t numAttributes() const noexcept { return isName() ? 1 : TupleValue.size(); }

		// Check if value is a name or a sub-tuple
		inline bool isName() const noexcept { return NameValue != no_name && NameValue != name_ttypetuple; }
		inline bool isSubTuple() const noexcept { return NameValue == name_ttypetuple; }
		
		// Get name value - eon::name_ttypetuple if not a single-name value, eon::no_name if not a type at all.
		inline name_t nameValue() const noexcept { return NameValue; }

		// Get name value if single-name type, or first attribute name value otherwise.
		inline name_t name() const noexcept { return NameValue != name_ttypetuple ? NameValue : TupleValue[ 0 ].NameValue; }

		// Check if this is a primitive type
		inline bool isPrimitive() const noexcept { return isName() && isPrimitive( NameValue ); }

		// Check if this is a tuple of any kind
		inline bool isTuple() const noexcept { return isSubTuple() && at( 0 ).nameValue() == name_tuple; }

		// Check if this is an action signature
		inline bool isAction() const noexcept { return isSubTuple() && at( 0 ).nameValue() == name_action; }

		// Check if this is a basic type
		inline bool isBasic() const noexcept {
			_populateBasics(); return isName() && BasicTypes.find( NameValue ) != BasicTypes.end(); }

		// Check if the object whose type this is, is an instance
		inline bool isInstance() const noexcept { return isName() && NameValue != name_action; }


		// Check if an attribute exists
		inline bool exists( index_t attribute_num ) const noexcept {
			return ( attribute_num == 0 && isName() ) || ( isTuple() && attribute_num < TupleValue.size() ); }
		inline bool exists( name_t attribute_name ) const noexcept {
			return NamedAttributes.find( attribute_name ) != NamedAttributes.end(); }

		// Get index of named attribute
		// Throws:
		//   - eon::type::NotFound : No such name
		index_t indexOf( name_t attribute_name ) const {
			auto found = NamedAttributes.find( attribute_name );
			if( found != NamedAttributes.end() ) return found->second; throw type::NotFound(
				"TypeTuple named attribute: " + eon::str( attribute_name ) ); }

		// Check if attribute is a name or a sub-tuple
		inline bool isName( index_t attribute_num ) const noexcept {
			return exists( attribute_num ) && TupleValue[ attribute_num ] != name_ttypetuple; }
		inline bool isName( name_t attribute_name ) const noexcept {
			return exists( attribute_name ) && TupleValue[ indexOf( attribute_name ) ] != name_ttypetuple; }
		inline bool isSubTuple( index_t attribute_num ) const noexcept {
			return exists( attribute_num ) && TupleValue[ attribute_num ] == name_ttypetuple; }
		inline bool isSubTuple( name_t attribute_name ) const noexcept {
			return exists( attribute_name ) && TupleValue[ indexOf( attribute_name ) ] == name_ttypetuple; }

		// Get an attribute
		// Throws:
		//   - eon::type::NotFound  : When no such attribute
		inline const TypeTuple& at( index_t attribute_num ) const {
			_assertInRange( attribute_num ); return TupleValue[ attribute_num ]; }
		inline const TypeTuple& at( int attribute_num ) const { return at( static_cast<index_t>( attribute_num ) ); }
		inline const TypeTuple& at( name_t attribute_name ) const {
			_assertInRange( indexOf( attribute_name ) ); return TupleValue[ indexOf( attribute_name ) ]; }

		// Get type tuple as 'T'-prefixed string representation
		void str( Stringifier& str ) const;
		inline string str() const { Stringifier strf; str( strf ); return strf.generateString(); }

		// Get the size necessary to allocate an object of this type
		index_t sizeOfObject() const noexcept;




		///////////////////////////////////////////////////////////////////////
		//
		// Comparison
		//
	public:

		// Compare with another Type Tuple, return -1 if this is less than other, 0 if equal, and 1 if greater
		int compare( const TypeTuple& other ) const noexcept;

		inline bool operator<( const TypeTuple& other ) const noexcept { return compare( other ) < 0; }
		inline bool operator<=( const TypeTuple& other ) const noexcept { return compare( other ) <= 0; }
		inline bool operator>( const TypeTuple& other ) const noexcept { return compare( other ) > 0; }
		inline bool operator>=( const TypeTuple& other ) const noexcept { return compare( other ) >= 0; }
		inline bool operator==( const TypeTuple& other ) const noexcept { return compare( other ) == 0; }
		inline bool operator!=( const TypeTuple& other ) const noexcept { return compare( other ) != 0; }


		// Compare with a plain name. If single-name type then compare name vs. name. Otherwise,
		// compare with first attribute (if name).
		inline int compare( name_t other ) const noexcept { auto n = name(); return n < other ? -1 : n > other ? 1 : 0; }

		inline bool operator<( name_t other ) const noexcept { return name() < other; }
		inline bool operator<=( name_t other ) const noexcept { return name() <= other; }
		inline bool operator>( name_t other ) const noexcept { return name() > other; }
		inline bool operator>=( name_t other ) const noexcept { return name() >= other; }
		inline bool operator==( name_t other ) const noexcept { return name() == other; }
		inline bool operator!=( name_t other ) const noexcept { return name() != other; }
		inline friend bool operator<( name_t a, const TypeTuple& b ) noexcept { return b >= a; }
		inline friend bool operator<=( name_t a, const TypeTuple& b ) noexcept { return b > a; }
		inline friend bool operator>( name_t a, const TypeTuple& b ) noexcept { return b <= a; }
		inline friend bool operator>=( name_t a, const TypeTuple& b ) noexcept { return b < a; }
		inline friend bool operator==( name_t a, const TypeTuple& b ) noexcept { return b != a; }
		inline friend bool operator!=( name_t a, const TypeTuple& b ) noexcept { return b == a; }


		// Check if this type is compatible with the other.
		// (If it is, then this can be copied to the other.)
		// If neither are tuples then same as compare!
		// If other is a tuple and this is not, then this must compare equal with the first attribute of other.
		// This tuple is compatible with the other if:
		//   1. All named attributes match on type
		//   2. All unnamed attributes match on position and type
		bool compatibleWith( const TypeTuple& other ) const noexcept;


		// Get hash value.
		size_t hash() const noexcept { size_t val = eon::hash( NameValue ); for( auto& sub : TupleValue )
			val = val * 2734561 + sub.hash(); return val; }




		///////////////////////////////////////////////////////////////////////
		//
		// Static methods
		//
	public:

		// Check if name is for a primitive type
		static inline bool isPrimitive( name_t type_name ) {
			_populatePrims(); return PrimTypes.find( type_name ) != PrimTypes.end(); }




		///////////////////////////////////////////////////////////////////////
		//
		// Helpers
		//
	private:

		inline void _ensureSubTuple() noexcept { if( NameValue != name_ttypetuple ) NameValue = name_ttypetuple; }

		inline void _assertInRange( index_t attribute_num ) const {
			if( attribute_num >= TupleValue.size() ) throw type::NotFound(); }

		void _toStr( Stringifier& str, bool top_level ) const;

		static void _populatePrims();
		static void _populateBasics();




		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	protected:

		// A TypeTuple's value can be a single name or a sub-tuple (not both!).
		// If the latter, the name value will be "#typetuple".
		// If no value at all, the name value will be eon::no_name.
		name_t NameValue{ no_name };

		// If sub-tuple, then we have a set of attributes = TypeTuples, and
		// these can be both unnamed and named.
		std::vector<TypeTuple> TupleValue;
		std::unordered_map<name_t, index_t> NamedAttributes;

		static std::unordered_set<name_t> PrimTypes, BasicTypes;
	};

	struct TypeTupleLt
	{
		inline bool operator()( const TypeTuple* lhs, const TypeTuple* rhs ) const noexcept { return *lhs < *rhs; }
	};
}

namespace std
{
	// Allow implicit use of [eon::TypeTuple] as key/value when used in
	// containers such as 'std::unordered_map' and 'std::unordered_set'.
	template<>
	struct hash<::eon::TypeTuple> {
		inline size_t operator()( const ::eon::TypeTuple& obj ) const { return obj.hash(); }
	};
	template<>
	struct equal_to<::eon::TypeTuple> {
		inline bool operator()( const ::eon::TypeTuple& a, const ::eon::TypeTuple& b ) const { return a == b; }
	};
}
