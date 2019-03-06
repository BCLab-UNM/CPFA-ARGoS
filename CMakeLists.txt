cmake_minimum_required(VERSION 2.8.12)
project(CPFA-ARGoS)

# For verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)

#CMAKE_BUILD_TYPE(Release)
#ARGOS_BUILD_NATTIVE(ON)

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

# Pass the git hash to the compiler
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

link_directories(${ARGOS_LIBRARY_DIRS} build)

# Descend into the source code directory and build the CPFA libraries. This is all that's needed to run experiments.
add_subdirectory(source)

# Only build the evolver program if selected.
if (BUILD_EVOLVER)

# Check for the MPI package.
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# Find the boost filesystem package.
find_package( Boost REQUIRED COMPONENTS system filesystem )
include_directories( ${Boost_INCLUDE_DIRS} )

# Specify the evolver program as a compile target.
add_executable(cpfa_evolver source/evolver.cpp)

# Specify the libraries needed to compile the evolver.
target_link_libraries(cpfa_evolver CPFA_controller
				   CPFA_loop_functions
				   GA-MPI   
				   ${MPI_LIBRARIES}
		   		   ${Boost_LIBRARIES}
				   argos3core_simulator)
else()
endif()