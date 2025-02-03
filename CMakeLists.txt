cmake_minimum_required(VERSION 3.15)
project(c9y
        VERSION 0.7.0
        DESCRIPTION "Concurency"
        LANGUAGES CXX
)

option(ENABLE_UNIT_TESTS "Enable building unit tests." OFF)

set(HEADERS
  c9y/async.h
  c9y/barrier.h
  c9y/c9y.h
  c9y/coroutine.h
  c9y/defer.h
  c9y/defines.h
  c9y/exceptions.h
  c9y/jthread.h
  c9y/latch.h
  c9y/parallel.h
  c9y/queue.h
  c9y/sync.h
  c9y/task_pool.h
  c9y/thread_pool.h
  c9y/utils.h
)

# library
add_library(c9y
  c9y/async.cpp
  c9y/defer.cpp
  c9y/exceptions.cpp
  c9y/jthread.cpp
  c9y/latch.cpp
  c9y/parallel.cpp
  c9y/sync.cpp
  c9y/task_pool.cpp
  c9y/thread_pool.cpp
)
target_include_directories(c9y SYSTEM INTERFACE
  "$<BUILD_INTERFACE:${c9y_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)
target_compile_features(c9y PRIVATE cxx_std_20)
set_target_properties(c9y PROPERTIES
    CXX_STANDARD 20
    PUBLIC_HEADER "${HEADERS}"
)


# unit tests
if(ENABLE_UNIT_TESTS)
  find_package(GTest CONFIG REQUIRED)
  add_executable(c9y-test
    c9y-test/async_test.cpp
    c9y-test/barrier_test.cpp
    c9y-test/coroutine_test.cpp
    c9y-test/defer_test.cpp
    c9y-test/exception_test.cpp
    c9y-test/jthread_test.cpp
    c9y-test/latch_test.cpp
    c9y-test/main.cpp
    c9y-test/paralell_test.cpp
    c9y-test/philosophers_test.cpp
    c9y-test/queue_test.cpp
    c9y-test/sync_test.cpp
    c9y-test/thread_pool_test.cpp
  )
  include_directories(.)
  target_compile_features(c9y-test PRIVATE cxx_std_20)
  set_target_properties(c9y PROPERTIES
      CXX_STANDARD 20
  )
  target_link_libraries(c9y-test PRIVATE c9y GTest::gtest)
  add_test(NAME c9y-test
           COMMAND c9y-test
  )
endif()

# install
install(TARGETS c9y
        EXPORT c9yTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include/c9y
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    c9yConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT c9yTargets
        FILE c9yTargets.cmake
        NAMESPACE c9y::
        DESTINATION lib/cmake/c9y
)

configure_file(c9yConfig.cmake.in c9yConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/c9yConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/c9yConfigVersion.cmake"
        DESTINATION lib/cmake/c9y
)

export(TARGETS c9y
       NAMESPACE c9y::
       FILE c9yTargets.cmake
)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE c9y)

configure_file(c9y.pc.in "${CMAKE_CURRENT_BINARY_DIR}/c9y.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/c9y.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
