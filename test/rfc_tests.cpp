// This file contains tests based on examples given in RFC8288
// see: https://datatracker.ietf.org/doc/html/rfc8288#section-3.5

#include "http_link_header.h"
#include "doctest.h"


TEST_CASE("parse header, rfc8288 ex 1") {
    auto links = http_link_header::parse(R"(<http://example.com/TheBook/chapter2>; rel="previous"; title="previous chapter")");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext.empty());
    CHECK(links[0].linkRelation == "previous");
    CHECK(links[0].linkTarget == "http://example.com/TheBook/chapter2");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "title");
    CHECK(links[0].targetAttributes[0].value == "previous chapter");
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

TEST_CASE("parse header, rfc8288 ex 4" * doctest::may_fail()) {

    // todo: this test won't pass until we finish implementing step 2.7.5 of the "Parsing
    // Parameters" algorithm and step 16.2 of the "Parsing a Link Field Value" algorithm.

    auto links = http_link_header::parse(R"(</TheBook/chapter2>; rel="previous"; title*=UTF-8'de'letztes%20Kapitel, </TheBook/chapter4>; rel="next"; title*=UTF-8'de'n%c3%a4chstes%20Kapitel")");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext == "");
    CHECK(links[0].linkRelation == "previous");
    CHECK(links[0].linkTarget == "/TheBook/chapter2");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "title");
    CHECK(links[0].targetAttributes[0].value == "tbd");

    CHECK(links[1].linkContext == "");
    CHECK(links[1].linkRelation == "next");
    CHECK(links[1].linkTarget == "/TheBook/chapter4");

    CHECK(links[1].targetAttributes.size() == 1);
    CHECK(links[1].targetAttributes[0].name == "title");
    CHECK(links[1].targetAttributes[0].value == "tbd");
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
