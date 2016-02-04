#include <stdio.h>
#include <math.h>
#include <cstdlib>
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

int n_trials = 10; // used by the objective function
double mutation_stdev = 0.1; // Used by LaunchArgos function.

int main(int argc, char **argv)
{
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  double mutation_rate = 0.1;
  double crossover_rate = 0.1;
  int population_size = 50;
  int n_generations = 100;

  
  char c='h';
  // Handle command line arguments
  while ((c = getopt (argc, argv, "tgp:")) != -1)
    switch (c)
      {
      case 't':
	n_trials = atoi(optarg);
	break;
      case 'g':
        n_generations = atoi(optarg);
        break;
      case 'p':
        population_size = atoi(optarg);
        break;
      case 'c':
	crossover_rate = strtod(optarg, NULL);
	break;
      case 'm':
        mutation_rate = strtod(optarg, NULL);
	break;
      case 'h':
	printf("Usage: %s -p {population size} -g {number of generations} -t {number of trials} -c {crossover rate} -m {mutation rate} -s {mutation standard deviation}", argv[0]);
	break;
      case '?':
        if (optopt == 'p')
          fprintf (stderr, "Option -%c requires an argument specifying the population size.\n", optopt);
	else if (optopt == 'g')
	  fprintf (stderr, "Option -%c requires an argument specifying the number of generations.\n", optopt);
	else if (optopt == 't')
	  fprintf (stderr, "Option -%c requires an argument specifying the number of trials.\n", optopt);
	else if (optopt == 'c')
	  fprintf (stderr, "Option -%c requires an argument specifying the crossover rate.\n", optopt);
	else if (optopt == 'm')
	  fprintf (stderr, "Option -%c requires an argument specifying the mutation rate.\n", optopt);
	else if (optopt == 's')
	  fprintf (stderr, "Option -%c requires an argument specifying the mutation standard deviation.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
	printf("Usage: %s -p {population size} -g {number of generations} -t {number of trials} -c {crossover rate} -m {mutation rate} -s {mutation standard deviation}", argv[0]);
        abort ();
      }
  

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
	
  // popsize / mpi_tasks must be an integer
  population_size = mpi_tasks * int((double)population_size/(double)mpi_tasks+0.999);
  
  // Define the genome
  GARealAlleleSetArray allele_array;
  allele_array.add(0,1/mutation_stdev); // Probability of switching to search
  allele_array.add(0,1/mutation_stdev); // Probability of returning to nest
  allele_array.add(0,4*M_PI/mutation_stdev); // Uninformed search variation
  allele_array.add(0,exp(5)/mutation_stdev); // Rate of informed search decay
  allele_array.add(0,20/mutation_stdev); // Rate of site fidelity
  allele_array.add(0,20/mutation_stdev); // Rate of laying pheremone
  allele_array.add(0,exp(10)/mutation_stdev); // Rate of pheremone decay

  
  // Create the template genome using the phenotype map we just made.
  GARealGenome genome(allele_array, objective);
  genome.crossover(GARealUniformCrossover);
  genome.mutator(GARealGaussianMutator);

  // Now create the GA using the genome and run it. We'll use sigma truncation
  // scaling so that we can handle negative objective scores.
  GASimpleGA ga(genome);
  GALinearScaling scaling;
  ga.maximize();		// Maxamize the objective
  ga.populationSize(population_size);
  ga.nGenerations(n_generations);
  ga.pMutation(mutation_rate);
  ga.pCrossover(crossover_rate);
  ga.scaling(scaling);
  ga.elitist(gaTrue);
  if(mpi_rank == 0)
    ga.scoreFilename("evolution.txt");
  else
    ga.scoreFilename("/dev/null");
  ga.recordDiversity(gaTrue);
  ga.scoreFrequency(1);
  ga.flushFrequency(1);
  ga.selectScores(GAStatistics::AllScores);
  

  // Pass MPI data to the GA class
  ga.mpi_rank(mpi_rank);
  ga.mpi_tasks(mpi_tasks);
  //ga.evolve(seed);

    // Name the results file with the current time and date
 time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    stringstream ss;

    ss << "results/CPFA-evolution-"
       << "powerlaw-6rovers" << '-'
       <<GIT_BRANCH<<"-"<<GIT_COMMIT_HASH<<"-"
       << (now->tm_year) << '-'
       << (now->tm_mon + 1) << '-'
       <<  now->tm_mday << '-'
       <<  now->tm_hour << '-'
       <<  now->tm_min << '-'
       <<  now->tm_sec << ".csv";

    string results_file_name = ss.str();

    if (mpi_rank == 0)
      {
    // Write output file header
    ofstream results_output_stream;
	results_output_stream.open(results_file_name, ios::app);
	results_output_stream << "Generation" 
			      << ", " << "Convergence"
			      << ", " << "Mean"
			      << ", " << "Maximum"
			      << ", " << "Minimum"
			      << ", " << "Standard Deviation"
			      << ", " << "Diversity"
			      << ", " << "ProbabilityOfSwitchingToSearching"
			      << ", " << "ProbabilityOfReturningToNest"
			      << ", " << "UninformedSearchVariation"
			      << ", " << "RateOfInformedSearchDecay"
			      << ", " << "RateOfSiteFidelity"
			      << ", " << "RateOfLayingPheromone"
			      << ", " << "RateOfPheromoneDecay";

	results_output_stream << endl;
	results_output_stream.close();
      }

	// initialize the ga since we are not using the evolve function
	ga.initialize();

  cout << "evolving...";
  while(!ga.done()){
    ga.step();

    if(mpi_rank == 0)
      {
	printf("Generation %d", ga.generation());
	ofstream results_output_stream;
	results_output_stream.open(results_file_name, ios::app);
       results_output_stream << ga.statistics().generation() 
			      << ", " << ga.statistics().convergence()
			      << ", " << ga.statistics().current(GAStatistics::Mean)
			      << ", " << ga.statistics().current(GAStatistics::Maximum)
			      << ", " << ga.statistics().current(GAStatistics::Minimum)
			      << ", " << ga.statistics().current(GAStatistics::Deviation)
			      << ", " << ga.statistics().current(GAStatistics::Diversity);
	
	  for (int i = 0; i < GENOME_SIZE; i++)
	    results_output_stream << ", " << dynamic_cast<const GARealGenome&>(ga.statistics().bestIndividual()).gene(i)*mutation_stdev;
	
	results_output_stream << endl;
	results_output_stream.close();
      }
  }

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
  float avg = 0;
  for (int i = 0; i < n_trials; i++)
    avg += LaunchARGoS(c);
  avg /= n_trials;
  
  printf("Fitness in objective function (mean of %d trials): %f\n", n_trials, avg );
  return avg;
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
	cpfa_genome[i] = cRealGenome.gene(i)*mutation_stdev; // Convert back from genes transormed to simulate mutation stdev
       
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
