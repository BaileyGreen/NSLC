from method import Method
from custom.world_observers import NSLCWorldObserver
import csv, os, sys, getopt

def main(argv):

    method = '0'
    difficulty = '1'

    #Get arguments
    try:
        opts, args = getopt.getopt(argv, "m:d:")
        for opt, arg in opts:
            if opt == '-m':
                method = arg
            elif opt == '-d':
                difficulty = arg
    except getopt.error as err:
        print(err)

    #If easy difficulty
    if difficulty == '1':
        NSLCWorldObserver.NB_OBJECTS_SMALL = 50
        NSLCWorldObserver.NB_OBJECTS_MEDIUM = 20
        NSLCWorldObserver.NB_OBJECTS_LARGE = 5
    #If medium difficulty
    elif difficulty == '2':
        NSLCWorldObserver.NB_OBJECTS_SMALL = 25
        NSLCWorldObserver.NB_OBJECTS_MEDIUM = 30
        NSLCWorldObserver.NB_OBJECTS_LARGE = 20
    #If hard difficulty
    elif difficulty == '3':
        NSLCWorldObserver.NB_OBJECTS_SMALL = 15
        NSLCWorldObserver.NB_OBJECTS_MEDIUM = 25
        NSLCWorldObserver.NB_OBJECTS_LARGE = 35
    #Invalid difficulty parameter
    else:
        print('Invalid difficulty parameter')
        sys.exit()

    #Set total resources
    NSLCWorldObserver.NB_OBJECTS = NSLCWorldObserver.NB_OBJECTS_SMALL + NSLCWorldObserver.NB_OBJECTS_MEDIUM + NSLCWorldObserver.NB_OBJECTS_LARGE   

    results = []
    r = None

    #Run the method
    if method == '0' or method == '1':
        m = Method(method)
        r = m.run()
    #Invalid method parameter
    else:
        print('Invalid method parameter')
        sys.exit()

    #Get the results 
    results = r

    #Write performance results to csv
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

    #Write diversity results to csv
    div_filename = 'diversity_' + str(method) + '_' + str(difficulty) + '.csv'
    if os.path.exists(div_filename):
        with open(div_filename, 'a') as f:
            write = csv.writer(f)
            write.writerow([])
            write.writerows(results[1])
    
    else:
        fields = ["Generation", "Time Small", "Time Medium", "Time Large", "Swarm Distance to Gathering Zone"]

        with open(div_filename, 'w') as f:
            write = csv.writer(f)
            
            write.writerow(fields)
            write.writerows(results[1])

if __name__ == "__main__":
    main(sys.argv[1:])