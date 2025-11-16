#ifndef MEMORYSIMULATION_H
#define MEMORYSIMULATION_H

#include <string>
#include <vector>
#include "randomgenerator.h"
#include "memoryallocator.h"
#include "csvwriter.h"

using namespace std;

struct SimulationConfig //inputs for running
{
    string testName;

    int percentSmall;
    int percentMedium;
    int percentLarge;

    int memoryUnitSize;
    int memoryUnitCount;

    unsigned seed;

    string summaryFilePath;
    string logFileFirstFit;  //paths
    string logFileNextFit;
    string logFileBestFit;
    string logFileWorstFit;

    SimulationConfig();
};


class MemorySimulation
{
public:
    MemorySimulation();

    void configure(const SimulationConfig &cfg);
    void run();

    struct Job
    {
        int jobId;
        char jobType; //S,M,L
        int arrivalTime;
        int runTime;
        int codeSize;
        int stackSize;
        int heapElementCount;

        Job()
        {
            jobId = -1;
            jobType = 'S';
            arrivalTime = 0;
            runTime = 0;
            codeSize = 0;
            stackSize = 0;
            heapElementCount = 0;
        }
    };

    struct HeapElement //singular heap alloc for a job
    {
        int elementId;
        int jobId;
        int arrivalTime;
        int lifetime;
        int sizeBytes;

        HeapElement()
        {
            elementId = -1;
            jobId = -1;
            arrivalTime = 0;
            lifetime = 0;
            sizeBytes = 0;
        }

        int getDepartureTime() const; //when element free
    };

    struct Event
    {
        int time;
        int jobId;
        int elementId;
        int eventType; // 0=job arrive  1=job depart  2=heap alloc   3=heap free

        Event()
        {
            time = 0;
            jobId = -1;
            elementId = -1;
            eventType = 0;
        }
    };

    SimulationConfig config;
    RandomGenerator rng;


    int heapAllocCountFF; //successfully alloc'd heap elements per type
    int heapAllocCountNF;
    int heapAllocCountBF;
    int heapAllocCountWF;

    int heapAllocBytesFF;
    int heapAllocBytesNF;
    int heapAllocBytesBF;
    int heapAllocBytesWF;


    MemoryAllocator allocatorFirstFit;  //mem alloc per alg
    MemoryAllocator allocatorNextFit;
    MemoryAllocator allocatorBestFit;
    MemoryAllocator allocatorWorstFit;

    vector<Job> jobs;
    vector<HeapElement> heapElements;
    vector<Event> events;

    // remember locations so we can free
    vector<int> jobCodeLocationFF;
    vector<int> jobStackLocationFF;
    vector<int> jobCodeLocationNF;
    vector<int> jobStackLocationNF;
    vector<int> jobCodeLocationBF;
    vector<int> jobStackLocationBF;
    vector<int> jobCodeLocationWF;
    vector<int> jobStackLocationWF;

    vector<int> heapLocationFF;
    vector<int> heapLocationNF;
    vector<int> heapLocationBF;
    vector<int> heapLocationWF;

//-/-/
    int sampleCount;
    int sumPercentInUseFF;
    int sumPercentInUseNF;
    int sumPercentInUseBF;
    int sumPercentInUseWF;
    int sumPercentInternalFF;
    int sumPercentInternalNF;
    int sumPercentInternalBF;
    int sumPercentInternalWF;
    int maxPercentInUseFF;
    int maxPercentInUseNF;
    int maxPercentInUseBF;
    int maxPercentInUseWF;
    int maxExternalFragFF;
    int maxExternalFragNF;
    int maxExternalFragBF;
    int maxExternalFragWF;
//-////-/

    void initializeAllocators();

    int chooseJobType(); // return 0/1/2 for S/M/L

    Job createRandomJob(int jobIdValue, int arrivalTime); //

    void createHeapElementsForJob(const Job &job);

    void buildJobsAndHeapElements(); //gen all jobs and heap elements
    void buildEvents();  // conv into events

    void processEventsAtTime(int currentTime, CsvWriter &logFF, CsvWriter &logNF, CsvWriter &logBF, CsvWriter &logWF);

    void logAllocation(CsvWriter &writer, int time, int jobId, int sizeBytes, int location); //alloc log row
    void logFree(CsvWriter &writer, int time, int jobId, int location); //free log row
};

#endif // MEMORYSIMULATION_H
