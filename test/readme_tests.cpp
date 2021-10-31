// This file contains tests based on examples given in the README

#include "http_link_header.h"
#include "doctest.h"


TEST_CASE("readme, ex 1") {
    auto links = http_link_header::parse(R"(<https://example.com/book/chap2>; rel="previous"; title="previous chapter")");

    CHECK(links.size() == 1);

    CHECK(links[0].linkContext.empty());
    CHECK(links[0].linkRelation == "previous");
    CHECK(links[0].linkTarget == "https://example.com/book/chap2");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "title");
    CHECK(links[0].targetAttributes[0].value == "previous chapter");
}

TEST_CASE("readme, ex 2") {
    auto links = http_link_header::parse(R"(<https://example.com/book/chap2>; rel="previous"; title="previous chapter", <https://example.com/book/chap4>; rel="next"; title="next chapter")");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext.empty());
    CHECK(links[0].linkRelation == "previous");
    CHECK(links[0].linkTarget == "https://example.com/book/chap2");

    CHECK(links[0].targetAttributes.size() == 1);
    CHECK(links[0].targetAttributes[0].name == "title");
    CHECK(links[0].targetAttributes[0].value == "previous chapter");

    CHECK(links[1].linkContext.empty());
    CHECK(links[1].linkRelation == "next");
    CHECK(links[1].linkTarget == "https://example.com/book/chap4");

    CHECK(links[1].targetAttributes.size() == 1);
    CHECK(links[1].targetAttributes[0].name == "title");
    CHECK(links[1].targetAttributes[0].value == "next chapter");
}

TEST_CASE("readme, ex 3") {
    auto links = http_link_header::parse(R"(<terms>; rel="copyright", <../privacy>; rel="policy")", "https://example.org/a/b");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext == "https://example.org/a/b");
    CHECK(links[0].linkRelation == "copyright");
    CHECK(links[0].linkTarget == "https://example.org/a/terms");

    CHECK(links[0].targetAttributes.empty());

    CHECK(links[1].linkContext == "https://example.org/a/b");
    CHECK(links[1].linkRelation == "policy");
    CHECK(links[1].linkTarget == "https://example.org/privacy");

    CHECK(links[1].targetAttributes.empty());
}

TEST_CASE("readme, ex 4") {
    auto links = http_link_header::parse(R"(<terms>; rel="copyright"; anchor="#legal", <other>; rel="other"; anchor="https://corporate.example.org")", "https://example.org/a/b");

    CHECK(links.size() == 2);

    CHECK(links[0].linkContext == "https://example.org/a/b#legal");
    CHECK(links[0].linkRelation == "copyright");
    CHECK(links[0].linkTarget == "https://example.org/a/terms");

    CHECK(links[1].linkContext == "https://corporate.example.org");
    CHECK(links[1].linkRelation == "other");
    CHECK(links[1].linkTarget == "https://example.org/a/other");

}
