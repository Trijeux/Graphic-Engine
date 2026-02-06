cmake_minimum_required (VERSION 3.18)
project(GPR924-CompGraph)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/data.cmake)
include(cmake/shaders.cmake)

add_subdirectory(externals/GPR924-Engine)

file(GLOB_RECURSE SRC_FILE src/*.cc include/*.h)

find_package(assimp CONFIG REQUIRED)

add_library(CompGraphLib STATIC ${SRC_FILE})
target_include_directories(CompGraphLib PUBLIC include)
target_link_libraries(CompGraphLib PUBLIC common PRIVATE assimp::assimp)
checkshaders("${CMAKE_SOURCE_DIR}" CompGraphLib)
copydata("${CMAKE_SOURCE_DIR}" CompGraphLib)

file(GLOB MAIN_FILES main/*.cc)
foreach(main ${MAIN_FILES})
    # Extract filename without extension to use as target name
    get_filename_component(target ${main} NAME_WE)

    message(STATUS "Creating executable ${target} from ${main}")

    add_executable(${target} ${main})
    target_link_libraries(${target} PUBLIC CompGraphLib)
endforeach()