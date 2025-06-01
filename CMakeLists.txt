cmake_minimum_required(VERSION 3.20)

project(Spider)

set(BOOST_ROOT "D:/boost_1_88_0")


#set (Boost_USE_MULTITHREADED ON)
#set (Boost_USE_STATIC_LIBS ON)
#set (Boost_USE_STATIC_RUNTIME OFF)
#set (BOOST_ALL_DYN_LINK OFF)

set(libpqxx_DIR "C:/Work/Projects/libpqxx-7.9.0")

find_package(Boost REQUIRED)

#find_package (Boost REQUIRED COMPONENTS program_options REQUIRED) 


if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost")
endif()


#if (Boost_FOUND)
    #include_directories (SYSTEM ${Boost_INCLUDE_DIR}) # подключаем заголовочные файлы
   # add_executable (Boost_test "main.cpp") # Создаем исполняемый файл для нашего проекта
    #target_link_libraries (Spider ${Boost_LIBRARIES}) # подключаем библиотеку
#endif ()

set(PostgreSQL_ADDITIONAL_VERSIONS "16")

add_subdirectory(${libpqxx_DIR} libpqxx-build)

add_subdirectory(httpClient)

add_subdirectory(httpServer)
