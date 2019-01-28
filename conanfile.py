#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools
import os


class LibodbSqliteConan(ConanFile):
    name = "libodb-sqlite"
    version = "2.5.0-b.9"
    description = "libodb-sqlite for conan, only for personal use"
    # topics can get used for searches, GitHub topics, Bintray tags etc. Add here keywords about the library
    topics = ("conan", "libodb-sqlite", "logging")
    url = "https://github.com/yssource/conan-libodb-sqlite"
    homepage = "https://git.codesynthesis.com/cgit/odb/libodb-sqlite/"
    author = "Jimmy M. Gong<yssource@163.com>"
    license = "MIT"  # Indicates license type of the packaged library; please use SPDX Identifiers https://spdx.org/licenses/
    exports = ["LICENSE.md"]      # Packages the license for the conanfile.py

    # Options may need to change depending on the packaged library.
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Custom attributes for Bincrafters recipe conventions
    _source_subfolder = "src"
    _build_subfolder = "build"
    _dist_subfolder = 'dist'

    def config_options(self):
        if self.settings.os == 'Windows':
            del self.options.fPIC

    def source(self):
        source_url = "https://pkg.cppget.org/1/beta/odb/libodb-sqlite"
        tools.get("{}-{}.tar.gz".format(source_url, self.version),
                  sha256="16b775b0765cd2bc5d1b75a0f469dc4773d25d99f92679e8c7c35180230c6dde")
        extracted_dir = self.name + "-" + self.version

        # Rename to "source_subfolder" is a convention to simplify later steps
        os.rename(extracted_dir, self._source_subfolder)

    def _configure(self):
        with tools.chdir("{}".format('..')):
            self.run('pwd')

    def build(self):
        self._configure()

    def package(self):
        self.copy(pattern="LICENSE", dst="licenses",
                  src=self._source_subfolder)
        # If the CMakeLists.txt has a proper install method, the steps below may be redundant
        # If so, you can just remove the lines below
        include_folder = os.path.join(self._dist_subfolder, "include")
        self.copy(pattern="*", dst="include", src=include_folder)
        self.copy(pattern="*.dll", dst="bin", keep_path=False)
        self.copy(pattern="*.lib", dst="lib", keep_path=False)
        self.copy(pattern="*.a", dst="lib", keep_path=False)
        self.copy(pattern="*.so*", dst="lib", keep_path=False)
        self.copy(pattern="*.dylib", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
