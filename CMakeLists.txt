cmake_minimum_required(VERSION 3.10)
project(reverse_proxy)

set(CMAKE_CXX_STANDARD 14)
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(Boost_INCLUDE_DIR /usr/local/include/boost)
    set(Boost_LIBRARY_DIR /usr/local/lib)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(Boost_INCLUDE_DIR "D:\\PATH\\boost_1_70_0")
    set(Boost_LIBRARY_DIR "D:\\PATH\\boost_1_70_0\\lib64-msvc-14.2")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_executable(reverse_proxy client.cpp server.cpp)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(reverse_proxy pthread)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(reverse_proxy)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")