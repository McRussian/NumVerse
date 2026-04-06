find_package(GTest REQUIRED)
include(GoogleTest)

file(GLOB_RECURSE LOGIC_SOURCES ../src/logic/*.cpp)

function(add_numverse_test name)
    add_executable(${name}
        ${name}.cpp
        ${LOGIC_SOURCES}
    )
    target_link_libraries(${name} PRIVATE GTest::gtest_main)
    target_include_directories(${name} PRIVATE ../src)
    gtest_discover_tests(${name})
endfunction()

# --- tests ---
add_numverse_test(test_game_cell)
add_numverse_test(test_board)
add_numverse_test(test_selection)
add_numverse_test(test_level_data)
