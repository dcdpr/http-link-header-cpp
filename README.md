# http_link_header

Minimal HTTP "Link:" header parser.

[RFC 8288](https://datatracker.ietf.org/doc/html/rfc8288) describes a model for
the relationships between resources on the Web ("links") and the type of those
relationships ("link relation types"). It also defines the serialization of
such links in HTTP headers with the "Link:" header field.

`http_link_header` is a "header-only" C++ library for parsing HTTP "Link:" headers.

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

`http_link_header` is a header-only C++11 library. Building can be done with cmake > 3.1 and has been tested with g++ and clang compilers. 

`http_link_header` uses a pretty standard cmake build system. After cloning the repository, execute the following:

```bash
cd http_link_header
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

You can also run all the tests:

```
make test
```

## Dependencies

`http_link_header` has a dependency on [uriparser](https://github.com/uriparser/uriparser/)
which it uses when resolving URI references. Any project using `http_link_header` should also be able to find a copy of
uriparser. uriparser should either be installed locally or included within the the project's code.

## Installing

`http_link_header` will install its header file, `http_link_header.h`, and a few cmake helper files that can be used by other
projects to find and use `http_link_header`.

The default installation locations for `http_link_header` are `/usr/local/include` and `/usr/local/share`.

To install using cmake after building and testing, execute the following:

```bash
cmake --build . --config Release --target install
```

You should see output similar to:

```bash
-- Installing: /usr/local/share/http_link_header/cmake/http_link_headerTargets.cmake
-- Installing: /usr/local/share/http_link_header/cmake/http_link_headerConfig.cmake
-- Installing: /usr/local/share/http_link_header/cmake/http_link_headerConfigVersion.cmake
-- Installing: /usr/local/include/http_link_header.h
```

If you want the files to be installed somewhere different, you can set the installation prefix when running the initial cmake command. For example:

```shell
cmake -DCMAKE_INSTALL_PREFIX:PATH=/tmp/http_link_header-install ..
cmake --build . --config Release --target install
```

## Using http_link_header in your projects

### After installing

Once `http_link_library` is installed, you can use it in your project with a simple cmake `find_package` command. Be
sure to mark it as using `CONFIG` mode since we just installed those config files. Also mark it as required if your
projects requires `http_link_library`.

```shell
find_package("http_link_header" CONFIG REQUIRED)
```

### Not installing

You can use `http_link_header` in your project without installing it at all. This will require a little more
configuration.

Once you put the `http_link_header` files somewhere (likely within your own project), then you use
cmake's `add_subdirectory()`. You will want to make sure not to install `http_link_header` during your install process
by using the `EXCLUDE_FROM_ALL` option.

```shell
add_subdirectory(path/to/http_link_header ${PROJECT_BINARY_DIR}/http_link_header-build EXCLUDE_FROM_ALL)
```
