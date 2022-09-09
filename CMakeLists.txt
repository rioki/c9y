cmake_minimum_required(VERSION 3.8)
project(c9y CXX)

set(PROJECT_VERSION 0.4.1)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 4)
set(PROJECT_VERSION_PATCH 1)

option(ENABLE_UNIT_TESTS "Enable building unit tests." OFF)

set(CMAKE_SHARED_LIBRARY_PREFIX)
set(CMAKE_STATIC_LIBRARY_PREFIX)

if(ENABLE_UNIT_TESTS)
  find_package(GTest CONFIG REQUIRED)
endif()

set(HEADERS
  c9y/async.h
  c9y/c9y.h
  c9y/coroutine.h
  c9y/defines.h
  c9y/latch.h
  c9y/parallel.h
  c9y/queue.h
  c9y/sync.h
  c9y/task_pool.h
  c9y/thread_pool.h
)

set(SOURCES
  c9y/async.cpp
  c9y/latch.cpp
  c9y/parallel.cpp
  c9y/sync.cpp
  c9y/task_pool.cpp
  c9y/thread_pool.cpp
)

# c9y library
add_library(c9y ${SOURCES})
set_target_properties(c9y PROPERTIES
    CXX_STANDARD 20
    PUBLIC_HEADER "${HEADERS}"
)

# c9y-test 
if(ENABLE_UNIT_TESTS)

  enable_testing()

  set(TEST_SOURCES
    c9y-test/async_test.cpp
    c9y-test/coroutine_test.cpp
    c9y-test/latch_test.cpp
    c9y-test/main.cpp
    c9y-test/paralell_test.cpp
    c9y-test/queue_test.cpp
    c9y-test/sync_test.cpp
    c9y-test/thread_pool_test.cpp
  )
  
  include_directories(.)
  add_executable(c9y-test ${TEST_SOURCES})
  set_target_properties(c9y-test PROPERTIES
    CXX_STANDARD 20
  )
  target_link_libraries(c9y-test PRIVATE c9y GTest::gtest)
  add_test(c9y-test c9y-test)

endif()

# install
install(
  TARGETS c9y
  EXPORT c9yTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/rioki/glow
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/c9yConfigVersion.cmake"
                                 COMPATIBILITY SameMajorVersion)

configure_package_config_file(${PROJECT_SOURCE_DIR}/c9yConfig.cmake.in
                                c9yConfig.cmake
                                INSTALL_DESTINATION share/c9y)

install(FILES
        "${PROJECT_BINARY_DIR}/c9yConfig.cmake"
        "${PROJECT_BINARY_DIR}/c9yConfigVersion.cmake"
        DESTINATION share/c9y
)

install(EXPORT c9yTargets
        NAMESPACE c9y::
        DESTINATION share/c9y
)