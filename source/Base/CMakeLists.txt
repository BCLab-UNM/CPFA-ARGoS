
###############################################
# define shared object files
###############################################

add_library(BaseController SHARED BaseController.h
                                      BaseController.cpp)

add_library(Food           SHARED Food.h
                                      Food.cpp)

add_library(Pheromone           SHARED Pheromone.h
                                      Pheromone.cpp)

###############################################
# link shared object files to dependencies
###############################################

target_link_libraries(BaseController
                      argos3core_simulator
                      argos3plugin_simulator_footbot
                      argos3plugin_simulator_genericrobot)

target_link_libraries(Food)
target_link_libraries(Pheromone)

###############################################
# some notes...
#     argos3core_simulator
#         = required for ARGoS integration
#     argos3plugin_simulator_footbot
#         = required for footbot sensors/actuators
#     argos3plugin_genericrobot
#         = required for general sensors/actuators
###############################################