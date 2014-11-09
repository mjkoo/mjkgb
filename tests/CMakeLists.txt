find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
add_executable(mjkgb_test
    ./accessors.cpp
)
target_link_libraries(mjkgb_test libmjkgb
    ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_test(mjkgb_test mjkgb_test)

