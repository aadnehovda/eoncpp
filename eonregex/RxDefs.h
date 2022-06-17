#pragma once
#include <vector>
#include <eonexcept/Exception.h>
#include <eonstring/Name.h>



namespace eon
{
	namespace rx
	{
		enum class NodeType
		{
			undef,
			op_or,
			op_not,
			node_group,
			capt_group,
			loc_start,
			loc_end,
			loc_wordstart,
			loc_wordend,
			val_fixed,
			val_chargroup,
			val_backslash,
			val_dquote,
			val_any,
			val_wordchar,
			val_lower,
			val_upper,
			val_digit,
			val_space,
			val_punctuation,
			val_escaped,
			val_backref
		};

		enum class Anchor
		{
			none = 0x00,
			input = 0x01,
			line = 0x02,
			word = 0x04
		};
		inline bool operator&( Anchor a, Anchor b ) noexcept { return static_cast<int>( a ) & static_cast<int>( b ); }
		inline Anchor& operator|=( Anchor& a, Anchor b ) noexcept {
			return a = static_cast<Anchor>( static_cast<int>( a ) | static_cast<int>( b ) ); }
		inline Anchor operator|( Anchor a, Anchor b ) noexcept {
			return static_cast<Anchor>( static_cast<int>( a ) | static_cast<int>( b ) ); }

		static const size_t nsize{ SIZE_MAX };

		enum class Flag : int16_t
		{
			none = 0x00,
			icase = 0x01,		// Ignore case
			lines = 0x02,		// Make ^ and $ apply to lines
			speed = 0x04,		// Maximize speed (for long and complex expressions)
			accuracy = 0x08,	// Maximize accuracy (for long and complex expressions)

			no_ungroup = 0x10,				// Do not remove superfluous groupings
			no_exposing = 0x20,				// Do not expose literal characters
			
			failfast_fixed_end = 0x040,		// When <fixed>$ at the end and not 'lines', check end of input first
		};
		inline bool operator&( Flag a, Flag b ) noexcept { return static_cast<int>( a ) & static_cast<int>( b ); }
		inline Flag& operator|=( Flag& a, Flag b ) noexcept {
			return a = static_cast<Flag>( static_cast<int>( a ) | static_cast<int>( b ) ); }
		inline Flag operator|( Flag a, Flag b ) noexcept {
			return static_cast<Flag>( static_cast<int>( a ) | static_cast<int>( b ) ); }

		class RecordedPos
		{
		public:
			RecordedPos() = default;
			inline RecordedPos( const RecordedPos& other ) noexcept { Pos = other.Pos; Marker = other.Marker; }
			inline RecordedPos& operator=( const RecordedPos& other ) noexcept {
				Pos = other.Pos; Marker = other.Marker; return *this; }
			
			inline bool same( string::iterator pos, int marker ) const noexcept { return Marker == marker && Pos == pos; }
			inline void mark( string::iterator pos, int marker ) noexcept { Marker = marker; Pos = pos; }

		private:
			string::iterator Pos;
			int Marker{ 0 };
		};


		///////////////////////////////////////////////////////////////////////
		//
		// Exception used when a regular expression is badly formatted
		//
		EONEXCEPT( InvalidExpression );
	}
}
