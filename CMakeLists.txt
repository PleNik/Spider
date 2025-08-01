cmake_minimum_required(VERSION 3.20)

project(Spider)

set(OPENSSL_ROOT_DIR "D:/OpenSSL-3.0.16")


set(BOOST_ROOT "D:/boost_1_88_0")

set(libpqxx_DIR "D:/libpqxx-7.10.1")

find_package(Boost REQUIRED)


if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost")
endif()

set(PostgreSQL_ADDITIONAL_VERSIONS "16")

add_subdirectory(${libpqxx_DIR} libpqxx-build)

find_package(OpenSSL REQUIRED)

add_subdirectory(httpClient)

add_subdirectory(httpServer)
