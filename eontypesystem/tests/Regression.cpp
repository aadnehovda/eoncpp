#include "Regression.h"



namespace eon
{
	TEST( TypeTupleTest, basic )
	{
		TypeTuple a{ name_bool };
		TypeTuple b{ { name( "on" ), name_bool } };
		TypeTuple c{ { no_name, { name_and, name_or } },
			{ name_tuple, { { name_action, name_float }, { no_name, name_int } } } };
		WANT_EQ( "T(bool)", a.str() );
		WANT_EQ( "T(on=bool)", b.str() );
		WANT_EQ( "T((and, or), tuple=(action=float, int))", c.str() );
	}
	TEST( TypeTupleTest, compare )
	{
		TypeTuple t1( name_bool ), t2( name_bool ), t3( name_char );

		WANT_TRUE( t1 == t2 );
		WANT_TRUE( t2 == t1 );
		WANT_FALSE( t1 == t3 );
	}
	TEST( TypeTupleTest, compatible )
	{
		TypeTuple t1{ name_bool };
		TypeTuple t2{ name_bool };
		TypeTuple t3{
			{ no_name, name_bool },
			{ no_name, name_int },
			{ no_name, name( "one" ) },
			{ name_name, name( "three" ) },
			{ no_name, name_string } };
		TypeTuple t4{
			{ name_name, name( "three" ) },
			{ no_name, name_int } };

		WANT_TRUE( t1.compatibleWith( t2 ) );
		WANT_TRUE( t2.compatibleWith( t1 ) );
		WANT_TRUE( t1.compatibleWith( t3 ) );
		WANT_FALSE( t3.compatibleWith( t1 ) );
		WANT_TRUE( t4.compatibleWith( t3 ) );
		WANT_FALSE( t3.compatibleWith( t4 ) );
	}



