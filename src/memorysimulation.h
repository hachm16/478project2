#ifndef MEMORYSIMULATION_H
#define MEMORYSIMULATION_H

#include <string>
#include <vector>
#include "randomgenerator.h"
#include "memoryallocator.h"
#include "csvwriter.h"

using namespace std;

struct SimulationConfig
{
    string testName;

    int percentSmall;
    int percentMedium;
    int percentLarge;

    int memoryUnitSize;
    int memoryUnitCount;

    unsigned seed;

    string summaryFilePath;
    string logFileFirstFit;
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

private:
    struct Job
    {
        int jobId;
        char jobType; // 'S', 'M', 'L'
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

    struct HeapElement
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

        int getDepartureTime() const
        {
            return arrivalTime + lifetime;
        }
    };

    struct Event
    {
        int time;
        int jobId;
        int elementId;
        int eventType; // 0 = job arrive, 1 = job depart, 2 = heap alloc, 3 = heap free

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

    MemoryAllocator allocatorFirstFit;
    MemoryAllocator allocatorNextFit;
    MemoryAllocator allocatorBestFit;
    MemoryAllocator allocatorWorstFit;

    vector<Job> jobs;
    vector<HeapElement> heapElements;
    vector<Event> events;

    // remember locations so we can free correctly
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

    void initializeAllocators();

    int chooseJobType();
    Job createRandomJob(int jobIdValue, int arrivalTime);
    void createHeapElementsForJob(const Job &job);

    void buildJobsAndHeapElements();
    void buildEvents();

    void processEventsAtTime(int currentTime,
                             CsvWriter &logFF,
                             CsvWriter &logNF,
                             CsvWriter &logBF,
                             CsvWriter &logWF);

    void logAllocation(CsvWriter &writer, int time, int jobId, int sizeBytes, int location);
    void logFree(CsvWriter &writer, int time, int jobId, int location);
};

#endif // MEMORYSIMULATION_H
