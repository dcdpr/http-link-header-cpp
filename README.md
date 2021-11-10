# http-link-header-cpp

Minimal HTTP "Link:" header parser.

[RFC 8288](https://datatracker.ietf.org/doc/html/rfc8288) describes a model for
the relationships between resources on the Web ("links") and the type of those
relationships ("link relation types"). It also defines the serialization of
such links in HTTP headers with the "Link:" header field.

`http-link-header-cpp` is a "header-only" C++ library for parsing HTTP "Link:" headers.

## Usage Examples

### Parse a simple header
```cpp
    std::vector<http_link_header::Link> links =
        http_link_header::parse(R"(<https://example.com/book/chap2>; rel="previous"; title="previous chapter")");

    std::cout << links[0].linkRelation << std::endl; // previous
    std::cout << links[0].linkTarget << std::endl; // https://example.com/book/chap2

    std::cout << links[0].targetAttributes[0].name << std::endl; // title
    std::cout << links[0].targetAttributes[0].value << std::endl; // previous chapter
```

### Parse a header with multiple links
```cpp
    std::vector<http_link_header::Link> links =
        http_link_header::parse(R"(<https://example.com/book/chap2>; rel="previous"; title="previous chapter", <https://example.com/book/chap4>; rel="next"; title="next chapter")");

    std::cout << links.size() << std::endl; // 2
```

### Parse a header with relative links and specific base URI
```cpp
    std::vector<http_link_header::Link> links =
        http_link_header::parse(R"(<terms>; rel="copyright", <../privacy>; rel="policy")", "https://example.org/a/b");

    std::cout << links[0].linkContext << std::endl; // https://example.org/a/b
    std::cout << links[0].linkTarget << std::endl; // https://example.org/a/terms
    std::cout << links[1].linkContext << std::endl; // https://example.org/a/b
    std::cout << links[1].linkTarget << std::endl; // https://example.org/privacy
```

### Parse a header with relative links, specific base URI, and "anchor" overrides
```cpp
    std::vector<http_link_header::Link> links =
        http_link_header::parse(R"(<terms>; rel="copyright"; anchor="#legal", <other>; rel="other"; anchor="https://corporate.example.org")", "https://example.org/a/b");

    std::cout << links[0].linkContext << std::endl; // https://example.org/a/b#legal
    std::cout << links[0].linkTarget << std::endl; // https://example.org/a/terms
    std::cout << links[1].linkContext << std::endl; // https://corporate.example.org
    std::cout << links[1].linkTarget << std::endl; // https://example.org/a/other
```

## Building

`http-link-header-cpp` is a header-only C++11 library. Building can be done with cmake >= 3.1 and has been tested with g++ and clang compilers. 

`http-link-header-cpp` uses a pretty standard cmake build system. After cloning the repository, execute the following:

```shell
cd http-link-header-cpp
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

You can also run all the tests:

```shell
make test
```

## Dependencies

`http-link-header-cpp` has a dependency on [uriparser](https://github.com/uriparser/uriparser/)
which it uses when resolving URI references. Any project using `http-link-header-cpp` should also be able to find a copy of
uriparser. uriparser should either be installed locally or included within the the project's code.

## Installing

`http-link-header-cpp` will install its header file, `http-link-header.h`, and a few cmake helper files that can be used by other
projects to find and use `http-link-header-cpp`.

The default installation locations for `http-link-header-cpp` are `/usr/local/include` and `/usr/local/share`.

To install using cmake after building and testing, execute the following:

```shell
cmake --build . --config Release --target install
```

You should see output similar to:

```shell
-- Installing: /usr/local/share/http-link-header-cpp/cmake/http-link-headerTargets.cmake
-- Installing: /usr/local/share/http-link-header-cpp/cmake/http-link-headerConfig.cmake
-- Installing: /usr/local/share/http-link-header-cpp/cmake/http-link-headerConfigVersion.cmake
-- Installing: /usr/local/include/http-link-header.h
```

If you want the files to be installed somewhere different, you can set the installation prefix when running the initial cmake command. For example:

```shell
cmake -DCMAKE_INSTALL_PREFIX:PATH=/tmp/http-link-header-cpp-install ..
cmake --build . --config Release --target install
```

## Using http-link-header-cpp

### After installing

Once `http-link-header-cpp` is installed, you can use it in your project with a simple cmake `find_package()` command. Be
sure to mark it as using `CONFIG` mode since we just installed those config files. Also mark it as required if your
project requires `http-link-header-cpp`.

```shell
find_package("http-link-header-cpp" CONFIG REQUIRED)
```

### Not installing

You can use `http-link-header-cpp` in your project without installing it at all. This will require a little more
configuration.

Once you put the `http-link-header-cpp` files somewhere (likely within your own project), then you use
cmake's `add_subdirectory()`. You will want to make sure not to install `http-link-header-cpp` during your install process
by using the `EXCLUDE_FROM_ALL` option.

```shell
add_subdirectory(path/to/http-link-header-cpp ${PROJECT_BINARY_DIR}/http-link-header-cpp-build EXCLUDE_FROM_ALL)
```
