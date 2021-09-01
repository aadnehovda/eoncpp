#include "Parser.h"


namespace eon
{
	parser::parser( const std::vector<string>& args ) noexcept
	{
		try
		{
			_parseArgs( args );
		}
		catch( exception& e )
		{
			std::cerr << "ERROR: " << e.details().stdstr() << "\n";
			State = statecode::error;
		}
	}




	int parser::run() noexcept
	{
		switch( State )
		{
			case statecode::error:
				return 4;
			case statecode::usage:
				_showUsage();
				return 0;
			default:
				try
				{
					return _run();
				}
				catch( exception& e )
				{
					std::cerr << "ERROR: " << e.details().stdstr() << "\n";
					return 4;
				}
		}
	}



	void parser::_showUsage()
	{
		std::cout << "Eon Parser Usage\n";
		std::cout << App.fpath().base() + " <input> <output> <how>\n";
		std::cout << "  input=<source>  : The source can be a file path, a directory path,\n";
		std::cout << "                    (ending in a slash), a directory path (optionally\n";
		std::cout << "                    ending in slash) followed by '::' and a regular\n";
		std::cout << "                    expression identifying files within it, or or a\n";
		std::cout << "                    comma-separated list of any combinations of the\n";
		std::cout << "                    above. Directories witout a pattern won't be\n";
		std::cout << "                    scanned, but will be available for EOF speci-\n";
		std::cout << "                    fied scanning.\n";
		std::cout << "  output=<dir>    : Directory where to write output file(s).\n";
		std::cout << "  how=<eof>       : Path to an EOF file that defines how to do the\n";
		std::cout << "                    parsing.\n";
	}
	void parser::_parseArgs( const std::vector<string>& args )
	{
		App = args[ 0 ];
		for( size_t i = 1; i < args.size(); ++i )
		{
			auto& arg = args[ i ];
			if( arg == "--help" || arg == "-h" || arg == "help" )
				State = statecode::usage;
			else if( arg.startsWith( "input=" ) )
				_parseInput( arg.substr( arg.begin() + 6 ) );
			else if( arg.startsWith( "output=" ) )
				OutputDir = arg.substr( arg.begin() + 7 );
			else if( arg.startsWith( "how=" ) )
				Docs.loadMerge( path( arg.substr( arg.begin() + 4 ) ) );
			else
			{
				std::cerr << "Unknown argument: " << arg << "\n";
				State = statecode::error;
			}
		}
	}
	void parser::_parseInput( const substring& input )
	{
		auto sources = input.splitSequential<std::vector<string>>( ',' );
		for( auto& source : sources )
		{
/*			auto pos = source.findFirst( "::" );
			if( pos )
			{
				path dir = source.substr( source.begin(), pos.begin() );
				dir.noEndSlash();
				regex pattern = source.substr( pos.end() );
				InputDirs.insert( dir );
				ScanDirs.push( std::make_pair(
					std::move( dir ), std::move( pattern ) ) );
			}
			else*/
			{
				path item = source;
				if( item.str().endsWith( "/" ) )
					InputDirs.insert( item );
				else
					InputFiles.push( item );
			}
		}
	}

	int parser::_run()
	{
auto str = Docs.doc( name::get( "docgen_cpp" ) ).str();
		_validateDocs();
		_findDocument();
		if( Doc == nullptr )
		{
			std::cerr << "ERROR: Got no named EOF document with the "
				"'eonparser' name specified in the document metadata!\n";
			return 4;
		}
		_showInfo();
		while( !InputFiles.empty() )
		{
			auto file = InputFiles.front();
			InputFiles.pop();
			std::cout << " - " << file.fpath().stdstr() << "\n";
			_parse( file );
		}
		return 0;
	}

	void parser::_validateDocs()
	{
		// TODO: Include pattern document!
		for( size_t i = 0; i < Docs.numDocs(); ++i )
		{
			auto name = Docs.name( i );
			if( name != no_name )
				Docs.validate( name );
		}
	}
	void parser::_findDocument()
	{
		for( size_t i = 0; i < Docs.numDocs(); ++i )
		{
			auto name = Docs.name( i );
			if( name == no_name )
				continue;
			auto meta = Docs.metadata( name );
			if( !meta )
				continue;
			if( !meta->containsUnnamedValue( eon::name::get( "eonparser" ) ) )
				continue;
			Doc = &Docs.doc( i );
		}
	}

