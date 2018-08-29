mkdir -p launch
logfilename="${1##*/}_$(date +%F_%H:%M:%S,%N)"

nohup mpirun -n $1 -machinefile $2 run_2_3types_ga.sh Powerlaw_dynamic_MPFA_10by10.xml Random_dynamic_MPFA_10by10.xml Cluster_dynamic_MPFA_10by10.xml>>launch/${logfilename}_stdout.log 2>> launch/${logfilename}_stderr.log &


#Command example: ./currentscript.sh run_2_3types_ga.sh Cluster_4nests.xml Powerlaw_4nests.xml Random_4nests.xml

