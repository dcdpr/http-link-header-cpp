#include "http-link-header.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


static std::string header_previousChapter = // NOLINT(cert-err58-cpp)
        R"(<https://example.com/TheBook/chapter2>; rel="previous"; title="previous chapter")";

static std::string header_nextChapter = // NOLINT(cert-err58-cpp)
        R"(<https://example.com/TheBook/chapter4>; rel="next"; title="next chapter")";

TEST_CASE("parsing empty header string returns empty vector") {
    CHECK(http_link_header::parse("").empty());
}

TEST_CASE("parsing Link header string returns vector with one Link") {
    CHECK(http_link_header::parse(header_previousChapter).size() == 1);
}

TEST_CASE("parsing Link header string returns vector with two Links") {
    std::string header = header_previousChapter + ',' + header_nextChapter;
    CHECK(http_link_header::parse(header).size() == 2);
}



TEST_CASE("parsing empty vector of header strings returns empty vector of Links") {
    std::vector<std::string> headers;
    CHECK(http_link_header::parse(headers).empty());
}

TEST_CASE("parsing vector of header strings with one Link header returns vector with one Link") {
    std::vector<std::string> headers;
    headers.push_back(header_previousChapter);
    CHECK(http_link_header::parse(headers).size() == 1);
}

TEST_CASE("parsing vector of header strings with one Link header string returns vector with two Links") {
    std::string header = header_previousChapter + ',' + header_nextChapter;
    std::vector<std::string> headers;
    headers.push_back(header);
    CHECK(http_link_header::parse(headers).size() == 2);
}

TEST_CASE("parsing vector of header strings with two Link header strings returns vector with four Links") {
    std::string header = header_previousChapter + ',' + header_nextChapter;
    std::vector<std::string> headers;
    headers.push_back(header);
    headers.push_back(header);
    CHECK(http_link_header::parse(headers).size() == 4);
}

TEST_CASE("create Link objects with subparts") {

    http_link_header::Link link{"a", "b", "c", {}};

    CHECK(link.linkContext == "a");
    CHECK(link.linkRelation == "b");
    CHECK(link.linkTarget == "c");

}

TEST_CASE("check equality of TargetAttributes") {
    http_link_header::TargetAttribute a1{"a", "b"};
    http_link_header::TargetAttribute a2{"a", "b"};
    http_link_header::TargetAttribute a3{"a", "c"};

    CHECK(a1 == a2);
    CHECK(a1 != a3);
}

TEST_CASE("check equality of Links") {
    http_link_header::Link link1{"a", "b", "c", {}};
    http_link_header::Link link2{"a", "b", "c", {}};
    http_link_header::Link link3{"d", "e", "f", {}};

    CHECK(link1 == link2);
    CHECK(link1 != link3);
}

TEST_CASE("parse quoted value, unquoted string") {
    std::string input = "bye";
    CHECK(http_link_header::parseQuotedString(input) == "");
    CHECK(input == "bye");
}

TEST_CASE("parse quoted value, quoted string") {
    std::string input = "\"hello\"";
    CHECK(http_link_header::parseQuotedString(input) == "hello");
    CHECK(input.empty());
}

TEST_CASE("parse quoted value, embedded escaped quotes") {
    std::string input = R"("one \"two\" three")";
    CHECK(http_link_header::parseQuotedString(input) == "one \"two\" three");
    CHECK(input.empty());
}


TEST_CASE("parse header, check parts, test1") {
    auto links = http_link_header::parse(header_previousChapter);

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext.empty());
    CHECK(links[0].linkRelation == "previous");
    CHECK(links[0].linkTarget == "https://example.com/TheBook/chapter2");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "title");
    CHECK(links[0].targetAttributes[0].value == "previous chapter");
}

TEST_CASE("parse header, check parts, test2") {
    auto links = http_link_header::parse(header_nextChapter);

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext.empty());
    CHECK(links[0].linkRelation == "next");
    CHECK(links[0].linkTarget == "https://example.com/TheBook/chapter4");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "title");
    CHECK(links[0].targetAttributes[0].value == "next chapter");
}

