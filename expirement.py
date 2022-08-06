from nslc import NSLC
import csv

def main():
    results = []
    for i in range(1):
        nslc = NSLC()
        r = nslc.run()
        results.append(r)
    
    fields = ["Max Fitness Individual", "Number of different genomes", "Total objects placed"]

    with open('results.csv', 'w') as f:
        write = csv.writer(f)
        
        write.writerow(fields)
        write.writerows(results)

if __name__ == "__main__":
    main()