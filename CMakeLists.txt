project(cxx_decls_extractor)
cmake_minimum_required(VERSION 3.1.3)

set(CMAKE_CXX_FLAGS "-std=c++1y -fPIC -fno-rtti")

add_library(CXXEigenAutoChecker SHARED src/checker.cpp)
SET_TARGET_PROPERTIES(CXXEigenAutoChecker PROPERTIES PREFIX "")

