find_package(GTest REQUIRED)

file(GLOB_RECURSE LOGIC_SOURCES ../src/logic/*.cpp)

add_executable(NumVerseTests
    test_main.cpp
    ${LOGIC_SOURCES}
)

target_link_libraries(NumVerseTests PRIVATE GTest::gtest_main)
target_include_directories(NumVerseTests PRIVATE ../src)

include(GoogleTest)
gtest_discover_tests(NumVerseTests)
