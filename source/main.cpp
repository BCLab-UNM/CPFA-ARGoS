#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <unistd.h> // For usleep and optargs

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

// For shared variable between child and parent process
#include  <sys/ipc.h>
#include  <sys/shm.h>

float objective(GAGenome &);
float LaunchARGoS(GAGenome &);

int mpi_tasks, mpi_rank;

int n_trials = 20; // used by the objective function
double mutation_stdev = 1.00; // Gaussian mutation stdev - will be scaled by possible range

int GARealGaussianMutatorStdev(GAGenome &, float);

int main(int argc, char **argv)
{
  std::chrono::time_point<std::chrono::system_clock> program_start, program_end;
  program_start = std::chrono::system_clock::now();

  // MPI init - this has to happen before getopt() so the argv can be properly trimmed
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
                         
  char hostname[1024];                                                                                                       
  hostname[1023] = '\0';                                          
  gethostname(hostname, 1023);

  srand(time(NULL));

  double mutation_rate = 0.1;
  double crossover_rate = 0.05;
  int population_size = 28;
  int n_generations = 100;

  string ga_description = "GASimple, no elitism, linear fitness function scaling.";
  
  char c='h';
  // Handle command line arguments
  while ((c = getopt (argc, argv, "t:g:p:c:m:s:h:")) != -1)
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
      case 's':
        mutation_stdev = strtod(optarg, NULL);
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

  //printf("%s:\tworker %d ready.\n", hostname, mpi_rank);                                          

  // See if we've been given a seed to use (for testing purposes).  When you
  // specify a random seed, the evolution will be exactly the same each time
  // you use that seed number
  unsigned int seed = 0;
  for(int i=1 ; i<argc ; i++)
    if(strcmp(argv[i++],"seed") == 0)
      seed = atoi(argv[i]);
	
  // popsize / mpi_tasks must be an integer
  population_size = mpi_tasks * int((double)population_size/(double)mpi_tasks+0.999);
  
  if (mpi_rank==0)
    {
      printf("Population size: %d\nNumer of trials: %d\nNumber of generations: %d\nCrossover rate: %f\nMutation rate: %f\nMutation stdev: %f\nGenetic algorithm description: %s\nAllocated MPI workers: %d\n", population_size, n_trials, n_generations, crossover_rate, mutation_rate, mutation_stdev, ga_description.c_str(), mpi_tasks);
    }

  // Define the genome
  GARealAlleleSetArray allele_array;
  allele_array.add(0,1); // Probability of switching to search
  allele_array.add(0,0.0001); // Probability of returning to nest
  allele_array.add(0,4*M_PI); // Uninformed search variation
  allele_array.add(0,20); // Rate of informed search decay
  allele_array.add(0,20); // Rate of site fidelity
  allele_array.add(0,20); // Rate of laying pheremone
  allele_array.add(0,2); // Rate of pheremone decay

  
  // Create the template genome using the phenotype map we just made.
  GARealGenome genome(allele_array, objective);
  genome.crossover(GARealUniformCrossover);
  genome.mutator(GARealGaussianMutatorStdev); // Specify our version of the Gaussuan mutator
 
  // Now create the GA using the genome and run it.
  GASimpleGA ga(genome);
  GALinearScaling scaling;
  ga.maximize();		// Maximize the objective
  ga.populationSize(population_size);
  ga.nGenerations(n_generations);
  ga.pMutation(mutation_rate);
  ga.pCrossover(crossover_rate);
  ga.scaling(scaling);
  ga.elitist(gaFalse);
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
	results_output_stream << "Population size: " << population_size << "\nNumer of trials: " << n_trials << "\nNumber of generations: "<< n_generations<<"\nCrossover rate: "<< crossover_rate<<"\nMutation rate: " << mutation_rate << "\nMutation stdev: "<< mutation_stdev << "\nGenetic algorithm: " << ga_description << "Algorithm: CPFA\n" << "Number of searchers: 6\n" << "Number of targets: 256\n" << "Target distribution: power law" << endl;
	results_output_stream << "Generation" 
			      << ", " << "Compute Time (s)"
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

    while(!ga.done()){

      std::chrono::time_point<std::chrono::system_clock>generation_start, generation_end;
      if (mpi_rank == 0)
	{
	 generation_start = std::chrono::system_clock::now();
	}
      
      // Calculate the generation
      ga.step();

    if(mpi_rank == 0)
      {
	std::chrono::duration<double> generation_elapsed_seconds = generation_end-generation_start;
	ofstream results_output_stream;
	results_output_stream.open(results_file_name, ios::app);
       results_output_stream << ga.statistics().generation() 
			     << ", " << generation_elapsed_seconds.count()
			     << ", " << ga.statistics().convergence()
			     << ", " << ga.statistics().current(GAStatistics::Mean)
			     << ", " << ga.statistics().current(GAStatistics::Maximum)
			     << ", " << ga.statistics().current(GAStatistics::Minimum)
			     << ", " << ga.statistics().current(GAStatistics::Deviation)
			     << ", " << ga.statistics().current(GAStatistics::Diversity);
       
	  for (int i = 0; i < GENOME_SIZE; i++)
	    results_output_stream << ", " << dynamic_cast<const GARealGenome&>(ga.population().best()).gene(i);
	
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

  program_end = std::chrono::system_clock::now();
 
  std::chrono::duration<double> program_elapsed_seconds = program_end-program_start;

  if(mpi_rank == 0)
    printf("Run time was %f seconds\n", program_elapsed_seconds.count());

  return 0;
  }
 
// The mutation operator based on the original from GALib but adds stdev
// The Gaussian mutator picks a new value based on a Gaussian distribution
// around the current value.  We respect the bounds (if any).
int GARealGaussianMutatorStdev(GAGenome& g, float pmut)
{
  GA1DArrayAlleleGenome<float> &child=
    DYN_CAST(GA1DArrayAlleleGenome<float> &, g);
  register int n, i;
  if(pmut <= 0.0) return(0);

  float nMut = pmut * (float)(child.length());
  int length = child.length()-1;
  if(nMut < 1.0){// we have to do a flip test on each element
    nMut = 0;
    for(i=length; i>=0; i--){
      float value = child.gene(i);
      if(GAFlipCoin(pmut)){
	if(child.alleleset(i).type() == GAAllele::ENUMERATED ||
	   child.alleleset(i).type() == GAAllele::DISCRETIZED)
	  value = child.alleleset(i).allele();
	else if(child.alleleset(i).type() == GAAllele::BOUNDED){
	  value += GAUnitGaussian()*mutation_stdev*(child.alleleset(i).upper()-child.alleleset(i).lower()); // since the standard deviation varies proportionally to a constant multiplier 
	  value = GAMax(child.alleleset(i).lower(), value);
	  value = GAMin(child.alleleset(i).upper(), value);
	}
	child.gene(i, value);
	nMut++;
      }
    }
  }
  else{// only mutate the ones we need to
    for(n=0; n<nMut; n++){
      int idx = GARandomInt(0,length);
      float value = child.gene(idx);
      if(child.alleleset(idx).type() == GAAllele::ENUMERATED ||
	 child.alleleset(idx).type() == GAAllele::DISCRETIZED)
	value = child.alleleset(idx).allele();
      else if(child.alleleset(idx).type() == GAAllele::BOUNDED){
	value += GAUnitGaussian()*mutation_stdev*(child.alleleset(i).upper()-child.alleleset(i).lower()); // since the standard deviation varies proportionally to a constant multiplier 
	value = GAMax(child.alleleset(idx).lower(), value);
	value = GAMin(child.alleleset(idx).upper(), value);
      }
      child.gene(idx, value);
    }
  }
  return((int)nMut);
}

// The objective function used by GAlib

float objective(GAGenome &c)
{
  float avg = 0;
  
  // For timing
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  for (int i = 0; i < n_trials; i++) avg += LaunchARGoS(c);
  
  avg /= n_trials;
  end = std::chrono::system_clock::now();
  
  std::chrono::duration<double> elapsed_seconds = end-start;

  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);     

      char hostname[1024];              
      hostname[1023] = '\0';                                          
      gethostname(hostname, 1023);     
 
       
      printf("Worker %d on %s evaluated genome [", mpi_rank, hostname);
      for (int i = 0; i < GENOME_SIZE; i++)
	printf("%f ",dynamic_cast<const GARealGenome&>(c).gene(i));
      printf("] in %f seconds. ", elapsed_seconds.count());
      printf("Fitness: %f.\n", avg );
      return avg;
}

/*
 * Launch ARGoS to evaluate a genome.
 */
float LaunchARGoS(GAGenome& c_genome) 
{
  float fitness = 0;

  int    ShmID;
  float*   ShmPTR;
  
  // Allocate the shared memory to use between us and the child argos process
  ShmID = shmget(IPC_PRIVATE, sizeof(float), IPC_CREAT | 0666);
  if (ShmID < 0) {
    printf("*** shmget error (server) ***\n");
  }
  
  ShmPTR = (float*) shmat(ShmID, NULL, 0);
  if ((float*) ShmPTR == (float*)-1) {
    printf("*** shmat error (server) ***\n");
  }

  *ShmPTR = 0;

  pid_t pid = fork();

  if (pid == 0)
    {
      // In child process - run the argos3 simulation

      /* Convert the received genome to the actual genome type */
      GARealGenome& cRealGenome = dynamic_cast<GARealGenome&>(c_genome);
  
      Real* cpfa_genome = new Real[GENOME_SIZE];

      for (int i = 0; i < GENOME_SIZE; i++)
	cpfa_genome[i] = cRealGenome.gene(i);
       
            
      /*
      printf("%s: worker %d started a genome evaluation. (%f, %f, %f, %f, %f, %f, %f)\n", hostname, mpi_rank, 
	     cpfa_genome[0],
	     cpfa_genome[1],
	     cpfa_genome[2],
	     cpfa_genome[3],
	     cpfa_genome[4],
	     cpfa_genome[5],
	     cpfa_genome[6]);
      */

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
     
      *ShmPTR = cLoopFunctions.Score();;
  

      //cLoopFunctions.Destroy();
      cSimulator.Destroy();

      
	delete [] cpfa_genome;

      _Exit(0); 
    }
  
  // In parent - wait for child to finish
  int status = wait(&status);

  fitness = *ShmPTR;  

  // Release shared memory
  shmdt((void *) ShmPTR);
  shmctl(ShmID, IPC_RMID, NULL);

  /* Return the result of the evaluation */  
  return fitness;
}
