###############################################
# define galib-mpi shared object files
###############################################
cmake_minimum_required(VERSION 2.8)

ADD_CUSTOM_TARGET(
  libga-mpi.a ALL
  COMMAND ${CMAKE_MAKE_PROGRAM}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
  )

