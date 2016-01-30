// This is the program entry point

// This program manages a population of experiments (CSimulation objects in ARGoS),
// spawns the sims into their own processes, reads the sim's fitness and uses
// galib to evolve the population to select the best CPFA parameters
// Can use openmp to manage process creation - like in the antbots.
