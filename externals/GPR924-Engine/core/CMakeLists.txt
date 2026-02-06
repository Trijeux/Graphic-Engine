file(GLOB_RECURSE CORE_SRC include/*.h src/*.cc)
add_library(core STATIC ${CORE_SRC})
target_include_directories(core PUBLIC include/)
setup_warnings(core)

if(ENABLE_TESTING)
    file(GLOB_RECURSE CORE_TEST_SRC test/*.cc)
    add_executable(core_test ${CORE_TEST_SRC})

    target_link_libraries(core_test PRIVATE core GTest::gtest GTest::gtest_main)
    setup_warnings(core_test)
    gtest_discover_tests(core_test)
endif()