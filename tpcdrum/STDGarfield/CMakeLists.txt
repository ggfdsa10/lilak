cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(STDGarfield)

find_package(Garfield REQUIRED)
find_package(ROOT REQUIRED)

set(STDGARFIELD_DIR ${CMAKE_CURRENT_LIST_DIR})

set(STDGARFIELD_SOURCE_DIR ${STDGARFIELD_DIR}/source CACHE INTERNAL "")

add_definitions(${ROOT_CXX_FLAGS})
include_directories(${ROOT_INCLUDE_DIR})
include_directories(${STDGARFIELD_SOURCE_DIR})

file(GLOB STDGARFIELD_SOURCE_FILES ${STDGARFIELD_SOURCE_DIR}/*.cpp)
file(GLOB STDGARFIELD_HEADER_FILES ${STDGARFIELD_SOURCE_DIR}/*.h)

add_library(STDGarfieldLib SHARED ${STDGARFIELD_SOURCE_FILES})
target_link_libraries(STDGarfieldLib ${ROOT_LIBRARIES} Garfield::Garfield)

add_executable(garfieldSTD STDGarfield.cc)
target_link_libraries(garfieldSTD STDGarfieldLib)


