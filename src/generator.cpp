#include "generator.h"
#include "csvwriter.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <vector>


GeneratorConfig::GeneratorConfig()
{
    seed = 125; //default value just for setup
    timeBetweenArrivalSampleCount = 200; // arrival times to generate for arrivals CSV
}


Generator::Generator(){} //no vars, constructor call



void Generator::configure(const GeneratorConfig &cfg)
{
    config = cfg; //rng setup
    //copy config values to instance

    rng.reseed(config.seed); //initialize rng with seed
}


string Generator::outPath(const string &baseName) { // build path for csv
    error_code ec;  // to receive error codes
    filesystem::create_directories("out", ec); // make sure out exists so errors go into ec
    return string("out/") + baseName + ".csv";  // "out/<baseName>.csv"
}



void Generator::generatetimeBetweenArrivalCSV() //generate csv of arrival times
{
    //doc ex: arrivals at base times 1,4,7... then add random 0...4
    CsvWriter writer(outPath("timeBetweenArrival")); //open csv writer
    writer.header({"index","arrivalTime"});

    for (int i = 0; i < config.timeBetweenArrivalSampleCount; i++) //requested number of ind
    {
        int baseTime = 1 + 3*i;              // base times like 1,4,7...
        int randomOffset = rng.randomInclusive(0, 4);  //3 +- 2
        int arrival = baseTime + randomOffset; //abs arrival time
        writer.row({to_string(i), to_string(arrival)});
    }
    writer.close();
}