TEST_CASE("empty link target") {
    auto links = http_link_header::parse(R"(<>; rel="start")");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "start");
    CHECK(links[0].linkTarget == "");

    CHECK(links[0].targetAttributes.empty());
}

TEST_CASE("quoted parameter values") {
    auto links = http_link_header::parse(R"(<http://example.org>; foo="bar"; bar="foo")");

    CHECK(links.size() == 1);

    CHECK(links[0].targetAttributes.size() == 2);
    CHECK(links[0].targetAttributes[0].name == "foo");
    CHECK(links[0].targetAttributes[0].value == "bar");
    CHECK(links[0].targetAttributes[1].name == "bar");
    CHECK(links[0].targetAttributes[1].value == "foo");
}

TEST_CASE("unquoted parameter values") {
    auto links = http_link_header::parse(R"(<http://example.org>; foo=bar; bar=foo)");

    CHECK(links.size() == 1);

    CHECK(links[0].targetAttributes.size() == 2);
    CHECK(links[0].targetAttributes[0].name == "foo");
    CHECK(links[0].targetAttributes[0].value == "bar");
    CHECK(links[0].targetAttributes[1].name == "bar");
    CHECK(links[0].targetAttributes[1].value == "foo");
}

TEST_CASE("mixed quoted parameter values, unquoted-quoted") {
    auto links = http_link_header::parse(R"(<http://example.org>; foo=bar; bar="foo")");

    CHECK(links.size() == 1);

    CHECK(links[0].targetAttributes.size() == 2);
    CHECK(links[0].targetAttributes[0].name == "foo");
    CHECK(links[0].targetAttributes[0].value == "bar");
    CHECK(links[0].targetAttributes[1].name == "bar");
    CHECK(links[0].targetAttributes[1].value == "foo");
}

TEST_CASE("mixed quoted parameter values, quoted-unquoted") {
    auto links = http_link_header::parse(R"(<http://example.org>; foo="bar"; bar=foo)");

    CHECK(links.size() == 1);

    CHECK(links[0].targetAttributes.size() == 2);
    CHECK(links[0].targetAttributes[0].name == "foo");
    CHECK(links[0].targetAttributes[0].value == "bar");
    CHECK(links[0].targetAttributes[1].name == "bar");
    CHECK(links[0].targetAttributes[1].value == "foo");
}

TEST_CASE("empty link target") {
    auto links = http_link_header::parse(R"(<>; rel="start")");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "start");
    CHECK(links[0].linkTarget == "");

    CHECK(links[0].targetAttributes.empty());
}

TEST_CASE("resolve uri, test 1") {
    std::string baseUri = "https://a/b/c/d;p?q";
    std::string uriToResolve = "g";
    std::string result;

    CHECK(http_link_header::uri::resolve(&baseUri, &uriToResolve, &result));
    CHECK(result == "https://a/b/c/g");
}

TEST_CASE("resolve uri, test 2") {
    std::string baseUri = "https://a/b/c/d;p?q";
    std::string uriToResolve = "g/";
    std::string result;

    CHECK(http_link_header::uri::resolve(&baseUri, &uriToResolve, &result));
    CHECK(result == "https://a/b/c/g/");
}

TEST_CASE("resolve uri, test 3") {
    std::string baseUri = "https://a/b/c/d;p?q";
    std::string uriToResolve = "/g";
    std::string result;

    CHECK(http_link_header::uri::resolve(&baseUri, &uriToResolve, &result));
    CHECK(result == "https://a/g");
}

TEST_CASE("resolve uri, test 4") {
    std::string baseUri = "https://a/b/c/d;p?q";
    std::string uriToResolve = "#s";
    std::string result;

    CHECK(http_link_header::uri::resolve(&baseUri, &uriToResolve, &result));
    CHECK(result == "https://a/b/c/d;p?q#s");
}

