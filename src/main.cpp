#include "csvwriter.h"
#include "generator.h"
#include "randomgenerator.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include "memorysimulation.h"

using namespace std;



void printUsage()
{

    cout<<"Options:" << endl;
    cout<<"--seed <unsigned>                        Seed for random number generator" << endl;
    cout<<"--timeBetweenArrivalSampleCount <int>    Number of arrival samples" << endl;
    cout<<"--percentSmall <int>                     Small jobs percent" << endl;
    cout<<"--percentMedium <int>                    Medium jobs percent" << endl;
    cout<<"--percentLarge <int>                     Large jobs percent" << endl;
    cout<<"--memoryUnitSize <int>                   Memory unit size (must be multiple of 8)" << endl;
    cout<<"--memoryUnitCount <int>                  Number of memory units" << endl;
    cout<<"--testName <string>                      Simulation test name" << endl;
    cout<<"--summaryFilePath <string>               Summary CSV output path" << endl;
    cout<<"--logFileFirstFit <string>               First Fit log CSV output path" << endl;
    cout<<"--logFileNextFit <string>                Next Fit log CSV output path" << endl;
    cout<<"--logFileBestFit <string>                Best Fit log CSV output path" << endl;
    cout<<"--logFileWorstFit <string>               Worst Fit log CSV output path" << endl;


    cout<<"Note: percentSmall + percentMedium + percentLarge must equal 100." << endl;
}





int main(int argc, char* argv[])
{

    GeneratorConfig generatorConfig;

    SimulationConfig simulationConfig;

    simulationConfig.seed= generatorConfig.seed;
    // keep simulation seed in sync with generator




    int i= 1; //skip title element
    while (i < argc) //loop for parsing for args, stop b4 out of bound
    {
        string a = argv[i];


        if (a == "--seed") //set rng seed case
        {
            if (i+1 < argc)
            {
                unsigned seedValue= (unsigned)stoi(string(argv[i + 1]));
                generatorConfig.seed= seedValue;
                simulationConfig.seed= seedValue;
                i += 2;
                //cast seed token and store int
                //go next, over --seed and its value
            }
            else break;
        }

        else if (a == "--timeBetweenArrivalSampleCount")
        { //how many indicies to generate for rand samples
            if (i + 1 < argc) // ensure a next token before reading
            {
                generatorConfig.timeBetweenArrivalSampleCount = stoi(string(argv[i+1]));
                // store int from string

                i += 2; // same as above
            }
            else break;
        }


        else if (a =="--percentSmall")
        {
            if (i + 1 < argc)
            {
                simulationConfig.percentSmall = stoi(string(argv[i + 1]));
                i += 2;
            }
            else break;
        }


        else if (a == "--percentMedium")
        {
            if (i + 1 < argc)
            {
                simulationConfig.percentMedium = stoi(string(argv[i + 1]));
                i += 2;
            }
            else break;
        }


        else if (a== "--percentLarge")
        {
            if (i + 1 < argc)
            {
                simulationConfig.percentLarge = stoi(string(argv[i + 1]));
                i += 2;
            }
            else break;
        }


        else if (a =="--memoryUnitSize")
        {
            if (i + 1 < argc)
            {
                simulationConfig.memoryUnitSize = stoi(string(argv[i + 1]));
                i += 2;
            }
            else break;
        }


        else if (a =="--memoryUnitCount")
        {
            if (i + 1 < argc)
            {
                simulationConfig.memoryUnitCount = stoi(string(argv[i + 1]));
                i += 2;
            }
            else break;
        }


        else if (a =="--testName")
        {
            if (i + 1 < argc)
            {
                simulationConfig.testName = string(argv[i + 1]);
                i += 2;
            }
            else break;
        }


        else if (a =="--summaryFilePath")
        {
            if (i + 1 < argc)
            {
                simulationConfig.summaryFilePath = string(argv[i + 1]);
                i += 2;
            }
            else break;
        }


        else if (a =="--logFileFirstFit")
        {
            if (i + 1 < argc)
            {
                simulationConfig.logFileFirstFit = string(argv[i + 1]);
                i += 2;
            }
            else break;
        }


        else if (a =="--logFileNextFit")
        {
            if (i + 1 < argc)
            {
                simulationConfig.logFileNextFit = string(argv[i + 1]);
                i += 2;
            }
            else break;

        }


        else if (a =="--logFileBestFit")
        {
            if (i + 1 < argc)
            {
                simulationConfig.logFileBestFit = string(argv[i + 1]);
                i += 2;
            }
            else break;
        }


        else if (a =="--logFileWorstFit")
        {
            if (i + 1 < argc)
            {
                simulationConfig.logFileWorstFit = string(argv[i + 1]);
                i += 2;
            }
            else break;
        }


        //unknown arg, skip
        else i++;
    }

    // finished parsing args

    // validate percentages for simulation
    int totalPercent = simulationConfig.percentSmall + simulationConfig.percentMedium+ simulationConfig.percentLarge;

    if (totalPercent != 100)
    {
        cout << "Error: percentSmall + percentMedium + percentLarge must add up to 100." << endl;
        return 1;
    }

    // validate memory configuration
    if (simulationConfig.memoryUnitSize <= 0 || simulationConfig.memoryUnitSize % 8 != 0 || simulationConfig.memoryUnitCount <= 0)
    {
        cout << "Invalid configuration. Exiting" << endl;
        return 1;
    }

    Generator generator;
    generator.configure(generatorConfig);
    generator.generatetimeBetweenArrivalCSV();

    cout<<"Running simulation for test: " << simulationConfig.testName << endl;

    MemorySimulation simulation;
    simulation.configure(simulationConfig);
    simulation.run();

    return 0;
}


