****Project Title****

Reputation-Based Decision Accuracy in V2V Communication with Limited Infrastructure

****Overview****

This project takes advantage of Simulation of Urban Mobility (SUMO), Objective Modular Network Testbed in C++ (OMNeT++), Vehicles
in Network Simulation (Veins) and the TRACI (Traffic Control Interface) package to simulate and analyze vehicle movement, reputation distribution, and certificate handling. The system takes data from a SUMO simulation, uses a reputation distribution model, and runs an OMNeT++ simulation to process the reputation and certificates of the vehicles. Finally, Python scripts are used to analyze the simulation results.

****Requirements****

SUMO (Simulation of Urban MObility)
OMNeT++
VEINS (For linking SUMO and OMNeT++)
TRACI (Traffic Control Interface package)
Python 3.x and necessary libraries
C++ for OMNeT++ simulation scripts

****A-Setup Instructions****

Install Dependencies

You need to have the following tools installed to run the project:

1-_SUMO_ (For traffic simulation)
Install SUMO: https://sumo.dlr.de/docs/Installing/index.html


2-_OMNeT++_ (For network simulation)
Install OMNeT++: https://omnetpp.org/download/

3-_VEINS_ (For linking SUMO and OMNeT++)
VEINS connects SUMO and OMNeT++ for vehicular network simulations.

Install VEINS:https://veins.car2x.org/download/

4-_TRACI_ (For traffic interaction between SUMO and OMNeT++)
TRACI should be installed along with SUMO.

5-_Python 3.x_
Install Python: Python Official Site
Install required Python libraries.



****B. Run SUMO Simulation****

You’ll first need to run a traffic simulation using SUMO. The SUMO simulation runs vehicle movements and creates outputs (e.g., vehicle positions, speeds).

Start the SUMO simulation:

sumo -c /path/to/sumo_simulation/sumo_config_file.sumocfg
Ensure that the TRACI package is integrated with your SUMO simulation to interact with vehicles and get necessary data.

****C. Distribute Reputation Values****

Next, use the tringle reputation distribution model to allocate reputation values to vehicles based on the simulation data.

Run the Python script:

python /path/to/python_analysis/Generate RepValues triangleDistrub.py
This script will distribute reputation values among vehicles resulted from SUMO simulation.

****D. Run OMNeT++ Simulation****

The reputation values and certificates are then used in differnt scenarios in OMNeT++ network simulation. The OMNeT++ simulation uses C++ scripts to handle the reputation and certificate logic for each vehicle.
Navigate to the OMNeT++ directory
This will simulate the network environment and process the reputation and certificates.

****E. Analyze Final Results with Python****

Finally, after running the OMNeT++ simulation, use a Python script to analyze the results.
This step will process the output data from the OMNeT++ simulation and generate the final results, such as reputation distribution across the network and certificate statuses.
