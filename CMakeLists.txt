cmake_minimum_required(VERSION 3.20)

project(Spider)

set(BOOST_ROOT "D:/boost_1_88_0")

set(libpqxx_DIR "C:/Work/Projects/libpqxx-7.9.0")

find_package(Boost REQUIRED)

set(PostgreSQL_ADDITIONAL_VERSIONS "16")

add_subdirectory(${libpqxx_DIR} libpqxx-build)

add_subdirectory(httpClient)

add_subdirectory(httpServer)
