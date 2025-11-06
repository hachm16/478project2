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

    GeneratorConfig config; //seed and timeBetweenArrivalSampleCount set default

//argv[1] is long --seed string on CL, argv[2] is the actual seed val, 3 is --samplecount, 4 being actual sample count num

    //argc = 5 bc program name + above args

    int i = 1; //skip title element
    while (i < argc) //loop for parsing for args, stop b4 out of bound
    {
        string a = argv[i];
        if (a == "--seed") //set rng seed case
        {
            if (i+1 < argc) // <5
            {
                config.seed = (unsigned)stoi(string(argv[i+1])); //cast seed token and store int
                i += 2; //go next, over --seed and its value
            }
        }

        else if (a == "--timeBetweenArrivalSampleCount")
        { //how many indicies to generate for rand samples
            if (i + 1 < argc) // ensure a next token before reading
            {
                config.timeBetweenArrivalSampleCount = stoi(string(argv[i+1]));
                // store int from string

                i += 2; // same as above
            }
        }
    }



    Generator generator;
    generator.configure(config);

    generator.generatetimeBetweenArrivalCSV(); //write to out/timeBetweenArrival.csv


    return 0;
}