	void parser::_showInfo()
	{
		std::cout << "Parsing ";
		std::cout << Doc->find( { name::get( "source_type" ) } )->actualString();
		std::cout << " files, generating ";
		std::cout << Doc->find( { name_output } )->actualString();
		std::cout << " output inside \"";
		std::cout << OutputDir.dpath().stdstr() << "\".\n";
	}

	void parser::_parse( const file& input_file )
	{
		source src( input_file.fpath(), input_file.loadText() );
		_parse( src );
	}
	void parser::_parse( source& src )
	{
		auto tokens = tokenizer()( src );
		tokenparser tp( std::move( tokens ) );
		auto& scan = Doc->find( { name_scan } )->tuple_value();
		_parseStructure( tp, scan );
	}
	bool parser::_parseStructure( tokenparser& tp, const tuple& structure )
	{
		Docs.variables().set( name_token, vars::valueptr( new vars::stringval( tp.current().substr() ) ) );
		auto start = tp.pos();
		for( size_t i = 0; i < structure.numAttributes(); ++i )
		{
			auto name = structure.name( i );
			if( name == no_name )
			{
				if( structure.at( i )->isReference() )
				{
					auto target = dynamic_cast<vars::refval*>( &*structure.at( i ) )->target();
					return _parseStructure( tp, target->actualTuple() );
				}
				if( _parseExplicit( tp, structure.at( i ) ) )
					return true;
			}
			else
			{
				if( name == name_loop )
				{
					if( _loop( tp, structure.metadata( name ), structure.at( i )->tuple_value() ) )
						return true;
				}
			}
			tp.pos( start );
		}
		return false;
	}

	bool parser::_parseExplicit( tokenparser& tp, vars::valueptr element )
	{
		switch( element->type() )
		{
			case vars::type_code::char_t:
				return _match( tp, element->targetChar( Docs.variables() ) );
			case vars::type_code::int_t:
				return _match( tp, element->targetInt( Docs.variables() ) );
			case vars::type_code::float_t:
				return _match( tp, element->targetFloat( Docs.variables() ) );
			case vars::type_code::name_t:
				return _match( tp, element->targetName( Docs.variables() ) );
			case vars::type_code::string_t:
			case vars::type_code::var_t:
			case vars::type_code::expr_t:
				return _match( tp, element->targetString( Docs.variables() ) );
			case vars::type_code::meta_t:
				return _executeMeta( tp, element->targetMeta( Docs.variables() ) );
		}
		return false;
	}
	bool parser::_match( tokenparser& tp, char_t val )
	{
		auto sub = tp.current().substr();
		if( sub.numChars() == 1 && *sub.begin() == val )
		{
			tp.forward();
			return true;
		}
		return false;
	}
	bool parser::_match( tokenparser& tp, int64_t val )
	{
		auto sub = tp.current().substr();
		if( sub.toInt64() == val )
		{
			tp.forward();
			return true;
		}
		return false;
	}
	bool parser::_match( tokenparser& tp, double val )
	{
		auto sub = tp.current().substr();
		if( sub.toDouble() == val )
		{
			tp.forward();
			return true;
		}
		return false;
	}
	bool parser::_match( tokenparser& tp, name_t val )
	{
		auto sub = tp.current().substr();
		if( sub.compare( val->substr() ) == 0 )
		{
			tp.forward();
			return true;
		}
		return false;
	}
	bool parser::_match( tokenparser& tp, const string& val )
	{
		auto sub = tp.current().substr();
		if( sub.compare( val.substr() ) == 0 )
		{
			tp.forward();
			return true;
		}
		return false;
	}
	bool parser::_executeMeta( tokenparser& tp, const tuple& val )
	{
		bool optional = false;
		const expression* condition{ nullptr };
		string id;
		name_t func{ no_name };
		vars::valueptr argument;
		for( size_t i = 0; i < val.numAttributes(); ++i )
		{
			auto attribute = val.at( i );
			auto name = val.name( i );
			if( name == no_name )
			{
				if( attribute->isName() && attribute->actualName() == name_optional )
					optional = true;
			}
			else if( name == name_match || name == name_skip )
			{
				func = name;
				argument = attribute;
			}
			else if( name == name_condition )
				condition = &attribute->targetExpression( Docs.variables() );
		}
		auto cond = condition == nullptr ? true : condition->evaluate( Docs.variables() )->targetBool( Docs.variables() );
		if( !cond )
			return false;
		if( func == name_match )
			return _executeMatch( tp, val, argument, optional );
		else if( func == name_skip )
			return _executeSkip( tp, val, argument, optional );
		else
			return false;
	}
	bool parser::_executeMatch( tokenparser& tp, const tuple& val, const vars::valueptr& argument, bool optional )
	{
		auto pos = tp.pos();
		bool matched = false;
		switch( argument->type() )
		{
			case vars::type_code::ref_t:
				matched = _matchReferenced( tp, argument->actualReference() );
				break;
			case vars::type_code::tuple_t:
				matched = _matchAlternates( tp, argument->actualTuple() );
				break;
		}
		if( !matched && optional )
		{
			tp.pos( pos );
			return true;
		}
		return matched;
	}
	bool parser::_executeSkip( tokenparser& tp, const tuple& val, const vars::valueptr& argument, bool optional )
	{
		auto pos = tp.pos();
		bool matched = false;
		switch( argument->type() )
		{
			case vars::type_code::tuple_t:
				matched = _skipAll( tp, argument->actualTuple() );
				break;
		}
		if( !matched && optional )
		{
			tp.pos( pos );
			return true;
		}
		return matched;
	}
	bool parser::_matchAlternates( tokenparser& tp, const tuple& alternates )
	{
		for( size_t i = 0; i < alternates.numAttributes(); ++i )
		{
			auto attribute = alternates.at( i );
			switch( attribute->type() )
			{
				case vars::type_code::ref_t:
					if( _matchReferenced( tp, attribute->actualReference() ) )
						return true;
			}
		}
		return false;
	}
	bool parser::_matchReferenced( tokenparser& tp, const nameref& ref )
	{
		auto target = Doc->find( ref );
		if( !target || !target->isTuple() )
			return false;
		auto structure = target->actualTuple().at( name_structure );
		if( !structure || !structure->isTuple() )
			return false;
		if( !_parseStructure( tp, structure->actualTuple() ) )
			return false;
		if( ref.last() == name_include )
			_handleIncludeInput( target->actualTuple().at( name_locate )->actualTuple() );
		return true;
	}
	bool parser::_skipAll( tokenparser& tp, const tuple& elements )
	{
		while( true )
		{
			bool skip = false;
			for( size_t i = 0; i < elements.numAttributes(); ++i )
			{
				auto attribute = elements.at( i );
				switch( attribute->type() )
				{
					case vars::type_code::name_t:
					{
						auto name = attribute->actualName();
						if( name == name_space )
							skip = tp.skipSpaces();
						break;
					}
					case vars::type_code::char_t:
					{
						if( tp.current().substr().numChars() == 1
							&& *tp.current().substr().begin() == attribute->actualChar() )
							skip = true;
						break;
					}
				}
			}
			if( !skip )
				break;
		}
		return true;
	}

