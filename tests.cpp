#include "http_link_header.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <typeinfo>

std::string header_previousChapter =
        R"(Link: <http://example.com/TheBook/chapter2>; rel="previous"; title="previous chapter")";

std::string header_nextChapter =
        R"(, <http://example.com/TheBook/chapter4>; rel="next"; title="next chapter")";

TEST_CASE("parsing empty header string returns empty vector") {
    CHECK(http_link_header::parse("").empty());
}

TEST_CASE("parsing non-Link header string returns empty vector") {
    CHECK(http_link_header::parse("Subject: hello").empty());
}

TEST_CASE("parsing Link header string returns vector with one Link") {
    CHECK(http_link_header::parse(header_previousChapter).size() == 1);
}

TEST_CASE("parsing Link header string returns vector with two Links") {
    std::string header = header_previousChapter + header_nextChapter;
    CHECK(http_link_header::parse(header).size() == 2);
}



TEST_CASE("parsing empty vector of header strings returns empty vector of Links") {
    std::vector<std::string> headers;
    CHECK(http_link_header::parse(headers).empty());
}

TEST_CASE("parsing vector of header strings containing non-Link header string returns empty vector") {
    std::vector<std::string> headers;
    headers.emplace_back("Subject: hello");
    CHECK(http_link_header::parse(headers).empty());
}

TEST_CASE("parsing vector of header strings with one Link header returns vector with one Link") {
    std::vector<std::string> headers;
    headers.push_back(header_previousChapter);
    CHECK(http_link_header::parse(headers).size() == 1);
}

TEST_CASE("parsing vector of header strings with one Link header string returns vector with two Links") {
    std::string header = header_previousChapter + header_nextChapter;
    std::vector<std::string> headers;
    headers.push_back(header);
    CHECK(http_link_header::parse(headers).size() == 2);
}

TEST_CASE("parsing vector of header strings with two Link header string sreturns vector with four Links") {
    std::string header = header_previousChapter + header_nextChapter;
    std::vector<std::string> headers;
    headers.push_back(header);
    headers.push_back(header);
    CHECK(http_link_header::parse(headers).size() == 4);
}

