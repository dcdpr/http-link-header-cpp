#include "http_link_header.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


std::string header_previousChapter =
        R"(<http://example.com/TheBook/chapter2>; rel="previous"; title="previous chapter")";

std::string header_nextChapter =
        R"(<http://example.com/TheBook/chapter4>; rel="next"; title="next chapter")";

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

TEST_CASE("split empty string returns empty vector") {
    CHECK(http_link_header::detail::split("", '.').empty());
}

TEST_CASE("split string without embedded delimiter character returns string") {
    std::vector<std::string> parts = http_link_header::detail::split("hello", '.');
    CHECK(parts.size() == 1);
    CHECK(parts[0] == "hello");
}

TEST_CASE("split string returns multiple substrings") {
    std::vector<std::string> parts = http_link_header::detail::split("hello, how are you?", ' ');
    CHECK(parts.size() == 4);
    CHECK(parts[0] == "hello,");
    CHECK(parts[1] == "how");
    CHECK(parts[2] == "are");
    CHECK(parts[3] == "you?");
}

TEST_CASE("split string with consecutive delimiters returns only non-empty substrings") {
    std::vector<std::string> parts = http_link_header::detail::split("  hello,  how    are you? ", ' ');
    CHECK(parts.size() == 4);
    CHECK(parts[0] == "hello,");
    CHECK(parts[1] == "how");
    CHECK(parts[2] == "are");
    CHECK(parts[3] == "you?");
}

TEST_CASE("create Link objects with subparts") {

    http_link_header::Link link{"a", "b", "c"};

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
    http_link_header::Link link1{"a", "b", "c"};
    http_link_header::Link link2{"a", "b", "c"};
    http_link_header::Link link3{"d", "e", "f"};

    CHECK(link1 == link2);
    CHECK(link1 != link3);
}

TEST_CASE("parse quoted value") {
    std::string input = "\"hello\"";
    CHECK(http_link_header::parseQuotedString(input) == "hello");
    CHECK(input.empty());

    input = "bye";
    CHECK(http_link_header::parseQuotedString(input) == "");
    CHECK(input == "bye");
}



TEST_CASE("parse header, check parts, test1") {
    auto links = http_link_header::parse(header_previousChapter);

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext.empty());
    CHECK(links[0].linkRelation == "previous");
    CHECK(links[0].linkTarget == "http://example.com/TheBook/chapter2");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "title");
    CHECK(links[0].targetAttributes[0].value == "previous chapter");
}

TEST_CASE("parse header, check parts, test2") {
    auto links = http_link_header::parse(header_nextChapter);

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext.empty());
    CHECK(links[0].linkRelation == "next");
    CHECK(links[0].linkTarget == "http://example.com/TheBook/chapter4");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "title");
    CHECK(links[0].targetAttributes[0].value == "next chapter");
}

TEST_CASE("parse header, rfc8288 ex 2") {
    auto links = http_link_header::parse(R"(</>; rel="http://example.net/foo")");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext.empty());
    CHECK(links[0].linkRelation == "http://example.net/foo");
    CHECK(links[0].linkTarget == "/");

    CHECK(links[0].targetAttributes.size() == 0);
}

TEST_CASE("parse header, rfc8288 ex 3") {
    auto links = http_link_header::parse(R"(</terms>; rel="copyright"; anchor="#foo")");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "#foo");
    CHECK(links[0].linkRelation == "copyright");
    CHECK(links[0].linkTarget == "/terms");

    CHECK(links[0].targetAttributes.empty());
}

TEST_CASE("parse header, rfc8288 ex 5") {
    auto links = http_link_header::parse(R"(<http://example.org/>;  rel="start http://example.net/relation/other")");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "start");
    CHECK(links[0].linkTarget == "http://example.org/");

    CHECK(links[0].targetAttributes.empty());

    CHECK(links[1].linkContext == "");
    CHECK(links[1].linkRelation == "http://example.net/relation/other");
    CHECK(links[1].linkTarget == "http://example.org/");

    CHECK(links[1].targetAttributes.empty());
}

// <https://example.org/>; rel="start", <https://example.org/index>; rel="index"

TEST_CASE("parse header, rfc8288 ex 6") {
    auto links = http_link_header::parse(R"(<https://example.org/>; rel="start", <https://example.org/index>; rel="index")");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "start");
    CHECK(links[0].linkTarget == "https://example.org/");

    CHECK(links[0].targetAttributes.empty());

    CHECK(links[1].linkContext == "");
    CHECK(links[1].linkRelation == "index");
    CHECK(links[1].linkTarget == "https://example.org/index");

    CHECK(links[1].targetAttributes.empty());
}

TEST_CASE("empty link target") {
    auto links = http_link_header::parse(R"(<>; rel="start")");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "start");
    CHECK(links[0].linkTarget == "");

    CHECK(links[0].targetAttributes.empty());

}

TEST_CASE("resolve against baseuri ") {
    auto links = http_link_header::parse(R"(</terms>; rel="copyright")", "http://example.org");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "copyright");
    CHECK(links[0].linkTarget == "http://example.org/terms");

    CHECK(links[0].targetAttributes.empty());

}
