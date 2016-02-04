cmake_minimum_required(VERSION 2.8.12)
project(CPFA-ARGoS)

##
## Get the Git hash for this code
##
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
add_definitions("-DGIT_BRANCH=\"${GIT_BRANCH}\"")


# Use C+11
if (NOT APPLE)
set(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

# Deactivate RPATH for MacOSX.
if (APPLE)
set(CMAKE_MACOSX_RPATH 0)
endif()

# Set the path where CMake will find additional scripts.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Find the ARGoS package.
find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

# Check for required libraries to compile code depending on Qt and OpenGL.
include(ARGoSCheckQTOpenGL)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

# Find the ARGoS package
find_package(PkgConfig)
pkg_check_modules(ARGOS REQUIRED argos3_simulator)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ARGOS_PREFIX}/share/argos3/cmake)

# Find the GSL package.
find_package(GSL REQUIRED)

# Find the LUA package.
find_package(Lua52 REQUIRED)

# Set ARGoS include directory.
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS} ${GSL_INCLUDE_DIR} ${LUA_INCLUDE_DIR} source)

# Set ARGoS link directory.
link_directories(${ARGOS_LIBRARY_DIRS} source/ga-mpi)

# Descend into the source code directory.
add_subdirectory(source)

add_executable(cpfa_evolver source/main.cpp)

target_link_libraries(cpfa_evolver CPFA_controller
				   CPFA_loop_functions
				   ga-mpi	    
				   #${GALIB_LIBRARIES}
				   ${MPI_LIBRARIES}		   
				   argos3core_simulator)


