from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps

required_conan_version = ">=1.53.0"

class HttpLinkHeaderCppConan(ConanFile):
    name = "http-link-header-cpp"
    url = "https://github.com/dcdpr/http-link-header-cpp"
    description = "http-link-header-cpp is a \"header-only\" C++ library for parsing HTTP \"Link:\" headers (see RFC 8288)."
    homepage = "https://github.com/dcdpr/http-link-header-cpp"
    license = "BSD 3-Clause License"

    settings = "os", "arch", "compiler", "build_type"
    default_options = {
        "shared": False,
        "fPIC": True
    }
    options = {name: [True, False] for name in default_options.keys()}
    exports_sources = "*"

    def requirements(self):
        self.requires("uriparser/0.9.7")
        self.requires("doctest/2.4.9", private=True)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["HLH_BUILD_TESTING"] = True
        tc.variables["CMAKE_VERBOSE_MAKEFILE"] = True
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ['include']  # Ordered list of include paths
        self.cpp_info.system_libs = []  # System libs to link against
