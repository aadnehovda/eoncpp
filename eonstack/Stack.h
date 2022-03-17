#pragma once
#include <eonexcept/Exception.h>
#include <vector>


/******************************************************************************
  The 'eon' namespace encloses all public functionality
******************************************************************************/
namespace eon
{
	//* Exception thrown when trying to access a stack element that doesn't exist
	EONEXCEPT( NoElement );




	/**************************************************************************
	  Eon Stack Class - eon::stack

	  This class is like std::stack, but also grants access to elements below
	  the top, as well as iteration.
	  A std::vector is used as underlying structure.
	**************************************************************************/
	template<class T>
	class stack
	{
		/**********************************************************************
		  Construction
		**********************************************************************/
	public:

		stack() = default;
		inline stack( const stack& other ) { Data = other.Data; }
		inline stack( stack&& other ) noexcept { Data = std::move( other.Data ); }

		inline stack( std::initializer_list<T> elements ) { Data = elements; }

		virtual ~stack() = default;




		/**********************************************************************
		  Modifier Methods
		**********************************************************************/
	public:

		//* Replace elements by copying another stack
		inline stack& operator=( const stack& other ) { Data = other.Data; return *this; }

		//* Replace elements by taking ownership of the elements of another stack
		inline stack& operator=( stack&& other ) noexcept { Data = std::move( other.Data ); return *this; }

		//* Copy the elements of the other stack onto the top of this
		//* The ordering of the elements will be maintained.
		inline stack& operator+=( const stack& other ) {
			for( auto& elm : other.Data ) Data.push_back( elm ); return *this; }

		//* Take ownership of the elements of the other stack, but put on top of this
		//* The ordering of the elements will be maintained.
		inline stack& operator+=( stack&& other ) {
			for( auto& elm : other.Data ) Data.push_back( std::move( elm ) ); other.clear(); return *this; }


		//* Reserver space for a specific number of elements
		inline void reserver( size_t capacity ) { Data.reserve( capacity ); }

		//* Clear all elements
		void clear() noexcept { Data.clear(); }




		/**********************************************************************
		  Read-only Methods
		**********************************************************************/
	public:

		//* Check if empty
		inline bool empty() const noexcept { return Data.empty(); }

		//* Use as boolean
		//* Returns true if not empty!
		inline operator bool() const noexcept { return !Data.empty(); }

		//* Get number of stack elements
		inline size_t size() const noexcept { return Data.size(); }

		//* Get current capacity
		inline size_t capacity() const noexcept { return Data.capacity(); }




		/**********************************************************************
		  Stack Operations
		**********************************************************************/
	public:

		//* Push an element onto the top of the stack
		inline void push( const T& element ) { Data.push_back( element ); }
		inline void push( T&& element ) { Data.push_back( std::move( element ) ); }

		//* Access the top element
		//* Throws [eon::NoElement] if stack is empty!
		inline const T& top() const { return element( 0 ); }
		inline T& top() { return element( 0 ); }

		//* Access the bottom element
		//* Throws [eon::NoElement] if stack is empty!
		inline const T& bottom() const { return element( Data.size() - 1 ); }
		inline T& bottom() { return element( Data.size() - 1 ); }

		//* Access an element below the top
		//* An argument value of zero is the same as calling top()!
		//* Throws [eon::NoElement] if stack is empty!
		inline const T& element( size_t steps_from_top ) const {
			if( _end() >= steps_from_top ) return Data[ _end() - steps_from_top ]; else throw NoElement(); }
		inline T& element( size_t steps_from_top ) {
			if( _end() >= steps_from_top ) return Data[ _end() - steps_from_top ]; else throw NoElement(); }

		//* Pop the top element
		inline void pop() noexcept { if( !Data.empty() ) Data.resize( Data.size() - 1 ); }




		/**********************************************************************
		  Iteration
		**********************************************************************/
	public:

		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;
		using reverse_iterator = typename std::vector<T>::reverse_iterator;
		using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

		inline iterator begin() noexcept { return Data.begin(); }
		inline iterator end() noexcept { return Data.end(); }

		inline const_iterator begin() const noexcept { return Data.begin(); }
		inline const_iterator end() const noexcept { return Data.end(); }
		inline const_iterator cbegin() const noexcept { return Data.begin(); }
		inline const_iterator cend() const noexcept { return Data.end(); }

		inline reverse_iterator rbegin() noexcept { return Data.rbegin(); }
		inline reverse_iterator rend() noexcept { return Data.rend(); }

		inline const_reverse_iterator rbegin() const noexcept { return Data.rbegin(); }
		inline const_reverse_iterator rend() const noexcept { return Data.rend(); }
		inline const_reverse_iterator crbegin() const noexcept { return Data.rbegin(); }
		inline const_reverse_iterator crend() const noexcept { return Data.rend(); }




		///////////////////////////////////////////////////////////////////////
		//
		// Helpers
		//
	private:

		inline size_t _end() const noexcept { return Data.size() - 1; }



		
		///////////////////////////////////////////////////////////////////////
		//
		// Attributes
		//
	private:

		std::vector<T> Data;
	};
}