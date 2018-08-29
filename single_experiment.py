from xml.dom.minidom import parse, parseString
import random
import subprocess
import sys, platform
import pdb, time

class Random_Argos:

    def __init__(self, argos_xml = None):
        self.argos_xml = argos_xml

if __name__ == "__main__":
    #system = 'linux' if platform.system() == 'Linux' else 'mac'
    file = sys.argv[1]
    runs = int(sys.argv[2])

    this_run = Random_Argos("./experiments/"+file)
    count =0
    startTime =time.time()
    #result =[]
    for _ in range(runs):
        print "Run "+str(count)
        count = count+1
        output = subprocess.check_output(['argos3 -n -c ' + this_run.argos_xml], shell=True, stderr=subprocess.STDOUT)
    endTime = time.time()
    print 'The total running time is '+str((endTime-startTime)/60.0)+' minutes...'
