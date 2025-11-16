#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H

#include <vector>
#include <string>

using namespace std;

struct AllocationRecord
{
    int startIndex;
    int unitCount;
    int requestedBytes;
    int jobId;
    bool isActive;

    AllocationRecord()
    {
        startIndex = -1; //index and id -1 out of the way
        unitCount = 0;
        requestedBytes = 0;
        jobId = -1;
        isActive = false;
    }
};


class MemoryAllocator
{
public:
    MemoryAllocator();

    void configure(int memoryUnitSizeValue, int memoryUnitCountValue, const string &algorithmNameValue);

    // algorithms for each memory
    int mallocFF(int sizeBytes);
    int mallocNF(int sizeBytes);
    int mallocBF(int sizeBytes);
    int mallocWF(int sizeBytes);

    // free memory method to start somewhere
    void freeFF(int startIndex);

    // tell allocator which job is making request
    void setCurrentJobId(int jobIdValue);

    // metrics
    int getTotalMemorySize();
    int getAllocatedMemorySize();
    int getFreeMemorySize();
    int getRequiredMemorySize();
    int getExternalFragmentationCount();
    int getLargestFreeBlockSize();
    int getSmallestFreeBlockSize();

    int getNumberOfAllocations();
    int getNumberOfAllocationOperations();
    int getNumberOfFreeRequests();
    int getNumberOfFreeOperations();

    string algorithmName;

    int memoryUnitSize;
    int memoryUnitCount;

    // -1 = free, else id
    vector<int> unitJobId;
    vector<AllocationRecord> allocations;

    int nextFitSearchStart;

    int currentJobId;

    int allocationCount;
    int allocationOperationCount;
    int freeRequestCount;
    int freeOperationCount;

    int allocateFirstFit(int requiredUnits); //method per type
    int allocateNextFit(int requiredUnits);
    int allocateBestFit(int requiredUnits);
    int allocateWorstFit(int requiredUnits);

    void markUnitsAllocated(int startIndex, int unitCount);
    void markUnitsFree(int startIndex, int unitCount);
};

#endif // MEMORYALLOCATOR_H