	TEST( TupleTest, plain_unnamed )
	{
		string chars{ u8"b�" };
		auto tup = tuple::plain( {
			{ false }, { 'b' }, { *chars.begin() }, { *chars.last() },
			{ static_cast<int_t>( -54 ) }, { static_cast<short_t>( 11 ) }, { static_cast<long_t>( 987654321 ) },
			{ static_cast<flt_t>( -11.2 ) }, { static_cast<low_t>( 11.1 ) }, { static_cast<high_t>( 999.88 ) },
			{ static_cast<index_t>( 888888888 ) },
			{ Attribute( name_action, type::hint::name ) },
			{ std::string( "bytes" ) }, { string( chars ) } } );

		REQUIRE_EXCEPT( tup.add( true ), eon::type::AccessDenied );

		TypeTuple exp_type{ { no_name, name_tuple }, { name_type, name_plain }, { name_tuple, TypeTuple( {
			name_bool, name_byte, name_char, name_char,
			name_int, name_short, name_long, name_float, name_low, name_high,
			name_index, name_name, name_bytes, name_string } ) } };

		WANT_EQ( exp_type.str(), tup.type().str() );
		REQUIRE_EQ( 14, tup.numAttributes() );
		WANT_FALSE( tup.read<bool>( 0 ) );
		WANT_EQ( 'b', ( tup.read<byte_t, type::hint::byte>( 1 ) ) );
		WANT_EQ( *chars.begin(), tup.read<char_t>( 2 ) );
		WANT_EQ( *chars.last(), tup.read<char_t>( 3 ) );
		WANT_EQ( -54, tup.read<int_t>( 4 ) );
		WANT_EQ( 11, tup.read<short_t>( 5 ) );
		WANT_EQ( 987654321, tup.read<long_t>( 6 ) );
		WANT_EQ( -11.2, tup.read<flt_t>( 7 ) );
		WANT_EQ( 11.1, tup.read<low_t>( 8 ) );
		WANT_EQ( 999.88, tup.read<high_t>( 9 ) );
		WANT_EQ( 888888888, ( tup.read<index_t, type::hint::index>( 10 ) ) );
		WANT_EQ( name_action, ( tup.read<name_t, type::hint::name>( 11 ) ) );
		WANT_EQ( "bytes", tup.read<std::string>( 12 ) );
		WANT_EQ( chars, tup.read<string>( 13 ) );
	}
	TEST( TupleTest, plain_named )
	{
		string chars{ u8"b�" };
		auto tup = tuple::plain( {
			{ name_bool, false }, { name_byte, 'b' }, { name_char, *chars.begin() },
			{ name_int, static_cast<int_t>( -54 ) },
			{ name_short, static_cast<short_t>( 11 ) }, { name_long, static_cast<long_t>( 987654321 ) },
			{ name_float, static_cast<flt_t>( -11.2 ) },
			{ name_low, static_cast<low_t>( 11.1 ) }, { name_high, static_cast<high_t>( 999.88 ) },
			{ name_index, static_cast<index_t>( 888888888 ) },
			{ name_name, Attribute( name_action, type::hint::name ) },
			{ name_bytes, std::string( "bytes" ) }, { name_string, string( chars ) } } );

		TypeTuple exp_type{ { no_name, name_tuple }, { name_type, name_plain }, { name_tuple, TypeTuple( {
			{ name_bool, name_bool }, { name_byte, name_byte }, { name_char, name_char },
			{ name_int, name_int }, { name_short, name_short }, { name_long, name_long },
			{ name_float, name_float }, { name_low, name_low }, { name_high, name_high },
			{ name_index, name_index }, { name_name, name_name },
			{ name_bytes, name_bytes }, { name_string, name_string } } ) } };

		WANT_EQ( exp_type.str(), tup.type().str() );
		REQUIRE_EQ( 13, tup.numAttributes() );
		WANT_FALSE( tup.read<bool>( name_bool ) );
		WANT_EQ( 'b', tup.read<byte_t>( name_byte ) );
		WANT_EQ( *chars.begin(), tup.read<char_t>( name_char ) );
		WANT_EQ( -54, tup.read<int_t>( name_int ) );
		WANT_EQ( 11, tup.read<short_t>( name_short ) );
		WANT_EQ( 987654321, tup.read<long_t>( name_long ) );
		WANT_EQ( -11.2, tup.read<flt_t>( name_float ) );
		WANT_EQ( 11.1, tup.read<low_t>( name_low ) );
		WANT_EQ( 999.88, tup.read<high_t>( name_high ) );
		WANT_EQ( 888888888, tup.read<index_t>( name_index ) );
		WANT_EQ( name_action, ( tup.read<name_t, type::hint::name>( name_name ) ) );
		WANT_EQ( "bytes", tup.read<std::string>( name_bytes ) );
		WANT_EQ( chars, tup.read<string>( name_string ) );
	}
	TEST( TupleTest, plain_str_1 )
	{
		auto tup = tuple::plain( { { Attribute( name_and, type::hint::name ) },
			{ Attribute( name_or, type::hint::name ) } } );
		auto act = tup.str();
		string exp{ "p(and, or)" };
		WANT_EQ( exp, act );
	}
	TEST( TupleTest, plain_str_2 )
	{
		auto tup = tuple::plain( { { name( "one" ), Attribute( name_and, type::hint::name ) },
			{ name("two" ), Attribute( name_or, type::hint::name ) } } );
		auto act = tup.str();
		string exp{ "p(one=and, two=or)" };
		WANT_EQ( exp, act );
	}
	TEST( TupleTest, plain_str_3 )
	{
		auto tup = tuple::plain( {
			{ Tuple( name_plain,
				{ { static_cast<byte_t>( 'a' ) }, { string( "string" ) } } ) },
			{ name( "second" ), Tuple( name_plain, {
				{ Tuple( name_plain, { { Attribute( name_and, type::hint::name ) },
					{ Attribute( name_or, type::hint::name ) } } ) },
				{ name( "another" ), Tuple( name_plain, { { Attribute( name_action, type::hint::name ) },
					{ Attribute( name_at, type::hint::name ) } } ) } } ) } } );
		auto act = tup.str();
		string exp{
			"p(\n"
			"  (B'a', \"string\"),\n"
			"  second:\n"
			"    (and, or),\n"
			"    another:\n"
			"      action, at)"
		};
		WANT_EQ( exp, act );
	}
	TEST( TupleTest, plain_compare )
	{
		auto tup = tuple::plain( {
			{ Tuple( name_plain,
				{ { static_cast<byte_t>( 'a' ) }, { string( "string" ) } } ) },
			{ name( "second" ), Tuple( name_plain, {
				{ Tuple( name_plain, { { Attribute( name_and, type::hint::name ) },
					{ Attribute( name_or, type::hint::name ) } } ) },
				{ name( "another" ), Tuple( name_plain, { { Attribute( name_action, type::hint::name ) },
					{ Attribute( name_at, type::hint::name ) } } ) } } ) } } );
		auto act = tup.str();
		string exp{
			"p(\n"
			"  (B'a', \"string\"),\n"
			"  second:\n"
			"    (and, or),\n"
			"    another:\n"
			"      action, at)"
		};
		WANT_EQ( exp, act );
	}




