#CPFA-ARGoS

ARGoS (Autonomous Robots Go Swarming) is a multi-physics robot simulator. iAnt-ARGoS is an extension to ARGoS that implements the iAnt CPFA algorithm and provides a mechanism for performing experiments with iAnts.

**NOTE:** ARGoS is installed on the CS Linux machines. If you wish to use ARGoS on those machines, you only need to [download and compile](https://github.com/BCLab-UNM/iAnt-ARGoS#2-compiling-and-running-the-iant-cpfa-in-argos) this repository's code.

For more detailed information, please check the [iAnt-ARGoS wiki](https://github.com/BCLab-UNM/iAnt-ARGoS/wiki).

###Quick Start Installation Guide

In order to use the iAnt CPFA in ARGoS, you must first install ARGoS on your system then download and compile the code in this repo to run with ARGoS.

#####1. Installing ARGoS

ARGoS is available for Linux and Macintosh systems. It is currently not supported on Windows. Detailed installation instructions can be found on the [ARGoS Website](http://www.argos-sim.info/user_manual.php).

######Linux Installation

1. [Download](http://www.argos-sim.info/core.php) the appropriate binary package for your Linux system.
2. In Terminal, run the following command in the directory of your installation file:
  * for Ubuntu and KUbuntu:
    ```
    $ sudo dpkg -i argos3_simulator-*.deb
    ```

  * for OpenSuse:
    ```
    $ sudo rpm -i argos3_simulator-*.rpm
    ```

######Macintosh Installation

1. The Mac OSX installation of ARGoS uses the Homebrew Package Manager. If you don't have it, install Homebrew by using the following command in Terminal.
  ```
  $ ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  ```

2. Obtain the Homebrew Tap for ARGoS using the following command in Terminal.
  ```
  $ brew tap ilpincy/argos3
  ```

3. Once tapped, install ARGoS with the following command in Terminal. ARGoS and its required dependencies will be downloaded and installed using Homebrew.
  ```
  $ brew install bash-completion qt lua argos3
  ```

4. Once installed, you can update ARGoS with the following two commands in Terminal.
  ```
  $ brew update
  $ brew upgrade argos3
  ```

#####2. Compiling and Running the CPFA in ARGoS

Once ARGoS is installed on your system. You can download the files in this repo, compile them for your system, and run the iAnt CPFA in ARGoS.

1. Pull the code from this repository.

2. From the terminal, use build.sh script to compile the code:
  ```
  $ ./build.sh
  ```

CPFA-ARGoS includes CPFA evolver. This program uses a distributed version of ga-lib to evolve CPFA parameters. An example script for running cpfa_evolver is provided: evolve_EXAMPLE.sh.

CPFA evolver uses MPI to distribute argos evaluations across a cluster. An example machine file (moses_cluster) specifies the hostnames of the MPI nodes to use and the number of processes to run on each node.

evolve_EXAMPLE.sh takes two arguments. The number of MPI processes to run and the machine file name for the MPI cluster. 

Since the evolver relies on MPI packages that are not required for compiling the CPFA, compilation of the evolver is turned off by default. 

To build the CPFA evolver modify the build.sh script and change

```
cmake -DBUILD_EVOLVER=NO ..
```

to 

```
cmake -DBUILD_EVOLVER=YES ..
```

The evolver takes an experiment xml file argument that specifies the simulation parameters. The CPFA genome in that experient file is ignored and evolved parameters used instead. Make sure visualisation is turned off in this experiment file. 

######3. Running an Experiment
To run an experiment launch ARGoS with the XML configuration file for your system:
  ```
  $ argos3 -c experiments/experiment_file.xml
  ```


###Useful Links

| Description                                 | Website                             |
|:--------------------------------------------|:------------------------------------|
| official ARGoS website and documentation    | http://www.argos-sim.info/          |
| homebrew utility for Mac OSX installations  | http://brew.sh/                     |
| cmake utility information                   | http://www.cmake.org/documentation/ |
| running MPI programs                        | https://www.shodor.org/refdesk/Resources/Tutorials/RunningMPI/ |
