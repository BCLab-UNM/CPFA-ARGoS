#CPFA-ARGoS

ARGoS (Autonomous Robots Go Swarming) is a multi-physics robot simulator. iAnt-ARGoS is an extension to ARGoS that implements the CPFA-ARGoS algorithm and provides a mechanism for performing experiments with iAnts.

###Quick Start Installation Guide

The CPFA-ARGoS system has two components: the CPFA logic controllers that implement the CPFA algorithm in the ARGoS robot simulator, and the Genetic Algorithm that evolves the parameters that the CPFA algorithm uses. You can run the CPFA algorithm on ARGoS using OS X or Linux (see installation instructions below). To run the evolver you must use the Moses MPI cluster, which has 6 hosts with 24 cores. You can also use the cluster to run experiments without having to tie up your local machine.

#####0. Setting up your MPI environment 

#####A. Request an account on the MPI cluster by contacting the cluster admin (Matthew Fricke).
#####B. Login to pragma.cs.unm.edu using your account
#####C. Setup key based authentication so MPI can access the machines in the cluster

    $ ssh-keygen -t rsa

Add keychain key manager to your ~/.bashrc login script:

     ### START-Keychain ###
     # Let  re-use ssh-agent and/or gpg-agent between logins
     /usr/bin/keychain $HOME/.ssh/id_rsa
     source $HOME/.keychain/$HOSTNAME-sh
     ### End-Keychain ###

Save ~/.bashrc

and apply the changes:

    $ source ~/.bashrc

Copy the ssh key to the cluster machines and follow the instructions that come up:

    $ ssh-copy-id eros
    $ ssh-copy-id pragma
    $ ssh-copy-id ludus
    $ ssh-copy-id philia
    $ ssh-copy-id philautia
    $ ssh-copy-id agape

Now when you run the CPFA evolution run script MPI will work correctly.

In order to use the iAnt CPFA in ARGoS, you must first install ARGoS on your system then download and compile the code in this repo to run with ARGoS.

#####1. Installing ARGoS (ARGoS is already installed on the Moses cluster)

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

| Description                                | Website                                                        |
|:-------------------------------------------|:---------------------------------------------------------------|
| BCLab Git Branching Model                  | https://github.com/BCLab-UNM/git-branch-model                  |
| official ARGoS website and documentation   | http://www.argos-sim.info/                                     |
| homebrew utility for Mac OSX installations | http://brew.sh/                                                |
| cmake utility information                  | http://www.cmake.org/documentation/                            |
| running MPI programs                       | https://www.shodor.org/refdesk/Resources/Tutorials/RunningMPI/ |
