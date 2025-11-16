#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include "randomgenerator.h"


using namespace std;

struct GeneratorConfig {
    unsigned seed; //no negatives

    int timeBetweenArrivalSampleCount;  // how many arrival samples to generate
    int processSampleCount;    // rows to gen
    int heapSampleProcessCount;    //heap sample processes to gen


    GeneratorConfig();
};


class Generator {
public:
    Generator();

    void configure(const GeneratorConfig &cfg);

    void generatetimeBetweenArrivalCSV();

    GeneratorConfig config;
    RandomGenerator rng;
    string outPath(const string &baseName);
};

#endif // GENERATOR_H
