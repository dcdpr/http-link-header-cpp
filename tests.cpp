#include "http_link_header.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("testing the hello function") {
    CHECK(hello() == "hello");
}