	bool parser::_loop( tokenparser& tp, const tupleptr metadata,
		const tuple& structure )
	{
		if( !metadata )
			return false;

		// Get loop details
		const expression* condition{ nullptr };
		for( size_t i = 0; i < metadata->numAttributes(); ++i )
		{
			auto what = metadata->at( i )->targetName( Docs.variables() );
			if( what == name_while )
				condition = &metadata->at( ++i )->targetExpression( Docs.variables() );
		}

		if( condition )
		{
			while( tp && condition->evaluate( Docs.variables() )->targetBool( Docs.variables() ) )
			{
				_parseStructure( tp, structure );

				tp.forward();
				if( !tp )
					return true;
				Docs.variables().set( name_token, vars::valueptr( new vars::stringval( tp.current().substr() ) ) );
			}
		}
		return true;
	}

	void parser::_handleIncludeInput( const tuple& locate )
	{
		for( size_t i = 0; i < locate.numAttributes(); ++i )
		{
			auto name = locate.name( i );
			if( name != no_name )
			{
				auto& meta = locate.metadata( name );
				if( meta->containsUnnamedValue( name_section ) )
					_handleIncludeInput( locate.at( i )->actualTuple() );
				else
					_execute( name, *meta, locate.at( i )->actualTuple() );
			}
		}
	}
	void parser::_execute( name_t action, const tuple& args, const tuple& block )
	{
		if( action == name_if )
			_executeIf( args, block );
	}
	void parser::_executeIf( const tuple& args, const tuple& block )
	{
		static name_t fexists = name::get( "file_exists" );

		auto func = args.at( 0 )->targetName( Docs.variables() );
		bool condition = false;

		if( func == fexists )
			condition = path( args.at( 1 )->targetExpression( Docs.variables() ).evaluate( Docs.variables() )
				->targetString( Docs.variables() ) ).exists();

		if( !condition )
			return;

		for( size_t i = 0; i < block.numAttributes(); ++i )
		{
			auto name = block.name( i );
			
		}
	}
}
