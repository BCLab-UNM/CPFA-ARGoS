#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h> // For usleep
#include <ga-mpi/ga.h>
#include <ga-mpi/std_stream.h>
#include <ctime> // For clock()
#include <chrono> // For clock()
#include "mpi.h"

float objective(GAGenome &);
float LaunchARGoS(GAGenome &)

int mpi_tasks, mpi_rank;

int main(int argc, char **argv)
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

  // MPI init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  printf("Hello from process %d of %d\n", mpi_rank, mpi_tasks);

  // See if we've been given a seed to use (for testing purposes).  When you
  // specify a random seed, the evolution will be exactly the same each time
  // you use that seed number
  unsigned int seed = 0;
  for(int i=1 ; i<argc ; i++)
    if(strcmp(argv[i++],"seed") == 0)
      seed = atoi(argv[i]);
	
  // Declare variables for the GA parameters and set them to some default values.
  int popsize  = 100; // Population
  int ngen     = 100; // Generations
  float pmut   = 0.03;
  float pcross = 0.65;

  // popsize / mpi_tasks must be an integer
  popsize = mpi_tasks * int((double)popsize/(double)mpi_tasks+0.999);

  // Create the phenotype for two variables.  The number of bits you can use to
  // represent any number is limited by the type of computer you are using.
  // For this case we use 10 bits for each var, ranging the square domain [0,5*PI]x[0,5*PI]
  GABin2DecPhenotype map;
  map.add(10, 0.0, 5.0 * M_PI);
  map.add(10, 0.0, 5.0 * M_PI);

  // Create the template genome using the phenotype map we just made.
  GABin2DecGenome genome(map, objective);

  // Now create the GA using the genome and run it. We'll use sigma truncation
  // scaling so that we can handle negative objective scores.
  GASimpleGA ga(genome);
  GALinearScaling scaling;
  ga.minimize();		// by default we want to minimize the objective
  ga.populationSize(popsize);
  ga.nGenerations(ngen);
  ga.pMutation(pmut);
  ga.pCrossover(pcross);
  ga.scaling(scaling);
  if(mpi_rank == 0)
    ga.scoreFilename("evolution.txt");
  else
    ga.scoreFilename("/dev/null");
  ga.scoreFrequency(1);
  ga.flushFrequency(1);
  ga.selectScores(GAStatistics::AllScores);
  // Pass MPI data to the GA class
  ga.mpi_rank(mpi_rank);
  ga.mpi_tasks(mpi_tasks);
  ga.evolve(seed);

  // Dump the GA results to file
  if(mpi_rank == 0)
    {
      genome = ga.statistics().bestIndividual();
      printf("GA result:\n");
      printf("x = %f, y = %f\n",
	     genome.phenotype(0), genome.phenotype(1));
    }

  MPI_Finalize();

   end = std::chrono::system_clock::now();
 
    std::chrono::duration<double> elapsed_seconds = end-start;

  if(mpi_rank == 0)
    printf("Run time was %f seconds\n", elapsed_seconds.count());

  return 0;
}
 
float objective(GAGenome &c)
{
  float fitness = LaunchARGoS(c);
  return fitness;
}

/*
 * Launch ARGoS to evaluate a genome.
 */
float LaunchARGoS(GAGenome& c_genome) {

/*
   * Initialize ARGoS
   */
  /* The CSimulator class of ARGoS is a singleton. Therefore, to
   * manipulate an ARGoS experiment, it is enough to get its instance */
  argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
  /* Set the .argos configuration file
   * This is a relative path which assumed that you launch the executable
   * from argos3-examples (as said also in the README) */
  cSimulator.SetExperimentFileName("experiments/CPFA.xml");
  /* Load it to configure ARGoS */
  cSimulator.LoadExperiment();

  /* Convert the received genome to the actual genome type */
  GARealGenome& cRealGenome = dynamic_cast<GARealGenome&>(c_genome);
  
  /* Get a reference to the loop functions */
  static CPFA_loop_functions& cLoopFunctions = dynamic_cast<CPFA_loop_functions&>(cSimulator.GetLoopFunctions());

  Real fitness;
  Real* cpfa_genome = new Real[GENOME_SIZE];

  /* Configure the controller with the genome */
  cLoopFunctions.ConfigureFromGenome(cpfa_genome);

  /* Run the experiment */
  cSimulator.Execute();

  /* Update performance */
  fitness = cLoopFunctions.Score();
  
  /* This internally calls also CEvolutionLoopFunctions::Reset(). */
  // cSimulator.Reset();

/* Return the result of the evaluation */
return fitness;
}
