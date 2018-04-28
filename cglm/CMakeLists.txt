cmake_minimum_required (VERSION 2.6)
project (Cglm)

set (CGLM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "cglm includes")

include_directories (${CGLM_INCLUDES})

#
# Only for GL/gl.h
#
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#
# Tests
#
add_executable (cglm_tests
  cglm_tests.c
  cglm.h)

if ("${CMAKE_SYSTEM}" MATCHES "Linux")
    target_link_libraries (cglm_tests -lm)
endif()

enable_testing()
add_test (NAME Mat4          COMMAND cglm_tests Mat4)
add_test (NAME Cross         COMMAND cglm_tests Cross)
add_test (NAME Dot           COMMAND cglm_tests Dot)
add_test (NAME Normalize     COMMAND cglm_tests Normalize)
add_test (NAME SubsVec3      COMMAND cglm_tests SubsVec3)
add_test (NAME AddVec3       COMMAND cglm_tests AddVec3)
add_test (NAME MultMat4      COMMAND cglm_tests MultMat4)
add_test (NAME LookAt        COMMAND cglm_tests LookAt)
add_test (NAME Ortho         COMMAND cglm_tests Ortho)
add_test (NAME Perspective   COMMAND cglm_tests Perspective)
add_test (NAME Frustum       COMMAND cglm_tests Frustum)
add_test (NAME ScalarMultMat4 COMMAND cglm_tests ScalarMultMat4)
add_test (NAME ScalarMultVec3 COMMAND cglm_tests ScalarMultVec3)

#
# Benchmark
#
add_executable (cglm_bench
  cglm_bench.c
  cglm.h)

if ("${CMAKE_SYSTEM}" MATCHES "Linux")
    target_link_libraries (cglm_bench -lm)
endif()

#
# doc
#
find_package (Doxygen)

if (DOXYGEN_FOUND)

  configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target (cglm_doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)

endif (DOXYGEN_FOUND)
