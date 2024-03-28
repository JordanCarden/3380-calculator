cmake_minimum_required(VERSION 3.27)
project(3380_calculator)

set(CMAKE_CXX_STANDARD 20)
add_executable(3380_calculator code/calculator.cpp)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(AddStudentTest)

enable_testing()
include(CTest)

add_student_benchmark(calculator)
