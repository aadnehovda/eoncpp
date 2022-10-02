#pragma once

#include "Definitions.h"
#include "Attribute.h"
#include "TypeMap.h"
#include "Action.h"



///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	// Tuple permissions
	enum class TuplePerm : uint8_t
	{
		// No permissions
		none = 0x00,
		
		// Attributes can be added
		add = 0x01,

		// Attributes can be removed
		remove = 0x02,

		// Attributes can be modified
		modify = 0x04,

		// Tuple can register types
		types = 0x08,

		// Tuple can register actions
		actions = 0x10,

		// All normal tuple permissions
		all_normal = add | remove | modify,
	};
	inline bool operator&( TuplePerm a, TuplePerm b ) noexcept {
		return static_cast<bool>( static_cast<int>( a ) & static_cast<int>( b ) ); }
	inline TuplePerm operator|( TuplePerm a, TuplePerm b ) noexcept {
		return static_cast<TuplePerm>( static_cast<int>( a ) | static_cast<int>( b ) ); }



	// Tuple attribute pair
	class AttributePair
	{
	public:
		AttributePair() = default;
		inline AttributePair( name_t name, Attribute&& value ) noexcept { Name = name; Value = std::move( value ); }
		inline AttributePair( Attribute&& value ) noexcept { Value = std::move( value ); }
		inline AttributePair( const AttributePair& other ) noexcept { Name = other.Name, Value = other.Value; }
		inline AttributePair( AttributePair&& other ) noexcept {
			Name = other.Name; other.Name = no_name; Value = std::move( other.Value ); }
		~AttributePair() = default;

	public:
		name_t Name{ no_name };
		Attribute Value;
	};




	///////////////////////////////////////////////////////////////////////////
	//
	// Eon Tuple class - eon::Tuple
	//
	// Generic tuple class
	//
	class Tuple
	{
		///////////////////////////////////////////////////////////////////////
		//
		// Construction
		//
	public:

		// Construct a void tuple - a place-holder
		Tuple() { _populateData(); }

		// Construct an empty tuple
		// If tuple type is 'plain', 'dynamic', or 'data' then the permissions
		// argument is ignored and permissions set to default for that tuple type!
		inline Tuple( name_t tuple_type_name, TuplePerm permissions = TuplePerm::all_normal ) {
			_populateData(); Type = TypeTuple{ { no_name, name_tuple }, { name_type, tuple_type_name } };
			_perm( tuple_type_name, permissions ); }

		// Construct tuple with named and unnamed attributes
		// If tuple type is 'plain', 'dynamic', or 'data' then the permissions
		// argument is ignored and permissions set to default for that tuple type!
		Tuple( name_t tuple_type_name, std::initializer_list<AttributePair> attributes,
			TuplePerm permissions = TuplePerm::all_normal );

		// Construct tuple with named and unnamed attributes.
		// If tuple type is 'plain', 'dynamic', or 'data' then the permissions
		// argument is ignored and permissions set to default for that tuple type!
		Tuple( name_t tuple_type_name, std::vector<AttributePair>&& attributes,
			TuplePerm permissions = TuplePerm::all_normal );

		// Copy, without duplicating objects
		inline Tuple( const Tuple& other ) { *this = other; }

		// Take ownership of other tuple
		inline Tuple( Tuple&& other ) noexcept { *this = std::move( other ); }

		~Tuple() = default;




		///////////////////////////////////////////////////////////////////////
		//
		// General modifiers
		//
	public:

		// Copy, without duplicating objects
		Tuple& operator=( const Tuple& other );

		// Transfer ownership
		Tuple& operator=( Tuple&& other ) noexcept;


		// Clear
		// Throws:
		//   - eon::type::AccessDenied : No 'remove' permission
		void clear() noexcept {
			_assertRemPerm(); Attributes.clear(); NamedAttributes.clear(); Actions.clear(); ActionsByName.clear(); }




		///////////////////////////////////////////////////////////////////////
		//
		// General information
		//
	public:

		// Use tuple as 'bool'.
		inline operator bool() const noexcept { return !empty(); }

		// Check if tuple is empty (has not attributes and no actions).
		inline bool empty() const noexcept { return Attributes.empty() && Actions.empty(); }

		
		// Get tuple type.
		inline const TypeTuple& type() const noexcept { return Type; }


		// Get parent, nullptr if none.
		inline Tuple* parent() noexcept { return Parent; }


		// Check attributes can be added to tuple.
		inline bool canAdd() const noexcept { return Permissions & TuplePerm::add; }

		// Check if attributes can be removed from tuple.
		inline bool canRemove() const noexcept { return Permissions & TuplePerm::remove; }

		// Check if attributes in tuple can be modified.
		inline bool canModify() const noexcept { return Permissions & TuplePerm::modify; }

		// Check if tuple can contain actions.
		inline bool canHaveActions() const noexcept { return Permissions & TuplePerm::actions; }


		// Check if this tuple is compatible with another tuple
		bool compatibleWith( const Tuple& other ) const noexcept;


		// Get object as string representation
		// Rules for data tuples:
		//   1. '(' immediately after tuple type identifier, no newline. Example: "data("
		//   2. Value of tuple-attribute with key shall follow indented on next line. Example: "value:\n  (a, b)"
		//   3. ')' to be added after last item without newlines. Example: "data(sub:\n  (a, b))"
		//   4. Long lines shall be split as follows:
		//      1. Split on ',' for highest possible attribute level
		//      2. Split on '='
		//      3. Split bytes/string values on space with a backslash at the end of the split line
		void str( Stringifier& strf ) const;
		inline string str() const { Stringifier strf; str( strf ); return strf.generateString(); }




		///////////////////////////////////////////////////////////////////////
		//
		// Adding and removing attributes
		//
	public:

		// Add a void unnamed attribute of specified type
		// Throws:
		//   - eon::type::AccessDenied  : No 'add' permission or value not legal for data tuple
		Tuple& add( const TypeTuple& type );

		// Add a void named attribute of specified type
		// Throws:
		//   - eon::type::AccessDenied  : No 'add' permission or value not legal for data tuple
		//   - eon::type::DuplicateName : Name has already been used
		Tuple& add( name_t name, const TypeTuple& type );

		// Add an unnamed tuple attribute
		// Throws:
		//   - eon::type::AccessDenied  : No 'add' permission or value not legal for data tuple
		inline Tuple& add( Tuple&& value ) { return add( Attribute::newTuple( std::move( value ) ) ); }

		// Add a named tuple attribute
		// Throws:
		//   - eon::type::AccessDenied  : No 'add' permission or value not legal for data tuple
		//   - eon::type::DuplicateName : Name has already been used
		inline Tuple& add( name_t name, Tuple&& value ) { return add( name, Attribute::newTuple( std::move( value ) ) ); }

		// Add an unnamed attribute
		// Throws:
		//   - eon::type::AccessDenied  : No 'add' permission or value not legal for data tuple
		Tuple& add( Attribute&& value );

		// Add a named attribute
		// Throws:
		//   - eon::type::AccessDenied  : No 'add' permission or value not legal for data tuple
		//   - eon::type::DuplicateName : Name has already been used
		Tuple& add( name_t name, Attribute&& value );

		// Add an unnamed attribute of implicit type
		// Throws:
		//   - eon::type::AccessDenied  : No 'add' permission or value not legal for data tuple
		template<typename T>
		Tuple& add( T&& value, type::hint type_hint = type::hint::none )
		{
			TypeTuple type( type::mapCppType( CPPTYPE( T ) ) );
			_assertAddPerm( Type.at( name_type ).nameValue(), type );
			Attributes.push_back( Attribute::newImplicit( std::move( value ), type_hint ) );
			return *this; }

		// Add a named attribute of implicit type
		// Throws:
		//   - eon::type::AccessDenied  : No 'add' permission or value not legal for data tuple
		//   - eon::type::DuplicateName : Name has already been used
		template<typename T>
		Tuple& add( name_t name, T&& value, type::hint type_hint = type::hint::none )
		{
			TypeTuple type( type::mapCppType( CPPTYPE( T ), type_hint ) );
			if( name != no_name )
			{
				if( exists( name ) )
					throw type::DuplicateName( "Cannot add tuple attribute \"" + eon::str( name ) + "\" to tuple!" );
				_assertAddPerm( Type.at( name_type ).nameValue(), type );
				NamedAttributes.insert( name, Attributes.size() );
			}
			Attributes.push_back( Attribute( std::move( value ) ) );
			return *this;
		}

		// Remove an attribute by position, negative counts from end
		// Throws:
		//   - eon::type::NotFound     : No such attribute
		//   - eon::type::AccessDenied : No 'remove' permission
		Tuple& remove( index_t attribute_num );
		Tuple& remove( int_t attribute_num );

		// Remove an attribute by name
		// Throws:
		//   - eon::type::NotFound     : No such attribute
		//   - eon::type::AccessDenied : No 'remove' permission
		inline Tuple& remove( name_t attribute_name ) { return remove( index( attribute_name ) ); }




		///////////////////////////////////////////////////////////////////////
		//
		// Accessing attributes
		//
	public:

		// Get number of attributes.
		inline index_t numAttributes() const noexcept { return Attributes.size(); }


		// Check if an attribute exists at the specified position.
		inline bool exists( index_t attribute_num ) const noexcept { return attribute_num < Attributes.size(); }

		// Check if an attribute with the specified name exists.
		inline bool exists( name_t attribute_name ) const noexcept {
			return NamedAttributes.findOne( attribute_name ) != NamedAttributes.endOne(); }


		// Get type of attribute at specified position, void empty/false type tuple if no such attribute!
		TypeTuple attributeType( index_t attribute_num ) const noexcept {
			return exists( attribute_num ) ? Attributes[ attribute_num ].type() : TypeTuple(); }

		// Get type of attribute with specified name, void empty/false type tuple if no such attribute!
		TypeTuple attributeType( name_t attribute_name ) const noexcept {
			return exists( attribute_name ) ? attributeType( index( attribute_name ) ) : TypeTuple();}


		// Get attribute value by position, mutable access
		// Throws eon::type::NotFound if no such attribute!
		// Throws eon::type::AccessDenied if no 'modify' permission!
		// Throws eon::type::WrongType if attribute has a different type!
		template<typename T, type::hint type_hint = type::hint::none>
		inline T& at( index_t attribute_num ) { _assertModPerm(); _assertInRange( attribute_num ); return
			Attributes[ attribute_num ].value<T, type_hint>(); }

		// Get attribute value by name, mutable access
		// Throws eon::type::NotFound if no such attribute!
		// Throws eon::type::AccessDenied if no 'modify' permission!
		// Throws eon::type::WrongType if attribute has a different type!
		template<typename T, type::hint type_hint = type::hint::none>
		inline T& at( name_t attribute_name ) { _assertModPerm(); auto pos = index( attribute_name ); _assertInRange(
			pos ); return Attributes[ pos ].value<T, type_hint>(); }

		// Get attribute value by position, immutable access.
		// Throws eon::type::NotFound if no such attribute!
		// Throws eon::type::WrongType if attribute has a different type!
		template<typename T, type::hint type_hint = type::hint::none>
		inline const T& at( index_t attribute_num ) const {
			_assertInRange( attribute_num ); return Attributes[ attribute_num ].value<T, type_hint>(); }

		// Get attribute value by position, immutable access.
		// Throws eon::type::NotFound if no such attribute!
		// Throws eon::type::WrongType if attribute has a different type!
		template<typename T, type::hint type_hint = type::hint::none>
		inline const T& read( index_t attribute_num ) const { return at<T, type_hint>( attribute_num ); }

		// Get attribute value by position, immutable access.
		// Throws eon::type::NotFound if no such attribute!
		// Throws eon::type::WrongType if attribute has a different type!
		template<typename T, type::hint type_hint = type::hint::none>
		inline const T& read( int attribute_num ) const { return at<T, type_hint>( static_cast<index_t>( attribute_num ) ); }

		// Get attribute value by name, immutable access.
		// Throws eon::type::NotFound if no such attribute!
		// Throws eon::type::WrongType if attribute has a different type!
		template<typename T, type::hint type_hint = type::hint::none>
		inline const T& at( name_t attribute_name ) const {
			auto pos = index( attribute_name ); _assertInRange( pos ); return Attributes[ pos ].value<T, type_hint>(); }

		// Get attribute value by name, immutable access.
		// Throws eon::type::NotFound if no such attribute!
		// Throws eon::type::WrongType if attribute has a different type!
		template<typename T, type::hint type_hint = type::hint::none>
		inline const T& read( name_t attribute_name ) const { return at<T, type_hint>( attribute_name ); }


		// Get full attribute at specified position, mutable access.
		// Throws eon::type::NotFound if no such attribute!
		Attribute& attribute( index_t attribute_num ) {
			_assertInRange( attribute_num ); return Attributes[ attribute_num ]; }
		
		// Get full attribute with specified name, mutable access.
		// Throws eon::type::NotFound if no such attribute!
		Attribute& attribute( name_t attribute_name ) {
			auto pos = index( attribute_name ); _assertInRange( pos ); return Attributes[ pos ]; }
		
		// Get full attribute at specified position, immutable access.
		// Throws eon::type::NotFound if no such attribute!
		const Attribute& attribute( index_t attribute_num ) const {
			_assertInRange( attribute_num ); return Attributes[ attribute_num ]; }
		
		// Get full attribute with specified name, immutable access.
		// Throws eon::type::NotFound if no such attribute!
		const Attribute& attribute( name_t attribute_name ) const {
			auto pos = index( attribute_name ); _assertInRange( pos ); return Attributes[ pos ]; }
		

		// Check if an unnamed name attribute (flag) with the specified value exists.
		bool flag( name_t name ) const noexcept;


		// Get index for an attribute name, eon::no_index if no such attribute!
		inline index_t index( name_t attribute_name ) const noexcept {
			auto found = NamedAttributes.findOne( attribute_name );
			return found != NamedAttributes.endOne() ? found->second : no_index; }

		// Get attribute name given an index, eon::no_name if no such attribute or unnamed attribute!
		inline name_t name( index_t attribute_num ) const noexcept {
			auto found = NamedAttributes.findTwo( attribute_num );
			return found != NamedAttributes.endTwo() ? found->second : no_name; }




		///////////////////////////////////////////////////////////////////////
		//
		// Types
		//
	public:

		// Register a type.
		// Throws eon::type::AccessDenied if no 'types' permission!
		// Throws eon::type::DuplicateName if type has already been registered!
		Tuple& registerType( name_t type );

		// Check if specified name is a registered type
		bool isType( name_t name ) const noexcept;




		///////////////////////////////////////////////////////////////////////
		//
		// Actions
		//
	public:

		// Add a reference to an action to this tuple.
		// Throws eon::type::AccessDenied if no 'actions' permission!
		// Throws eon::type::DuplicateName if action has already been registered!
		Tuple& registerAction( type::Action* action );


		// Get an action based on signature.
		// Returns nullptr if not found.
		inline const type::Action* action( const TypeTuple& signature ) const noexcept {
			if( Parent ) return Parent->action( signature );
			auto found = Actions.find( signature ); return found != Actions.end() ? found->second : nullptr; }

		// Get signatures of all actions with the specified name, for any type and arguments
		const std::set<TypeTuple>& signatures( name_t action_name ) const;

		// Get signatures of all actions with the specified name, implemented for the specified type
		// and with the specified arguments.
		std::set<TypeTuple> signatures( name_t action_name, name_t type_name, const TypeTuple& args ) const;




		///////////////////////////////////////////////////////////////////////
		//
		// Helpers
		//
	private:

		void _perm( name_t tuple_type, TuplePerm permissions );

		void _assertAddPerm( name_t tuple_type, const TypeTuple& type ) const;
		void _assertRemPerm() const;
		void _assertModPerm() const;
		void _assertTypPerm() const;
		void _assertActPerm() const;

		void _assertInRange( index_t attribute_num ) const;

		void _add( name_t tuple_type, Attribute&& value );
		void _add( name_t tuple_type, name_t attribute_name, Attribute&& value );

		void _str( Stringifier& strf, stack<const Tuple*>& ancestors, bool named_tuple = false ) const;
		bool _strTupleStart( Stringifier& strf, stack<const Tuple*>& ancestors, bool named_tuple ) const;
		void _strValue( Stringifier& strf, const Attribute& attribute ) const;

		static inline void _populateData() { _populateLegalForDataTuple(); _populateStrPrefixPostfixForTypes(); }
		static void _populateLegalForDataTuple();
		static void _populateStrPrefixPostfixForTypes();




		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	protected:

		TypeTuple Type{ no_name };
		TuplePerm Permissions{ TuplePerm::all_normal };

		Tuple* Parent{ nullptr };

		std::vector<Attribute> Attributes;
		reflexive_map<name_t, index_t> NamedAttributes;

		std::unordered_map<TypeTuple, type::Action*> Actions;
		std::unordered_map<name_t, std::set<TypeTuple>> ActionsByName;

		static std::unordered_set<name_t> LegalForDataTuple;
		static std::unordered_map<name_t, std::pair<string, string>> StrPrefixPostfixForTypes;
	};
}
