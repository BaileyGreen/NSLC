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
        
    results.append(r)
    filename = 'results_' + str(method) + '_' + str(difficulty) + '.csv'
    if os.path.exists(filename):
        with open(filename, 'a') as f:
            write = csv.writer(f)
            write.writerows(results)
    
    else:
        fields = ["Max Fitness Individual", "Average Fitness", "Number of different genomes", "Total objects placed", "End Iteration"]

        with open(filename, 'w') as f:
            write = csv.writer(f)
            
            write.writerow(fields)
            write.writerows(results)

if __name__ == "__main__":
    main(sys.argv[1:])