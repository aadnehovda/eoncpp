#pragma once

#include "Action.h"
#include "Operators.h"
#include "Tuple.h"
#include "TypeTuple.h"



///////////////////////////////////////////////////////////////////////////////
//
// The 'eon' namespace encloses all public functionality
//
namespace eon
{
	///////////////////////////////////////////////////////////////////////////
	//
	// The 'type' namespace enclosing all background type functionality
	//
	namespace type
	{
		struct StringConstruct : public Action {
			inline StringConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				values.push( Attribute::newExplicit( string(), name_string ) ); return sig_t::norm; }
		};
		struct StringCopyConstruct : public Action {
			inline StringCopyConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_string } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<string>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringByteConstruct : public Action {
			inline StringByteConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_byte } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<byte_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringCharConstruct : public Action {
			inline StringCharConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_char } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<char_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringIntConstruct : public Action {
			inline StringIntConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_int } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<int_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringShortConstruct : public Action {
			inline StringShortConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_short } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<short_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringLongConstruct : public Action {
			inline StringLongConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_long } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<long_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm;
			}
		};
		struct StringFloatConstruct : public Action {
			inline StringFloatConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_float } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<flt_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringLowConstruct : public Action {
			inline StringLowConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_low } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<low_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringHighConstruct : public Action {
			inline StringHighConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_high } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<high_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringIndexConstruct : public Action {
			inline StringIndexConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_index } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<index_t>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringNameConstruct : public Action {
			inline StringNameConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_name } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<name_t>(); values.pop(); values.push(
					Attribute::newExplicit( eon::str( a1 ), name_string ) ); return sig_t::norm; }
		};
		struct StringBytesConstruct : public Action {
			inline StringBytesConstruct() : Action( TypeTuple::action( name_string, name_constructor, name_operator, name_string,
				{ { no_name, name_bytes } } ) ) {}
			inline sig_t operator()( stack<Attribute>& values ) const override {
				auto a1 = values.top().value<std::string>(); values.pop(); values.push(
					Attribute::newExplicit( string( a1 ), name_string ) ); return sig_t::norm; }
		};

		struct StringCompare : public Action {
			inline StringCompare() : Action( TypeTuple::action( name_string, symbol_cmp, name_operator, name_int,
				{ { no_name, name_string } } ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override;
		};

		struct StringReset : public Action {
			inline StringReset() : Action( TypeTuple::action( name_string, name_reset, name_operator, name_string, {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<string>(); a1.clear(); return sig_t::norm; }
		};

		struct StringLength : public Action {
			inline StringLength() : Action( TypeTuple::action( name_string, name_length, name_firstarg, name_index, {} ) ) {}
			sig_t operator()( stack<Attribute>& values ) const override {
				auto& a1 = values.top().value<string>(); a1.clear(); return sig_t::norm; }
		};

		void registerString();
	}
}
