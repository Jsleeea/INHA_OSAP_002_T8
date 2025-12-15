cmake_minimum_required(VERSION 3.13)
project(AVLSetProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

# 테스트용 라이브러리 (main 제외)
add_library(avlset_lib
        src/AVLSet.cpp
)
target_compile_definitions(avlset_lib PRIVATE AVLSET_NO_MAIN)

# 테스트 실행파일
add_executable(avlset_test
        test_avlset.cpp
)

target_compile_definitions(avlset_test PRIVATE AVLSET_NO_MAIN)
target_link_libraries(avlset_test
        GTest::gtest
        GTest::gtest_main
)

enable_testing()
add_test(NAME AVLSetTest COMMAND avlset_test)

#원래 프로그램 실행용(main 포함)
add_executable(avlset_app
        src/AVLSet.cpp
)