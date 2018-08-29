if (BUILD_EVOLVER)
add_subdirectory(ga-mpi)
else()
endif()

add_subdirectory(Base)
add_subdirectory(CPFA)



