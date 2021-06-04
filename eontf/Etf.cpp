#include "Etf.h"
#include <eontokenizer/Tokenizer.h>


namespace eon
{
	bool etf::validate( name_t document, name_t pattern_document ) const
	{
		auto doc = Docs.attribute( document );
		if( !doc || !doc->isTuple() )
			return false;

		doc->tuple_value().validate();

		if( pattern_document != no_name )
		{
			if( Patterns.find( pattern_document ) == Patterns.end() )
				throw tup::Invalid( "The specified pattern document \""
					+ *pattern_document
					+ "\" is not a \"pattern document\"!" );
			auto ptrn = Docs.attribute( pattern_document );
			if( !ptrn )
				throw tup::Invalid( "Pattern document not available: "
					+ *pattern_document );
			if( ptrn->isTuple() && ptrn->tuple_value() )
				ptrn->tuple_value().validate( doc->tuple_value() );
		}

		return true;
	}




	void etf::load( const path& file )
	{
		textfilereader reader( file );
		auto name = eon::name::get( file.baseWithoutExt() );
		source src( file.str(), reader.read( SIZE_MAX ) );
		_parse( name, src );
	}
	void etf::parse( const string& str, name_t document_name )
	{
		source src( *document_name, string( str ) );
		_parse( document_name, src );
	}




	void etf::_parse( name_t name, source& src )
	{
		tf::Parser parser{ sourceref( src ) };
		while( true )
		{
			// Get document name and meta data - if any
			auto result = parser.parseDocumentStart();
			if( !result )
				break;
			auto doc_name = result.attribute( 0 )->name_value();
			auto doc_meta = result.attribute( 1 )->meta_value();

			// Use given name if no name in header
			if( doc_name == no_name )
				doc_name = name;

			// Prepare meta data
			tupleptr meta;
			if( doc_meta )
				meta = tupleptr( new tuple( std::move( doc_meta ) ) );

			// If this is a pattern document, we need to record it as such
			if( meta && meta->containsUnnamedValue( name_pattern ) )
				Patterns.insert( doc_name );

			// Create (empty) tuple with these details
			Docs.append( tup::valueptr( new tup::tupleval() ),
				doc_name, meta );
			auto& doc = Docs.attribute(
				Docs.numAttributes() - 1 )->tuple_value();

			// Keep on parsing until end of document
			while( parser.parseDocumentAttribute( doc ) )
				;
		}
	}
}