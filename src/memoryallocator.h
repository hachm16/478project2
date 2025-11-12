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
        startIndex = -1;
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

    // memory assignment algorithms
    int mallocFF(int sizeBytes);
    int mallocNF(int sizeBytes);
    int mallocBF(int sizeBytes);
    int mallocWF(int sizeBytes);

    // free memory starting at a location
    void freeFF(int startIndex);

    // tell allocator which job is currently making the request
    void setCurrentJobId(int jobIdValue);

    // basic metrics
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
    int getFailedAllocationCount();

    string algorithmName;

private:
    int memoryUnitSize;
    int memoryUnitCount;

    // -1 = free, otherwise job id
    vector<int> unitJobId;
    vector<AllocationRecord> allocations;

    int nextFitSearchStart;

    int currentJobId;

    int allocationCount;
    int allocationOperationCount;
    int freeRequestCount;
    int freeOperationCount;
    int failedAllocationCount;

    int allocateFirstFit(int requiredUnits);
    int allocateNextFit(int requiredUnits);
    int allocateBestFit(int requiredUnits);
    int allocateWorstFit(int requiredUnits);

    void markUnitsAllocated(int startIndex, int unitCount);
    void markUnitsFree(int startIndex, int unitCount);
};

#endif // MEMORYALLOCATOR_H
