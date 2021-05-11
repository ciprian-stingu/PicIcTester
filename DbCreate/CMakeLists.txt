project(Database C CXX)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)

#required packages
#find_package(PkgConfig REQUIRED)

#config
#set(CMAKE_BUILD_TYPE Release CACHE STRING "Default release build." FORCE)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default debug build." FORCE)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -W -O3 -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -W -O0 -pedantic")

add_executable(DbCreate DbCreate.cpp)
set_target_properties(DbCreate PROPERTIES COMPILE_FLAGS "-DCONSOLE_APPLICATION -DAPP_NAME=DbCreate")
install(TARGETS DbCreate RUNTIME DESTINATION bin)
