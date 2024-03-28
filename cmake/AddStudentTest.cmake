set(WITH_BENCHMARKING
    ON
    CACHE BOOL "Enable benchmarking")

macro(add_student_test name)
  add_executable(${name} code/${name}.cpp)
  target_compile_definitions(${name} PRIVATE CATCH_CONFIG_MAIN)
  add_test(
    NAME ${name}
    COMMAND "$<TARGET_FILE:${name}>" ${options}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

macro(add_student_runner_test name)
  add_executable(${name} code/${name}.cpp)
  target_compile_definitions(${name} PRIVATE CATCH_CONFIG_RUNNER)
  add_test(
    NAME ${name}
    COMMAND "$<TARGET_FILE:${name}>" ${options}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

macro(add_student_benchmark name)
  add_executable(${name} code/${name}.cpp)
  target_compile_definitions(${name} PRIVATE CATCH_CONFIG_MAIN
                                             CATCH_CONFIG_ENABLE_BENCHMARKING)
  set(options "--benchmark-samples" "10")
  if(NOT WITH_BENCHMARKING)
    set(options ${options} "--skip-benchmarks")
  endif()
  add_test(
    NAME ${name}
    COMMAND "$<TARGET_FILE:${name}>" ${options}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()
