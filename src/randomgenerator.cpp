#include "randomgenerator.h"


RandomGenerator::RandomGenerator() {
    rngGenerator.seed(125);//fixed seed so runs are repeatable
}

RandomGenerator::RandomGenerator(unsigned seed) {
        // arg constructor with user seed
    rngGenerator.seed(seed);
}

void RandomGenerator::reseed(unsigned seed) {
    //change the seed after construction
    rngGenerator.seed(seed); //reset state to start new rand numbers
}

int RandomGenerator::randomInclusive(int low, int high) {
    if (low > high)
    {
        int tmp = low;
        low = high;
        high = tmp;
    } // swap if reversed

    int range = (high - low) + 1; // count of ints in low-high

    unsigned int rawrng = rngGenerator();
    int offset = rawrng % range;
    int mapped = low + offset; // shift into low-high range
    return mapped;

}


