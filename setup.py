#!/usr/bin/env python3

import os
import re
import sys
import sysconfig
import platform
import subprocess
import pybind11

from distutils.version import LooseVersion
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.test import test as TestCommand
from shutil import copyfile, copymode


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(["cmake", "--version"])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: "
                + ", ".join(e.name for e in self.extensions)
            )

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r"version\s*([\d.]+)", out.decode()).group(1))
            if cmake_version < "3.5.0":
                raise RuntimeError("CMake >= 3.5.0 is required")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = [
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir,
            "-Dbuild_py=ON",
            f"-Dpython_include_A={pybind11.get_include()}",
            f"-Dpython_include_B={sysconfig.get_path('include')}",
        ]

        cfg = "Debug" if self.debug else "Release"
        build_args = ["--config", cfg]

        if platform.system() == "Windows":
            cmake_args += ["-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}".format(cfg.upper(), extdir)]
            if sys.maxsize > 2 ** 32:
                cmake_args += ["-A", "x64"]
            build_args += ["--", "/m"]
        else:
            cmake_args += ["-DCMAKE_BUILD_TYPE=" + cfg]
            build_args += ["--", "-j2"]

        env = os.environ.copy()
        env["CXXFLAGS"] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get("CXXFLAGS", ""), self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        cmake_set_args = ["cmake", ext.sourcedir] + cmake_args
        cmake_build_args = ["cmake", "--build", "."] + build_args
        subprocess.check_call(cmake_set_args, cwd=self.build_temp, env=env)
        subprocess.check_call(cmake_build_args, cwd=self.build_temp)


setup(
    name="mmtf_bindings",
    version="0.1.0",
    author="Daniel P. Farrell",
    author_email="danpf@uw.edu",
    url="https://github.com/rcsb/mmtf-cpp",
    description="A hybrid Python/C++ test project",
    long_description=open("README.md").read(),
    packages=find_packages("python_src", exclude=["tests", "python_src/tests"]),
    package_dir={"": "python_src"},
    ext_modules=[CMakeExtension("mmtf_cppy/")],
    cmdclass=dict(build_ext=CMakeBuild),
    test_suite="tests",
    zip_safe=False,
    tests_require=["pytest", "pytest-cov"],
    install_requires=["numpy", "msgpack", "pybind11",],
)