TEST_CASE("resolve uri, test 5") {
    std::string baseUri = "https://a/b/c/d;p?q";
    std::string uriToResolve = "../";
    std::string result;

    CHECK(http_link_header::uri::resolve(&baseUri, &uriToResolve, &result));
    CHECK(result == "https://a/b/");
}

TEST_CASE("resolve against baseuri, test 1") {
    auto links = http_link_header::parse(R"(<terms>; rel="copyright")", "https://example.org/a/b");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "https://example.org/a/b");
    CHECK(links[0].linkRelation == "copyright");
    CHECK(links[0].linkTarget == "https://example.org/a/terms");

    CHECK(links[0].targetAttributes.empty());
}

TEST_CASE("resolve against baseuri, test 2") {
    auto links = http_link_header::parse(R"(<../terms>; rel="copyright")", "https://example.org/a/b");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "https://example.org/a/b");
    CHECK(links[0].linkRelation == "copyright");
    CHECK(links[0].linkTarget == "https://example.org/terms");

    CHECK(links[0].targetAttributes.empty());
}

TEST_CASE("override baseuri with anchor attribute") {
    auto links = http_link_header::parse(R"(<../terms>; rel="copyright"; anchor="#foo")", "https://example.org/a/b");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "https://example.org/a/b#foo");
    CHECK(links[0].linkRelation == "copyright");
    CHECK(links[0].linkTarget == "https://example.org/terms");

    CHECK(links[0].targetAttributes.empty());
}

TEST_CASE("multiple relations generate multiple Links") {
    auto links = http_link_header::parse(R"(<http://example.org>; rel="one two" )");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "one");
    CHECK(links[0].linkTarget == "http://example.org");

    CHECK(links[0].targetAttributes.empty());

    CHECK(links[1].linkContext == "");
    CHECK(links[1].linkRelation == "two");
    CHECK(links[1].linkTarget == "http://example.org");

    CHECK(links[1].targetAttributes.empty());
}

TEST_CASE("badly formed parameters, missing value") {
    auto links = http_link_header::parse(R"(<http://example.org>; a= )");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "");
    CHECK(links[0].linkTarget == "http://example.org");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "a");
    CHECK(links[0].targetAttributes[0].value == "");
}

TEST_CASE("badly formed parameters, missing value and =") {
    auto links = http_link_header::parse(R"(<http://example.org>; a )");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "");
    CHECK(links[0].linkTarget == "http://example.org");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "a");
    CHECK(links[0].targetAttributes[0].value == "");
}

TEST_CASE("badly formed parameters, missing name") {
    auto links = http_link_header::parse(R"(<http://example.org>; =1 )");

    // todo: check if having empty param name should be valid?
    // todo: should we be removing trailing whitespace from the param value?

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "");
    CHECK(links[0].linkTarget == "http://example.org");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "");
    CHECK(links[0].targetAttributes[0].value == "1 ");
}

TEST_CASE("badly formed parameters, missing end quote for relations") {
    auto links = http_link_header::parse(R"(<http://example.org>; rel="one two )");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "one");
    CHECK(links[0].linkTarget == "http://example.org");

    CHECK(links[0].targetAttributes.empty());

    CHECK(links[1].linkContext == "");
    CHECK(links[1].linkRelation == "two");
    CHECK(links[1].linkTarget == "http://example.org");

    CHECK(links[1].targetAttributes.empty());
}

TEST_CASE("badly formed parameters, missing start quote for relations") {
    auto links = http_link_header::parse(R"(<http://example.org>; rel=one two" )");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "one");
    CHECK(links[0].linkTarget == "http://example.org");

    CHECK(links[0].targetAttributes.empty());

    CHECK(links[1].linkContext == "");
    CHECK(links[1].linkRelation == "two\"");
    CHECK(links[1].linkTarget == "http://example.org");

    CHECK(links[1].targetAttributes.empty());
}

