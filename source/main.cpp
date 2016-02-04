#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h> // For usleep

// For shared memory management
#include <sys/mman.h>

#include <ctime> // For clock()
#include <chrono> // For clock()
#include <mpi.h>

#include <sys/wait.h> // For wait(pid)

#include <limits> // For float max

// GA MPI Headers
#include <ga-mpi/ga.h>
#include <ga-mpi/std_stream.h>
#include <ga-mpi/GARealGenome.h>
#include <ga-mpi/GARealGenome.C>

// Argos headers
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <source/CPFA/CPFA_loop_functions.h>

// For timing
#include <ctime> // For clock()
#include <chrono> // For clock()

// Shared variable between child and parent process
static float *shared_fitness;

float objective(GAGenome &);
float LaunchARGoS(GAGenome &);

int mpi_tasks, mpi_rank;

int main(int argc, char **argv)
{
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  float max_float = std::numeric_limits<float>::max();

  // Allocate the shared memory to use between us and the child argos process
  shared_fitness = static_cast<float*>(mmap((caddr_t)NULL, sizeof (*shared_fitness), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));

  // MPI init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
                         
  char hostname[1024];                                                                                                       
  hostname[1023] = '\0';                                          
  gethostname(hostname, 1023);                        
  printf("%s:\tworker %d ready.\n", hostname, mpi_rank);                                          

  // See if we've been given a seed to use (for testing purposes).  When you
  // specify a random seed, the evolution will be exactly the same each time
  // you use that seed number
  unsigned int seed = 0;
  for(int i=1 ; i<argc ; i++)
    if(strcmp(argv[i++],"seed") == 0)
      seed = atoi(argv[i]);
	
  // Declare variables for the GA parameters and set them to some default values.
  int popsize  = 50; // Population
  int ngen     = 100; // Generations
  float pmut   = 0.1;
  float pcross = 0.05;

  // popsize / mpi_tasks must be an integer
  popsize = mpi_tasks * int((double)popsize/(double)mpi_tasks+0.999);
  
  // Define the genome
  GARealAlleleSetArray allele_array;
  allele_array.add(0,1); // Probability of switching to search
  allele_array.add(0,1); // Probability of returning to nest
  allele_array.add(0,1); // Uninformed search variation
  allele_array.add(0,exp(5)); // Rate of informed search decay
  allele_array.add(0,20); // Rate of site fidelity
  allele_array.add(0,20); // Rate of laying pheremone
  allele_array.add(0,exp(10)); // Rate of pheremone decay

  
  // Create the template genome using the phenotype map we just made.
  GARealGenome genome(allele_array, objective);
  genome.crossover(GARealUniformCrossover);
  genome.mutator(GARealGaussianMutator);

  // Now create the GA using the genome and run it. We'll use sigma truncation
  // scaling so that we can handle negative objective scores.
  GASimpleGA ga(genome);
  GALinearScaling scaling;
  ga.maximize();		// Maxamize the objective
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

  // Display the GA's progress
  if(mpi_rank == 0)
    {
      cout << ga.statistics() << " " << ga.parameters() << endl;
    }

  MPI_Finalize();

  end = std::chrono::system_clock::now();
 
  std::chrono::duration<double> elapsed_seconds = end-start;

  if(mpi_rank == 0)
    printf("Run time was %f seconds\n", elapsed_seconds.count());

  return 0;
}
 
// The objective function used by GAlib

float objective(GAGenome &c)
{
  float fitness = LaunchARGoS(c);
  printf("Fitness in objective function: %f\n", fitness );
  return fitness;
}

/*
 * Launch ARGoS to evaluate a genome.
 */
float LaunchARGoS(GAGenome& c_genome) 
{
  float fitness = 0;

  *shared_fitness = 0; // init the shared variable

  pid_t pid = fork();

  if (pid == 0)
    {
      // In child process - run the argos3 simulation

      /* Convert the received genome to the actual genome type */
      GARealGenome& cRealGenome = dynamic_cast<GARealGenome&>(c_genome);
  
      Real* cpfa_genome = new Real[GENOME_SIZE];

      for (int i = 0; i < GENOME_SIZE; i++)
	cpfa_genome[i] = cRealGenome.gene(i);
       
      std::chrono::time_point<std::chrono::system_clock> start, end;
      start = std::chrono::system_clock::now();
      MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);     
      
      char hostname[1024];              
      hostname[1023] = '\0';                                          
      gethostname(hostname, 1023);     
      printf("%s: worker %d started a genome evaluation. (%f, %f, %f, %f, %f, %f, %f)\n", hostname, mpi_rank, 
	     cpfa_genome[0],
	     cpfa_genome[1],
	     cpfa_genome[2],
	     cpfa_genome[3],
	     cpfa_genome[4],
	     cpfa_genome[5],
	     cpfa_genome[6]);

      /* Redirect LOG and LOGERR to dedicated files to prevent clutter on the screen */

      std::ofstream cLOGFile("argos_logs/ARGoS_LOG_" + ToString(::getpid()), std::ios::out);
      LOG.DisableColoredOutput();
      LOG.GetStream().rdbuf(cLOGFile.rdbuf());
      std::ofstream cLOGERRFile("argos_logs/ARGoS_LOGERR_" + ToString(::getpid()), std::ios::out);
      LOGERR.DisableColoredOutput();
      LOGERR.GetStream().rdbuf(cLOGERRFile.rdbuf());

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

      /* Get a reference to the loop functions */
      CPFA_loop_functions& cLoopFunctions = dynamic_cast<CPFA_loop_functions&>(cSimulator.GetLoopFunctions());


      /* This internally calls also CEvolutionLoopFunctions::Reset(). */
      // cSimulator.Reset();

      /* Configure the controller with the genome */
      cLoopFunctions.ConfigureFromGenome(cpfa_genome);

      /* Run the experiment */
      cSimulator.Execute();

      /* Update performance */
      *shared_fitness = cLoopFunctions.Score();
  

      //cLoopFunctions.Destroy();
      cSimulator.Destroy();

      end = std::chrono::system_clock::now();
                     
      std::chrono::duration<double> elapsed_seconds = end-start;
      printf("%s: worker %d completed a genome evaluation in %f seconds. Fitness was %f\n", hostname, mpi_rank, elapsed_seconds.count(), *shared_fitness);                                          

	delete [] cpfa_genome;

      _Exit(0); 
    }
  
  // In parent - wait for child to finish
  int status = wait(&status);
  
  /* Return the result of the evaluation */
  fitness = *shared_fitness;

  return fitness;
}
