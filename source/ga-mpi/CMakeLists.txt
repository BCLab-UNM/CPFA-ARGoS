###############################################
# define galib-mpi shared object files
###############################################
cmake_minimum_required(VERSION 2.8)

file(GLOB GA-MPI_LIB_SRC
    "*.h"
    "*.C"
)

# Check for the MPI package.
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_library(GA-MPI STATIC ${GA-MPI_LIB_SRC})
target_link_libraries(GA-MPI ${MPI_LIBRARIES})