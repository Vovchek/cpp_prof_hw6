cmake_minimum_required(VERSION 3.14)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(matrix VERSION ${PROJECT_VESRION})

# ++for google test
include(FetchContent)   # run ..\CMake\share\cmake-X.YY\Modules\FetchContent.cmake
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  #URL G:/softdev/googletest-main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

enable_testing()    # Enables testing for this directory and below
add_executable(matrix matrix_test.cpp)
target_link_libraries(matrix_test GTest::gtest_main)
# --for google test

add_executable(matrix matrix.cpp) # target source ...

set_target_properties(matrix PROPERTIES    # target PROPERTIES prop1 value1  ...
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(matrix
    PRIVATE "${CMAKE_BINARY_DIR}"
)

if (MSVC)
    target_compile_options(matrix PRIVATE
        /W4
    )
else ()
    target_compile_options(matrix PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

# ++for google test
include(GoogleTest)               # run ..\CMake\share\cmake-X.YY\Modules\GoogleTest.cmake
gtest_discover_tests(matrix_test)  # discover tests by asking the compiled test executable to enumerate its tests
# ++for google test

install(TARGETS matrix matrix_test RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
