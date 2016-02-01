// This is the program entry point

// This program manages a population of experiments (CSimulation objects in ARGoS),
// spawns the sims into their own processes, reads the sim's fitness and uses
// galib to evolve the population to select the best CPFA parameters
// Can use openmp to manage process creation - like in the antbots.

// Plan:
// Define m fields (target placement instances).
// Define n genomes (parameterizatios of the CPFA)
// Define k openmpi workers and place in a queue.
// All combinations of fields and genomes produce n*m trials
// Allocate an evaluation to a worker and pop the queue.
// Workers runs argos instances to evaluate the trial they were assigned and send the fitness to the master
// When a worker send the fitness to the master the master resets the worker and pushes it onto the queue.
// When all trials have been processed pass the genomes and the average fitness for each to GALib.
// GALib generates a new population of n genomes. 
// Create n*m trials and allocate to the pool of workers as above. Repeat.
// Stop after j generations.

#include <ga/ga.h>
#include <ga/GARealGenome.h>

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>

#include <source/CPFA/CPFA_loop_functions.h>

// in one (and only one) place in the code that uses the string genome
#include <ga/GARealGenome.C>

#include <cmath>

#include <iostream>

using namespace std;


/****************************************/
/****************************************/

/*
 * Launch ARGoS to evaluate a genome.
 */
float LaunchARGoS(GAGenome& c_genome) {
  /* Convert the received genome to the actual genome type */
  GARealGenome& cRealGenome = dynamic_cast<GARealGenome&>(c_genome);
  /* The CSimulator class of ARGoS is a singleton. Therefore, to
   * manipulate an ARGoS experiment, it is enough to get its instance.
   * This variable is declared 'static' so it is created
   * once and then reused at each call of this function.
   * This line would work also without 'static', but written this way
   * it is faster. */
  static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
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
  cSimulator.Reset();

/* Return the result of the evaluation */
return fitness;
}


/****************************************/
/****************************************/


int main( int argc, char** argv)
{
  cout << "Evolving CPFA Parameters..." << endl;

  /*
   * Initialize GALIB
   */
  /* Create an allele whose values can be in the range [-10,10] */
  
  // The allel set is in the same order as Table 1 of Beyond Pheremones (Hecker and Moses)
  GARealGenomeAlleleSet alleles(-inf,inf);
    

  /* Create a genome with 10 genes, using LaunchARGoS() to evaluate it */
  GARealGenome cGenome(GENOME_SIZE, alleles, LaunchARGoS);


  /* Create and configure a basic genetic algorithm using the genome */
  GASimpleGA cGA(cGenome);
  cGA.minimize();                     // the objective function must be minimized
  cGA.populationSize(5);              // population size for each generation
  cGA.nGenerations(500);              // number of generations
  cGA.pMutation(0.05f);               // prob of gene mutation
  cGA.pCrossover(0.15f);              // prob of gene crossover
  cGA.scoreFilename("evolution.dat"); // filename for the result log
  cGA.flushFrequency(1);              // log the results every generation

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


  // /*
  //  * Launch the evolution, setting the random seed
  //  */
  // cGA.initialize(12345);
  // do {
  //    argos::LOG << "Generation #" << cGA.generation() << "...";
  //    cGA.step();
  //    argos::LOG << "done.";
  //    if(cGA.generation() % cGA.flushFrequency() == 0) {
  //       argos::LOG << "   Flushing...";
  //       /* Flush scores */
  //       cGA.flushScores();
  //       /* Flush best individual */
  //       FlushBest(dynamic_cast<const GARealGenome&>(cGA.statistics().bestIndividual()),
  //                 cGA.generation());
  //       argos::LOG << "done.";
  //    }
  //    LOG << std::endl;
  //    LOG.Flush();
  // }
  // while(! cGA.done());

  /*
   * Dispose of ARGoS stuff
   */
  cSimulator.Destroy();

  /* All is OK */
  return 0;
}

/****************************************/
/****************************************/
