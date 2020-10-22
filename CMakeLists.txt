cmake_minimum_required(VERSION 3.17)
project(errorHandler)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES symbolAnalysis.cpp Token.h Token.cpp ParseSyntax.cpp ParseSyntax.h ParseToken.cpp ParseToken.h ErrorInfo.cpp ErrorInfo.h OutputControl.h SymbolTableItem.cpp SymbolTableItem.h)
add_executable(errorHandler ${SOURCE_FILES})

