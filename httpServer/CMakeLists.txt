cmake_minimum_required(VERSION 3.20)

add_executable(HttpServerApp HttpServer.cpp HttpServer.h)
	
target_compile_features(HttpServerApp PRIVATE cxx_std_17) 

target_link_libraries(HttpServerApp pqxx)  

target_include_directories(HttpServerApp PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(HttpServerApp ${Boost_LIBRARIES})