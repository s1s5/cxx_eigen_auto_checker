# -*- python -*-
# coding:utf-8
import os
import subprocess

if 'LLVM_DIR' in os.environ:
    llvm_dir = os.environ['LLVM_DIR']


def getFlags(flags):
    llvm_config = os.path.join(llvm_dir, 'bin', 'llvm-config')
    p = subprocess.Popen([llvm_config, flags], stdout=subprocess.PIPE)
    stdout, stderr = p.communicate()
    if p.returncode:
        raise Exception(stdout + "\n" + stderr)
    return stdout.strip().split(' ')


cxxflags = getFlags('--cxxflags')
cppflags = getFlags('--cppflags')
ldflags = getFlags('--ldflags')
libs = getFlags('--libs')

BIN_PREFIX = ''
CC = BIN_PREFIX + 'clang'
CXX = BIN_PREFIX + 'clang++'

INCLUDES = [
]

CFLAGS = [
    '-g3',
    '-Wall',
    '-Werror=return-type',
] + cxxflags
CXXFLAGS = CFLAGS + [
    '-std=c++11',
]

LINKFLAGS = [
    '-Wl,-no-as-needed',
    '-Wl,-rpath=' + os.path.join(llvm_dir, 'lib'),
] + ldflags

LIBFLAGS = [
]

env = Environment(
    ENV=os.environ,
    CC=CC,
    CPATH=INCLUDES,
    CXX=CXX,
    CPPPATH=INCLUDES,
    CFLAGS=' '.join(CFLAGS),
    CXXFLAGS=' '.join(CXXFLAGS),
    LINKFLAGS=' '.join(LINKFLAGS),
    _LIBFLAGS=' '.join(LIBFLAGS),
)

env.SharedLibrary(
    target='eigen_cxx_auto_checker_plugin', source=['checker.cpp'])
