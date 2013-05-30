#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ZWD");
    BOOST_CHECK(GetBoolArg("-ZWD"));
    BOOST_CHECK(GetBoolArg("-ZWD", false));
    BOOST_CHECK(GetBoolArg("-ZWD", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ZWDo"));
    BOOST_CHECK(!GetBoolArg("-ZWDo", false));
    BOOST_CHECK(GetBoolArg("-ZWDo", true));

    ResetArgs("-ZWD=0");
    BOOST_CHECK(!GetBoolArg("-ZWD"));
    BOOST_CHECK(!GetBoolArg("-ZWD", false));
    BOOST_CHECK(!GetBoolArg("-ZWD", true));

    ResetArgs("-ZWD=1");
    BOOST_CHECK(GetBoolArg("-ZWD"));
    BOOST_CHECK(GetBoolArg("-ZWD", false));
    BOOST_CHECK(GetBoolArg("-ZWD", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noZWD");
    BOOST_CHECK(!GetBoolArg("-ZWD"));
    BOOST_CHECK(!GetBoolArg("-ZWD", false));
    BOOST_CHECK(!GetBoolArg("-ZWD", true));

    ResetArgs("-noZWD=1");
    BOOST_CHECK(!GetBoolArg("-ZWD"));
    BOOST_CHECK(!GetBoolArg("-ZWD", false));
    BOOST_CHECK(!GetBoolArg("-ZWD", true));

    ResetArgs("-ZWD -noZWD");  // -ZWD should win
    BOOST_CHECK(GetBoolArg("-ZWD"));
    BOOST_CHECK(GetBoolArg("-ZWD", false));
    BOOST_CHECK(GetBoolArg("-ZWD", true));

    ResetArgs("-ZWD=1 -noZWD=1");  // -ZWD should win
    BOOST_CHECK(GetBoolArg("-ZWD"));
    BOOST_CHECK(GetBoolArg("-ZWD", false));
    BOOST_CHECK(GetBoolArg("-ZWD", true));

    ResetArgs("-ZWD=0 -noZWD=0");  // -ZWD should win
    BOOST_CHECK(!GetBoolArg("-ZWD"));
    BOOST_CHECK(!GetBoolArg("-ZWD", false));
    BOOST_CHECK(!GetBoolArg("-ZWD", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ZWD=1");
    BOOST_CHECK(GetBoolArg("-ZWD"));
    BOOST_CHECK(GetBoolArg("-ZWD", false));
    BOOST_CHECK(GetBoolArg("-ZWD", true));

    ResetArgs("--noZWD=1");
    BOOST_CHECK(!GetBoolArg("-ZWD"));
    BOOST_CHECK(!GetBoolArg("-ZWD", false));
    BOOST_CHECK(!GetBoolArg("-ZWD", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", "eleven"), "eleven");

    ResetArgs("-ZWD -bar");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", "eleven"), "");

    ResetArgs("-ZWD=");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", "eleven"), "");

    ResetArgs("-ZWD=11");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", "eleven"), "11");

    ResetArgs("-ZWD=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ZWD", 0), 0);

    ResetArgs("-ZWD -bar");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ZWD=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ZWD=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ZWD");
    BOOST_CHECK_EQUAL(GetBoolArg("-ZWD"), true);

    ResetArgs("--ZWD=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ZWD", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noZWD");
    BOOST_CHECK(!GetBoolArg("-ZWD"));
    BOOST_CHECK(!GetBoolArg("-ZWD", true));
    BOOST_CHECK(!GetBoolArg("-ZWD", false));

    ResetArgs("-noZWD=1");
    BOOST_CHECK(!GetBoolArg("-ZWD"));
    BOOST_CHECK(!GetBoolArg("-ZWD", true));
    BOOST_CHECK(!GetBoolArg("-ZWD", false));

    ResetArgs("-noZWD=0");
    BOOST_CHECK(GetBoolArg("-ZWD"));
    BOOST_CHECK(GetBoolArg("-ZWD", true));
    BOOST_CHECK(GetBoolArg("-ZWD", false));

    ResetArgs("-ZWD --noZWD");
    BOOST_CHECK(GetBoolArg("-ZWD"));

    ResetArgs("-noZWD -ZWD"); // ZWD always wins:
    BOOST_CHECK(GetBoolArg("-ZWD"));
}

BOOST_AUTO_TEST_SUITE_END()
