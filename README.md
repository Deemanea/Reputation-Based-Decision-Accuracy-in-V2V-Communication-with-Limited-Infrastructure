Project Title
Reputation-Based Decision Accuracy in V2V Communication with Limited Infrastructure
Overview: 
This project takes advantage of Simulation of Urban Mobility (SUMO), Objective Modular Network Testbed in C++ (OMNeT++), Vehicles
in Network Simulation (Veins) and the TRACI (Traffic Control Interface) package to simulate and analyze vehicle movement, reputation distribution, and certificate handling. The system takes data from a SUMO simulation, uses a reputation distribution model, and runs an OMNeT++ simulation to process the reputation and certificates of the vehicles. Finally, Python scripts are used to analyze the simulation results.

Requirements
SUMO (Simulation of Urban MObility)
OMNeT++
VEINS (For linking SUMO and OMNeT++)
TRACI (Traffic Control Interface package)
Python 3.x and necessary libraries
C++ for OMNeT++ simulation scripts

A-Setup Instructions

1. Install Dependencies
You need to have the following tools installed to run the project:

1-SUMO (For traffic simulation)
Install SUMO: https://sumo.dlr.de/docs/Installing/index.html


2-OMNeT++ (For network simulation)
Install OMNeT++: https://omnetpp.org/download/

3-VEINS (For linking SUMO and OMNeT++)
VEINS connects SUMO and OMNeT++ for vehicular network simulations.

Install VEINS:https://veins.car2x.org/download/

4-TRACI (For traffic interaction between SUMO and OMNeT++)
TRACI should be installed along with SUMO.

5-Python 3.x
Install Python: Python Official Site
Install required Python libraries.



B. Run SUMO Simulation
You’ll first need to run a traffic simulation using SUMO. The SUMO simulation runs vehicle movements and creates outputs (e.g., vehicle positions, speeds).

Start the SUMO simulation:

sumo -c /path/to/sumo_simulation/sumo_config_file.sumocfg
Ensure that the TRACI package is integrated with your SUMO simulation to interact with vehicles and get necessary data.

C. Distribute Reputation Values
Next, use the tringle reputation distribution model to allocate reputation values to vehicles based on the simulation data.

Run the Python script:

python /path/to/python_analysis/Generate RepValues triangleDistrub.py
This script will distribute reputation values among vehicles resulted from SUMO simulation.

D. Run OMNeT++ Simulation
The reputation values and certificates are then used in differnt scenarios in OMNeT++ network simulation. The OMNeT++ simulation uses C++ scripts to handle the reputation and certificate logic for each vehicle.

Navigate to the OMNeT++ directory
This will simulate the network environment and process the reputation and certificates.

E. Analyze Final Results with Python
Finally, after running the OMNeT++ simulation, use a Python script to analyze the results.
This step will process the output data from the OMNeT++ simulation and generate the final results, such as reputation distribution across the network and certificate statuses.

Detailed Steps
Step 1: SUMO Simulation
SUMO simulates the movement of vehicles on a given network.
Outputs can include vehicle position, speed, and other traffic-related data, which are captured and processed by TRACI.
Step 2: Reputation Distribution
Reputation values are assigned to vehicles based on certain criteria (e.g., trustworthiness, reliability, behavior).
These values are distributed using a tringle distribuation approach.
Step 3: OMNeT++ Simulation
OMNeT++ simulates the network protocols, including how vehicles communicate with each other.
Each vehicle's reputation and certificates are processed, influencing the vehicle's actions in the network.
Step 4: Python Final Analysis
After all simulations are complete, Python scripts are used to analyze and visualize the final results.

