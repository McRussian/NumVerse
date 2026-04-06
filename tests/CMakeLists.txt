find_package(GTest REQUIRED)
include(GoogleTest)

file(GLOB_RECURSE LOGIC_SOURCES ../src/logic/*.cpp)

function(add_numverse_test name source)
    add_executable(${name}
        ${source}
        ${LOGIC_SOURCES}
    )
    target_link_libraries(${name} PRIVATE GTest::gtest_main)
    target_include_directories(${name} PRIVATE ../src)
    gtest_discover_tests(${name})
endfunction()

# --- data ---
add_numverse_test(test_game_cell  data/test_game_cell.cpp)
add_numverse_test(test_board      data/test_board.cpp)
add_numverse_test(test_selection  data/test_selection.cpp)
add_numverse_test(test_level_data data/test_level_data.cpp)
