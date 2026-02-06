find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(GLEW REQUIRED)

file(GLOB_RECURSE COMMON_SRC include/*.h src/*.cc)
add_library(common STATIC ${COMMON_SRC})
target_include_directories(common PUBLIC include/)
target_link_libraries(common PUBLIC core SDL3::SDL3 imgui::imgui GLEW::GLEW)
setup_warnings(common)

if(ENABLE_TESTING)

    file(GLOB_RECURSE COMMON_TEST_SRC test/*.cc)
    add_executable(common_test ${COMMON_TEST_SRC})
    target_link_libraries(common_test PRIVATE common GTest::gtest GTest::gtest_main)
    setup_warnings(common_test)
    gtest_discover_tests(common_test)
endif()