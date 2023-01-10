#include "Locale.h"
#include <eoninlinetest/InlineTest.h>


namespace eon
{
	thread_local locale locale::CurLocale;



	EON_TEST( locale, get, basic,
		EON_EQ( "en_US.utf8", locale::get().name() ) );

	EON_TEST_2STEP( locale, set, basic,
		locale::set( locale( "no_NB.utf8" ) ),
		EON_EQ( "no_NB.utf8", locale::get().name() ) );

	EON_TEST( locale, decimalSep, standard,
		EON_EQ( char_t( '.' ), locale::get().decimalSep() ) );
	EON_TEST( locale, decimalSep, no_NB,
		EON_EQ( char_t( ',' ), locale( "no_NB.utf8" ).decimalSep() ) );

	EON_TEST( locale, thousandsSep, standard,
		EON_EQ( char_t( ',' ), locale::get().thousandsSep() ) );
	EON_TEST( locale, thousandsSep, no_NB,
		EON_EQ( NBSpaceChr, locale( "no_NB.utf8" ).thousandsSep() ) );

	EON_TEST( locale, grouping, standard,
		EON_EQ( "\3", locale::get().grouping() ) );
	EON_TEST( locale, grouping, no_NB,
		EON_EQ( "\3", locale( "no_NB.utf8" ).grouping() ) );




	EON_TEST( locale, toUpper, standard_1,
		EON_EQ( L'A', locale::get().toUpper( L'a' ) ) );
	EON_TEST( locale, toUpper, standard_2,
		EON_EQ( L'Á', locale::get().toUpper( L'á' ) ) );
	EON_TEST( locale, toUpper, no_NB_1,
		EON_EQ( L'A', locale::get().toUpper( L'a' ) ) );
	EON_TEST( locale, toUpper, no_NB_2,
		EON_EQ( L'Å', locale::get().toUpper( L'å' ) ) );

	EON_TEST( locale, toLower, standard_1,
		EON_EQ( L'a', locale::get().toLower( L'A' ) ) );
	EON_TEST( locale, toLower, standard_2,
		EON_EQ( L'á', locale::get().toLower( L'Á' ) ) );
	EON_TEST( locale, toLower, no_NB_1,
		EON_EQ( L'a', locale::get().toLower( L'A' ) ) );
	EON_TEST( locale, toLower, no_NB_2,
		EON_EQ( L'å', locale::get().toLower( L'Å' ) ) );




	EON_TEST( locale, decimalSepMoney, standard,
		EON_EQ( char_t( '.' ), locale::get().decimalSepMoney() ) );
	EON_TEST( locale, decimalSepMoney, no_NB,
		EON_EQ( char_t( ',' ), locale( "no_NB.utf8" ).decimalSepMoney() ) );

	EON_TEST( locale, thousandsSepMoney, standard,
		EON_EQ( char_t( ',' ), locale::get().thousandsSepMoney() ) );
	EON_TEST( locale, thousandsSepMoney, no_NB,
		EON_EQ( NBSpaceChr, locale( "no_NB.utf8" ).thousandsSepMoney() ) );

	EON_TEST( locale, groupingMoney, standard,
		EON_EQ( "\3", locale::get().groupingMoney() ) );
	EON_TEST( locale, groupingMoney, no_NB,
		EON_EQ( "\3", locale( "no_NB.utf8" ).groupingMoney() ) );

	EON_TEST( local, currencySymbol, standard,
		EON_EQ( L"$", locale::get().currencySymbol() ) );
	EON_TEST( locale, currencySymbol, no_NB,
		EON_EQ( string( char_t( 0xA4 ) ), locale( "no_NB.utf8" ).currencySymbol() ) );

	EON_TEST( locale, positiveSign, standard,
		EON_EQ( L"", locale::get().positiveSign() ) );
	EON_TEST( locale, positiveSign, no_NB,
		EON_EQ( L"", locale( "no_NB.utf8" ).positiveSign() ) );

	EON_TEST( locale, negativeSign, standard,
		EON_EQ( L"-", locale::get().negativeSign() ) );
	EON_TEST( locale, negativeSign, no_NB,
		EON_EQ( L"-", locale( "no_NB.utf8" ).negativeSign() ) );

	EON_TEST( locale, fractionalDigits, standard,
		EON_EQ( 2, locale::get().fractionalDigits() ) );
	EON_TEST( locale, fractionalDigits, no_NB,
		EON_EQ( 2, locale( "no_NB.utf8" ).fractionalDigits() ) );




	void locale::_set( std::string name )
	{
		Loc = std::locale( name );
		Coll = &std::use_facet<std::collate<wchar_t>>( Loc );
		Num = &std::use_facet<std::numpunct<wchar_t>>( Loc );
		CType = &std::use_facet<std::ctype<wchar_t>>( Loc );
		Money = &std::use_facet<std::moneypunct<wchar_t>>( Loc );
	}
}
