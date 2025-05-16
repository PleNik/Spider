cmake_minimum_required (VERSION 3.20)

add_executable (HTTPClientApp httpClient.cpp httpClient.h
							  ../parserFileSettingsIni.h
							  ../parserFileSettingsIni.cpp
							  ../Database/dataBase.h
							  ../Database/dataBase.cpp
)

target_compile_features(HTTPClientApp PRIVATE cxx_std_17) 

target_link_libraries(HTTPClientApp pqxx)

target_include_directories(HTTPClientApp PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(HTTPClientApp ${Boost_LIBRARIES})