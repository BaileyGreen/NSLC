# NSLC
This project contains experiments to compare the **mEDEA** method with our hybridized method **(NSLC)** combining **mEDEA** and **novelty search with local competition**  

The experiments are done with a collective gathering task where the swarm of robots must push resources into a gathering zone. There are three different types of resources, small, medium, and large requiring one, two and three robots to be pushed respectively. There are also three different task environments, easy, medium and hard each containing a different proportion of small, medium and large resources. 

# Installation
To install this project roborobo4 must first be installed. The instructions on how to do this can be found here: [https://github.com/nekonaute/roborobo4](https://github.com/nekonaute/roborobo4)

After installing roborobo4 make sure your conda environment is activated using the following command, replacing **yourenv** with the name of your conda environment:

    conda activate yourenv

You can then run the experiment using the following command (note this runs the experiment using the **NSLC** method on the easiest task environment, see arguments section to change this):

    python experiment.py

# Arguments
There are two optional arguments when running the experimentto change the method and task environment difficulty. The default values for these arguments are the **NSLC** method and the easiest task environment difficulty.

To change the method **-m** can be appended to the run command followed by:

 - 0 for **NSLC** method
 - 1 for **mEDEA** method

To change the task environemt difficulty **-d** can be appended to the run command followed by:

 - 1 for easy task environment
 - 2 for medium task environment
 - 3 for hard task environment

An example of using these arguments to run the experiment using the **mEDEA** method on the hardest difficulty is as follows: 

    python experiment.py -m 1 -d 3

# Results
Once the experiment has completed the results from that run are appended to a performance and diversity csv file. If these csv files do not exist they will be created automatically. A separate csv file for performance and diversity will be created for each method and task environment combination that is run. 

The results from the experiment we conducted in our paper can be found in the Results folder of this repo. It contains **Diversity.xslx** and **Performance.xslx** for the diversity and performance results, which contain the filtered data as well as graphs and statistical tests. The Results folder also contains the raw csv files from our runs.