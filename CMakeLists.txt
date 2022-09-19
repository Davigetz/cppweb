cmake_minimum_required(VERSION 3.22)
project(web)

# EITHER Using the "cmake_find_package" generator
#set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
#set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
set(
    readFromString
    readFromStream
    stringWrite
    streamWrite
)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(
        -D_SCL_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_WARNINGS
        -D_WIN32_WINNT=0x601
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
    )
endif()
# add_definitions( "-std=c++17 -ljsoncpp " )
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(web src/main.cpp)