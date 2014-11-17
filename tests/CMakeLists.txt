find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ../libmjkgb/include
    ../libmjkgb/src
)
add_executable(mjkgb_test
    ./accessors.cpp
    ./opcodes.cpp

    ./main.cpp
)
target_link_libraries(mjkgb_test libmjkgb
    ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_test(mjkgb_test mjkgb_test)

