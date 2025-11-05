#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H


using namespace std;
#include <cstdlib>



struct randEngine
{
    void seed(unsigned s) {
        srand(s);
    }

    unsigned int operator()() { //give raw random number
        int r = rand();
        unsigned int u = (unsigned int) r; // conv to unsigned int by casting
        return u;
    }
};



class RandomGenerator {
public:
    RandomGenerator();
    RandomGenerator(unsigned seed);

    void reseed(unsigned seed);
    int randomInclusive(int low, int high);
    int roundUpTo(int value, int unit);

    randEngine rngGenerator;
};

#endif // RANDOMGENERATOR_H
