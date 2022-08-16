from nslc import NSLC
from medea import MEDEA
from custom.world_observers import NSLCWorldObserver
import csv, os, sys, getopt

def main(argv):

    method = '0'
    difficulty = '1'

    try:
        opts, args = getopt.getopt(argv, "m:d:")
        for opt, arg in opts:
            if opt == '-m':
                method = arg
            elif opt == '-d':
                difficulty = arg
    except getopt.error as err:
        print(err)

    if difficulty == '1':
        NSLCWorldObserver.NB_OBJECTS_EASY = 50
        NSLCWorldObserver.NB_OBJECTS_MEDIUM = 20
        NSLCWorldObserver.NB_OBJECTS_HARD = 5
    elif difficulty == '2':
        NSLCWorldObserver.NB_OBJECTS_EASY = 25
        NSLCWorldObserver.NB_OBJECTS_MEDIUM = 30
        NSLCWorldObserver.NB_OBJECTS_HARD = 20
    elif difficulty == '3':
        NSLCWorldObserver.NB_OBJECTS_EASY = 15
        NSLCWorldObserver.NB_OBJECTS_MEDIUM = 25
        NSLCWorldObserver.NB_OBJECTS_HARD = 35
    else:
        print('Invalid difficulty parameter')
        sys.exit()

    NSLCWorldObserver.NB_OBJECTS = NSLCWorldObserver.NB_OBJECTS_EASY + NSLCWorldObserver.NB_OBJECTS_MEDIUM + NSLCWorldObserver.NB_OBJECTS_HARD   

    results = []
    r = None

    if method == '0':
        nslc = NSLC()
        r = nslc.run()
    elif method == '1':
        medea = MEDEA()
        r = medea.run()
    else:
        print('Invalid method parameter')
        sys.exit()
        
    results = r

    perf_filename = 'performance_' + str(method) + '_' + str(difficulty) + '.csv'
    if os.path.exists(perf_filename):
        with open(perf_filename, 'a') as f:
            write = csv.writer(f)
            write.writerow(results[0])
    
    else:
        fields = ["Max Fitness Individual", "Average Fitness", "Number of different genomes", "Total objects placed", "End Iteration"]

        with open(perf_filename, 'w') as f:
            write = csv.writer(f)
            
            write.writerow(fields)
            write.writerow(results[0])

    div_filename = 'diversity_' + str(method) + '_' + str(difficulty) + '.csv'
    if os.path.exists(div_filename):
        with open(div_filename, 'a') as f:
            write = csv.writer(f)
            write.writerow([])
            write.writerows(results[1])
    
    else:
        fields = ["Generation", "Time A", "Time B", "Time C", "Distance"]

        with open(div_filename, 'w') as f:
            write = csv.writer(f)
            
            write.writerow(fields)
            write.writerows(results[1])

if __name__ == "__main__":
    main(sys.argv[1:])