	TEST( ExpressionTest, minimal )
	{
		// The smallest expression is one containing only a single operand
		string str{ "9.9" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		REQUIRE_EQ( 1, result.size() );
		REQUIRE_TRUE( result[ 0 ].type() == name_float );
		WANT_EQ( 9.9, result[ 0 ].value<flt_t>() );
	}

	TEST( ExpressionTest, literal_bool )
	{
		string str{ "true;false" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool }, error ) ) << error;
		WANT_TRUE( result[ 0 ].value<bool>() );
		WANT_FALSE( result[ 1 ].value<bool>() );
	}
	TEST( ExpressionTest, literal_byte_char )
	{
		string str{ "B'x';'X'" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_byte, name_char }, error ) ) << error;
		WANT_EQ( 'x', (result[ 0 ].value<byte_t, type::hint::byte>() ) );
		WANT_EQ( 'X', result[ 1 ].value<char_t>() );
	}
	TEST( ExpressionTest, literal_numbers )
	{
		string str{ "55;-99;3.22;-12.3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_int, name_int, name_float, name_float }, error ) ) << error;
		WANT_EQ( 55, result[ 0 ].value<int_t>() );
		WANT_EQ( -99, result[ 1 ].value<int_t>() );
		WANT_EQ( 3.22, result[ 2 ].value<flt_t>() );
		WANT_EQ( -12.3, result[ 3 ].value<flt_t>() );
	}
	TEST( ExpressionTest, literal_names )
	{
		string str{ "operator;#a" };
		scope::global().add( name( "a" ), 99, type::hint::name );
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_name, name_name }, error ) ) << error;
		WANT_EQ( name_operator, ( result[ 0 ].value<name_t, type::hint::name>() ) );
		WANT_EQ( name( "a" ), ( result[ 1 ].value<name_t, type::hint::name>() ) );
	}
	TEST( ExpressionTest, literal_bytes )
	{
		string str{ "B\"bytes\";B\"more bytes\"" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bytes, name_bytes }, error ) ) << error;
		WANT_EQ( "bytes", result[ 0 ].value<std::string>() );
		WANT_EQ( "more bytes", result[ 1 ].value<std::string>() );
	}
	TEST( ExpressionTest, literal_string )
	{
		string str{ "\"string\";\"another string\"" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_string, name_string }, error ) ) << error;
		WANT_EQ( "string", result[ 0 ].value<string>() );
		WANT_EQ( "another string", result[ 1 ].value<string>() );
	}
	TEST( ExpressionTest, literal_regex )
	{
		// Single literal expressions
		string str{ "r\"\\d+\\w+\"" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_regex }, error ) ) << error;
		WANT_EQ( "\\d+\\w+", result[ 0 ].value<regex>().str() );
	}
	TEST( ExpressionTest, literal_namepath )
	{
		// Single literal expressions
		string str{ "@one;@one/two/three" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_namepath, name_namepath }, error ) ) << error;
		WANT_EQ( "@one", result[ 0 ].value<namepath>().str() );
		WANT_EQ( "@one/two/three", result[ 1 ].value<namepath>().str() );
	}
	TEST( ExpressionTest, literal_path )
	{
		// Single literal expressions
		string str{ "p\"C:/Program Files/eon\"" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_path }, error ) ) << error;
		WANT_EQ( "C:/Program Files/eon", result[ 0 ].value<path>().str() );
	}
	TEST( ExpressionTest, literal_plaintuple )
	{
		string str{ "p(1,2);p( '2' , x  = \"x\" )" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_tuple, name_tuple }, error ) ) << error;
		WANT_EQ( "p(1, 2)", result[ 0 ].value<Tuple>().str() );
		WANT_EQ( "p('2', x=\"x\")", result[ 1 ].value<Tuple>().str() );
	}
	TEST( ExpressionTest, literal_dynamictuple )
	{
		string str{ "dynamic(1,2)" };
		Expression expr;
		REQUIRE_EXCEPT( expr = Expression( std::move( str ), scope::global() ), InvalidExpression );
	}
	TEST( ExpressionTest, literal_datatuple )
	{
		string str{ "data(1,2)" };
		Expression expr;
		REQUIRE_EXCEPT( expr = Expression( std::move( str ), scope::global() ), InvalidExpression );
	}
	TEST( ExpressionTest, literal_typetuple )
	{
		string str{ "T(int,float);T(x=int, y=int, z=int);T((int, int),named=(int, int))" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_typetuple, name_typetuple, name_typetuple }, error ) ) << error;
		WANT_EQ( "T(int, float)", result[ 0 ].value<TypeTuple>().str() );
		WANT_EQ( "T(x=int, y=int, z=int)", result[ 1 ].value<TypeTuple>().str() );
		WANT_EQ( "T((int, int), named=(int, int))", result[ 2 ].value<TypeTuple>().str() );
	}
	TEST( ExpressionTest, literal_expression )
	{
		string str{ "e(2*3);e(1.1  +  2.2)" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_expression, name_expression }, error ) ) << error;
		WANT_EQ( "2 * 3", result[ 0 ].value<Expression>().str() );
		WANT_EQ( "1.1 + 2.2", result[ 1 ].value<Expression>().str() );
	}

	TEST( ExpressionTest, basic )
	{
		// The four basic arithmetic operators
		string str{ "1 + 2; 3 - 4; 5 * 6; 8 / 2" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, {name_int, name_int, name_int, name_int }, error ) ) << error;
		WANT_EQ( 3, result[ 0 ].value<int_t>() );
		WANT_EQ( -1, result[ 1 ].value<int_t>() );
		WANT_EQ( 30, result[ 2 ].value<int_t>() );
		WANT_EQ( 4, result[ 3 ].value<int_t>() );
	}
	TEST( ExpressionTest, name_operators )
	{
		// The four basic arithmetic operators
		string str{ "5 mod 3; 3 pow 4" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_int, name_int }, error ) ) << error;
		WANT_EQ( 2, result[ 0 ].value<int_t>() );
		WANT_EQ( 81, result[ 1 ].value<int_t>() );
	}
	TEST( ExpressionTest, basic_var1 )
	{
		// Variable as operand
		string str{ "3 + a" };
		scope::global().add( name( "a" ), int_t( 4 ) );
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_int }, error ) ) << error;
		WANT_EQ( 7, result[ 0 ].value<int_t>() );
	}
	TEST( ExpressionTest, basic_var2 )
	{
		// Variables on left side of arithmetic assignment
		string str{ "a += 2; b -= 3; c *= 4; d /= 3" };
		scope::global().add( name( "a" ), int_t( 4 ) );
		scope::global().add( name( "b" ), int_t( 4 ) );
		scope::global().add( name( "c" ), int_t( 4 ) );
		scope::global().add( name( "d" ), int_t( 6 ) );
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_int, name_int, name_int, name_int }, error ) ) << error;
		WANT_EQ( 6, result[ 0 ].value<int_t>() );
		WANT_EQ( 1, result[ 1 ].value<int_t>() );
		WANT_EQ( 16, result[ 2 ].value<int_t>() );
		WANT_EQ( 2, result[ 3 ].value<int_t>() );
	}
	TEST( ExpressionTest, parenthesized )
	{
		// Using parenthesis to change precedence
		string str{ "(1 + 2) * 3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_int }, error ) ) << error;
		WANT_EQ( 9, result[ 0 ].value<int_t>() );
	}

	TEST( ExpressionTest, int_cmp )
	{
		// Basic less-than for int (which implements all comparison operators).
		string str{ "3 <=> 4; 3 <=> 3; 4 <=> 3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_int, name_int, name_int }, error ) ) << error;
		WANT_EQ( -1, result[ 0 ].value<int_t>() );
		WANT_EQ( 0, result[ 1 ].value<int_t>() );
		WANT_EQ( 1, result[ 2 ].value<int_t>() );
	}
	TEST( ExpressionTest, int_lt )
	{
		// Basic less-than for int (which implements all comparison operators).
		string str{ "3 < 4; 3 < 3; 4 < 3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_TRUE( result[ 0 ].value<bool>() );
		WANT_FALSE( result[ 1 ].value<bool>() );
		WANT_FALSE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, int_le )
	{
		// Basic less-than-or-equal-to for int (which implements all comparison operators).
		string str{ "3 <= 4; 3 <= 3; 4 <= 3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_TRUE( result[ 0 ].value<bool>() );
		WANT_TRUE( result[ 1 ].value<bool>() );
		WANT_FALSE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, int_gt )
	{
		// Basic greater-than for int (which implements all comparison operators).
		string str{ "3 > 4; 3 > 3; 4 > 3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_FALSE( result[ 0 ].value<bool>() );
		WANT_FALSE( result[ 1 ].value<bool>() );
		WANT_TRUE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, int_ge )
	{
		// Basic greater-than-or-equal-to for int (which implements all comparison operators).
		string str{ "3 >= 4; 3 >= 3; 4 >= 3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_FALSE( result[ 0 ].value<bool>() );
		WANT_TRUE( result[ 1 ].value<bool>() );
		WANT_TRUE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, int_eq )
	{
		// Basic equal-to for int (which implements all comparison operators).
		string str{ "3 == 4; 3 == 3; 4 == 3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_FALSE( result[ 0 ].value<bool>() );
		WANT_TRUE( result[ 1 ].value<bool>() );
		WANT_FALSE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, int_ne )
	{
		// Basic not-equal-to for int (which implements all comparison operators).
		string str{ "3 != 4; 3 != 3; 4 != 3" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_TRUE( result[ 0 ].value<bool>() );
		WANT_FALSE( result[ 1 ].value<bool>() );
		WANT_TRUE( result[ 2 ].value<bool>() );
	}

	TEST( ExpressionTest, float_lt )
	{
		// Basic less-than for float (which only implements the <=> operator).
		string str{ "3.0 < 4.0; 3.0 < 3.0; 4.0 < 3.0" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_TRUE( result[ 0 ].value<bool>() );
		WANT_FALSE( result[ 1 ].value<bool>() );
		WANT_FALSE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, float_le )
	{
		// Basic less-than-or-equal-to for float (which only implements the <=> operator).
		string str{ "3.0 <= 4.0; 3.0 <= 3.0; 4.0 <= 3.0" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_TRUE( result[ 0 ].value<bool>() );
		WANT_TRUE( result[ 1 ].value<bool>() );
		WANT_FALSE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, float_gt )
	{
		// Basic greater-than for float (which only implements the <=> operator).
		string str{ "3.0 > 4.0; 3.0 > 3.0; 4.0 > 3.0" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_FALSE( result[ 0 ].value<bool>() );
		WANT_FALSE( result[ 1 ].value<bool>() );
		WANT_TRUE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, float_ge )
	{
		// Basic greater-than-or-equal-to for float (which only implements the <=> operator).
		string str{ "3.0 >= 4.0; 3.0 >= 3.0; 4.0 >= 3.0" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_FALSE( result[ 0 ].value<bool>() );
		WANT_TRUE( result[ 1 ].value<bool>() );
		WANT_TRUE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, float_eq )
	{
		// Basic equal-to than for float (which only implements the <=> operator).
		string str{ "3.0 == 4.0; 3.0 == 3.0; 4.0 == 3.0" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_FALSE( result[ 0 ].value<bool>() );
		WANT_TRUE( result[ 1 ].value<bool>() );
		WANT_FALSE( result[ 2 ].value<bool>() );
	}
	TEST( ExpressionTest, float_ne )
	{
		// Basic not-equal-to for float (which only implements the <=> operator).
		string str{ "3.0 != 4.0; 3.0 != 3.0; 4.0 != 3.0" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_bool, name_bool, name_bool }, error ) ) << error;
		WANT_TRUE( result[ 0 ].value<bool>() );
		WANT_FALSE( result[ 1 ].value<bool>() );
		WANT_TRUE( result[ 2 ].value<bool>() );
	}

	TEST( ExpressionTest, bits_element )
	{
		// Element operator on int (get bit value)
		string str{ "b8(3)[0];b8(3)[1];b8(3)[2];b8(3)[3]" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_b8, name_b8, name_b8, name_b8 }, error ) ) << error;
		WANT_EQ( 1, ( result[ 0 ].value<b8_t, type::hint::bits>() ) );
		WANT_EQ( 1, ( result[ 1 ].value<b8_t, type::hint::bits>() ) );
		WANT_EQ( 0, ( result[ 2 ].value<b8_t, type::hint::bits>() ) );
		WANT_EQ( 0, ( result[ 3 ].value<b8_t, type::hint::bits>() ) );
	}
	TEST( ExpressionTest, int_slice )
	{
		// Slice operator on int (get bits)
		string str{ "b8(85)[2:-2]" };
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		string error;
		REQUIRE_TRUE( validateResult( result, { name_b8 }, error ) ) << error;
		WANT_EQ( 5, (result[ 0 ].value<b8_t, type::hint::bits>() ) );
	}

	TEST( ExpressionTest, int_reset )
	{
		string str{ "reset a" };
		scope::global().add( name( "a" ), int_t( 4 ) );
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		auto value = scope::global().at<int_t>( name( "a" ) );
		WANT_EQ( 0, value );
	}
	TEST( ExpressionTest, string_reset )
	{
		string str{ "reset a" };
		scope::global().add( name( "a" ), string( "Value" ) );
		Expression expr;
		REQUIRE_NO_EXCEPT( expr = Expression( std::move( str ), scope::global() ) );
		std::vector<Attribute> result;
		REQUIRE_NO_EXCEPT( result = expr.execute() );
		auto value = scope::global().at<string>( name( "a" ) );
		WANT_EQ( "", value );
	}
}