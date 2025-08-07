cmake_minimum_required(VERSION 3.13)

project(codecrafters-kafka)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     # ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
     # ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
     )

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
  # ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hxx
  )
add_executable(kafka ${SOURCES} ${HEADERS})


message(STATUS "HEADERS: ${HEADERS}")
message(STATUS "SOURCES: ${SOURCES}")
