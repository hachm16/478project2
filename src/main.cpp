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

using namespace std;


//
int main(int argc, char* argv[])
{
    cout << "\nPart 1 generator starting." << endl;

    GeneratorConfig config; //seed and timeBetweenArrivalSampleCount set default

    int i = 1;
    while (i < argc) //loop for parsing
    {
        string a = argv[i];
        if (a == "--seed") //rng unsigned seed
        {
            if (i + 1 < argc) { config.seed = (unsigned)stoi(string(argv[i+1])); i += 2; }
        }

        else if (a == "--timeBetweenArrivalSampleCount") //how many arrival times to gen
        {
            if (i + 1 < argc) { config.timeBetweenArrivalSampleCount = stoi(string(argv[i+1])); i += 2; } // make into int value and set
        }
    }



    Generator generator;
    generator.configure(config);

    generator.generatetimeBetweenArrivalCSV(); //write to out/timeBetweenArrival.csv


    return 0;
}
//example run ("--" long for setting user seed)
//.\478project2.exe --seed 12345 --timeBetweenArrivalSampleCount 200
