#pragma once

#include "RxDefs.h"
#include "RxData.h"
#include "Node.h"
#include "CharGroup.h"
#include "OpOr.h"


/******************************************************************************
  The 'eon' namespace encloses all public functionality
******************************************************************************/
namespace eon
{
	/**************************************************************************
	  The 'eon::rx' namespace enclosed special elements for Eon regular
	  expressions
	**************************************************************************/
	namespace rx
	{
		class Graph
		{
		public:
			Graph() = default;
			inline Graph( const Graph& other ) { *this = other; }
			inline Graph( Graph&& other ) noexcept { *this = std::move( other ); }
			virtual ~Graph() { clear(); };

			Graph& operator=( const Graph& other );
			Graph& operator=( Graph&& other ) noexcept;

			inline void clear() noexcept { if( Head != nullptr ) { delete Head; Head = nullptr; } }

			void parse( const substring& source );

			inline Flag flags() const noexcept { return MyFlags; }

			inline bool empty() const noexcept { return Head == nullptr; }

			inline bool match( RxData& param ) const { return Head != nullptr ? Head->match( param ) : false; }

			inline const substring& source() const noexcept { return Source; }
			inline const substring& expressionSub() const noexcept { return ExprSub; }
			inline const substring& flagsSub() const noexcept { return FlagsSub; }
			inline char_t separator() const noexcept { return Separator; }

			inline string strStruct() const { return Head ? Head->strStruct() : string(); }

		private:
			void parse();

			void parseFlags( string::iterator& last );

			class ParseParam
			{
			public:
				ParseParam() = delete;
				inline ParseParam( const ParseParam& other ) {
					Copy = true; GroupCount = other.GroupCount; Source = other.Source; CurPos = other.CurPos; }
				ParseParam( ParseParam&& ) = delete;
				inline ParseParam( const substring& source ) { Source = source; CurPos = Source.begin();}
				~ParseParam() { if( !Copy && HeadNode != nullptr ) delete HeadNode; }

				inline operator bool() const noexcept { return static_cast<bool>( CurPos && CurPos != Source.end() ); }

				inline char_t operator()() noexcept { return *CurPos; }
				inline bool advance() noexcept { return ++CurPos; }
				inline string_iterator pos() const noexcept { return CurPos; }
				inline void pos( const string_iterator& pos ) noexcept { CurPos = pos; }

				inline void startGroup() noexcept { ++GroupCount; }
				inline bool inGroup() const noexcept { return GroupCount > 0; }
				inline void endGroup() noexcept { --GroupCount; }

				inline const substring& source() const noexcept { return Source; }

				inline size_t strSize() const noexcept { return Source.numChars(); }

				void set( Node* node ) noexcept;
				inline Node* head() const noexcept { return HeadNode; }
				inline Node* cur() const noexcept { return CurNode; }
				void replaceCur( Node* node ) noexcept;
				inline Node* extract() noexcept { auto node = HeadNode; HeadNode = nullptr; CurNode = nullptr; return node; }

			private:
				substring Source;
				string_iterator CurPos;
				int GroupCount{ 0 };
				Node* HeadNode{ nullptr };
				Node* PrevNode{ nullptr };
				Node* CurNode{ nullptr };
				bool Copy{ false };
			};

			// Returns true if end character was seen (and advanced past)
			// Returns false if no end character (set or seen)
			bool parse( ParseParam& param, int64_t end = -1 );

			Node* parseNode( ParseParam& param );

			Node* parseNot( ParseParam& param, const string_iterator& start );
			Node* parseOr( OpOr* node_or, ParseParam& param, const string_iterator& start );

			Node* parseCharGroup( ParseParam& param, const string_iterator& start );
			void parseCharGroupRange( ParseParam& param, CharGroup::CharGrp& value, char_t start );

			Node* parseCaptureRelated( ParseParam& param, const string_iterator& start );
			Node* parseCapture( ParseParam& param, const string_iterator& start );
			Node* parseBackref( ParseParam& param, const string_iterator& start );

			Node* parseEscaped( ParseParam& param, const string_iterator& start );

			bool parseQuantifier( ParseParam& param, size_t& min, size_t& max, const string_iterator& start );

			Node* parseNodeGroup( ParseParam& param, const string_iterator& start );

			Node* parseFixed( ParseParam& param, const string_iterator& start );

			Node* setQuantifier( ParseParam& param, size_t min, size_t max );

			void removeDuplicates();
			void countMinCharsRemaining();




		private:
			substring Source;
			substring ExprSub, FlagsSub;
			Node* Head{ nullptr };
			Flag MyFlags{ Flag::none };
			char_t Separator{ '/' };
		};
	}
}
