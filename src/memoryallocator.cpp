#include "memoryallocator.h"

MemoryAllocator::MemoryAllocator()
{
    memoryUnitSize = 0;
    memoryUnitCount = 0;

    nextFitSearchStart = 0;

    currentJobId = -1;

    allocationCount = 0;
    allocationOperationCount = 0;
    freeRequestCount = 0;
    freeOperationCount = 0;

}

void MemoryAllocator::configure(int memoryUnitSizeValue, int memoryUnitCountValue, const string &algorithmNameValue)
{
    memoryUnitSize = memoryUnitSizeValue; //bytes per memory unit
    memoryUnitCount = memoryUnitCountValue; //available count
    algorithmName = algorithmNameValue;

    unitJobId.clear();
    unitJobId.resize(memoryUnitCount, -1);          //reset

    allocations.clear();

    nextFitSearchStart = 0;

    allocationCount = 0;
    allocationOperationCount = 0;
    freeRequestCount = 0;
    freeOperationCount = 0;
}

void MemoryAllocator::setCurrentJobId(int jobIdValue)
{
    currentJobId = jobIdValue;
}

int MemoryAllocator::mallocFF(int sizeBytes)
{
    if (memoryUnitSize <= 0) return -1; //if fail case


    //how many memory units to hold sizeBytes and round up
    int requiredUnits = (sizeBytes + memoryUnitSize - 1) / memoryUnitSize;
    int location = allocateFirstFit(requiredUnits); //find space of free

    if (location >= 0) //if space found, continue
    {
        AllocationRecord record;
        record.startIndex = location;
        record.unitCount = requiredUnits; //how many reserved
        record.requestedBytes = sizeBytes;
        record.jobId = currentJobId;
        record.isActive = true;

        allocations.push_back(record);
        allocationCount++;        //save and iterate
    }


    return location;
}


int MemoryAllocator::mallocNF(int sizeBytes)
{
    if (memoryUnitSize <= 0) return -1;  //same process as above


    int requiredUnits = (sizeBytes + memoryUnitSize - 1) / memoryUnitSize;
    int location = allocateNextFit(requiredUnits);

    if (location >= 0)
    {
        AllocationRecord record;
        record.startIndex = location;
        record.unitCount = requiredUnits;
        record.requestedBytes = sizeBytes;
        record.jobId = currentJobId;
        record.isActive = true;

        allocations.push_back(record);
        allocationCount++;
    }


    return location;
}

int MemoryAllocator::mallocBF(int sizeBytes)
{
    if (memoryUnitSize <= 0) return -1;


    int requiredUnits = (sizeBytes + memoryUnitSize - 1) / memoryUnitSize;
    int location = allocateBestFit(requiredUnits);

    if (location >= 0)
    {
        AllocationRecord record;
        record.startIndex = location;
        record.unitCount = requiredUnits;
        record.requestedBytes = sizeBytes;
        record.jobId = currentJobId;
        record.isActive = true;

        allocations.push_back(record);
        allocationCount++;
    }

    return location;
}

int MemoryAllocator::mallocWF(int sizeBytes)
{
    if (memoryUnitSize <= 0) return -1;

    int requiredUnits = (sizeBytes + memoryUnitSize - 1) / memoryUnitSize;
    int location = allocateWorstFit(requiredUnits);

    if (location >= 0)
    {
        AllocationRecord record;
        record.startIndex = location;
        record.unitCount = requiredUnits;
        record.requestedBytes = sizeBytes;
        record.jobId = currentJobId;
        record.isActive = true;
        allocations.push_back(record);
        allocationCount++;
    }



    return location;
}

void MemoryAllocator::freeFF(int startIndex)
{
    if (startIndex < 0) return; //bad start, end


    freeRequestCount++;

    int count = (int)allocations.size();


    for (int i = 0; i < count; i++)
    {
        freeOperationCount++;    //alloc record

        AllocationRecord &record = allocations[i];

        // look for active allocation at this index
        if (record.isActive && record.startIndex == startIndex)
        {
            // mark units free in unitJobId array
            markUnitsFree(record.startIndex, record.unitCount);
            record.isActive = false;
            break;
        }
    }
}

int MemoryAllocator::allocateFirstFit(int requiredUnits)
{
    int start = -1; //free num
    int length = 0;

    for (int i = 0; i<memoryUnitCount; i++) //from 0 to find first space
    {
        allocationOperationCount++;

        if (unitJobId[i] == -1) //if free
        {
            if (length == 0) start = i; //keep track of index of free

            length++;

            if (length >= requiredUnits)
            { //mark allocated
                markUnitsAllocated(start, requiredUnits);
                return start;
            }
        }

        else
        {
            start = -1; //hit alloc'd unit, reset
            length = 0;
        }
    }

    return -1; //spaces too small, failed
}

int MemoryAllocator::allocateNextFit(int requiredUnits)
{
    int examined = 0; //units checked
    int index = nextFitSearchStart; //start from last position

    int start = -1;
    int length = 0;

    while (examined < memoryUnitCount) //keep scanning til every unit examined
    {
        int realIndex = index % memoryUnitCount; //wrap around
        allocationOperationCount++;

        if (unitJobId[realIndex] == -1) //free unit
        {
            if (length == 0) start = realIndex;//if first free unit, remember start index

            length++;


            if (length >= requiredUnits) // save start for next search after this block
            {
                markUnitsAllocated(start, requiredUnits);
                nextFitSearchStart = (start + requiredUnits) % memoryUnitCount;
                return start; //starting index of allocated space
            }
        }

        else
        {
            start = -1; //reset case
            length = 0;
        }


        examined++;
        index++;
    }

    return -1;
}

int MemoryAllocator::allocateBestFit(int requiredUnits)
{
    int bestStart = -1;
    int bestLength = memoryUnitCount + 1;

    int start = -1;
    int length = 0;

    for (int i = 0; i < memoryUnitCount; i++)
    {
        allocationOperationCount++;

        if (unitJobId[i] == -1) //free spot
        {
            if (length == 0) start = i;

            length++;
        }

        else
        {
            // alloc'd unit, check if free block better than prev best

            if (length >= requiredUnits && length < bestLength)
            {
                bestStart = start;
                bestLength = length;
            }
            start = -1;
            length = 0;
        }
    }

    //case where last units form free space
    if (length >= requiredUnits && length < bestLength) bestStart = start;

    if (bestStart >= 0) markUnitsAllocated(bestStart, requiredUnits); //good BF spot



    return bestStart;
}


int MemoryAllocator::allocateWorstFit(int requiredUnits)
{
    int worstStart = -1;
    int worstLength = -1;

    int start = -1;
    int length = 0;

    for (int i = 0; i < memoryUnitCount; i++)
    {
        allocationOperationCount++;

        if (unitJobId[i] == -1)
        {
            if (length == 0) start = i;
            length++;
        }

        else
        {
            if (length >= requiredUnits && length > worstLength)
            {
                worstStart = start;
                worstLength = length;
            }
            start = -1;
            length = 0;
        }
    }

    if (length >= requiredUnits && length > worstLength) worstStart = start;

    if (worstStart >= 0) markUnitsAllocated(worstStart, requiredUnits);

    return worstStart;
}


void MemoryAllocator::markUnitsAllocated(int startIndex, int unitCount)
{
    int end = startIndex + unitCount;

    if (end > memoryUnitCount) end = memoryUnitCount; //end cannot exceed, bound

    for (int i = startIndex; i < end; i++) unitJobId[i] = currentJobId; // mark each as free again

}

void MemoryAllocator::markUnitsFree(int startIndex, int unitCount)
{
    int end = startIndex + unitCount;

    if (end > memoryUnitCount) end = memoryUnitCount;

    for (int i = startIndex; i < end; i++) unitJobId[i] = -1; //set free

}


int MemoryAllocator::getTotalMemorySize()
{
    return memoryUnitCount * memoryUnitSize; //num units * bytes per unit
}


int MemoryAllocator::getAllocatedMemorySize()
{
    int usedUnits = 0;

    for (int i = 0; i < memoryUnitCount; i++) //units in use count
    {
        if (unitJobId[i] != -1) usedUnits++;
    }

    return usedUnits * memoryUnitSize; //conv to bytes
}

int MemoryAllocator::getFreeMemorySize()
{
    int freeUnits = 0;

    for (int i = 0; i < memoryUnitCount; i++) if (unitJobId[i] == -1) freeUnits++;

    return freeUnits * memoryUnitSize;
}

int MemoryAllocator::getRequiredMemorySize()
{
    int total = 0;
    int count = (int)allocations.size();

    for (int i = 0; i < count; i++) //add requested sizes of all active alloc's
    {
        const AllocationRecord &record = allocations[i];

        if (record.isActive) total += record.requestedBytes; //only count active
    }

    return total;
}


int MemoryAllocator::getExternalFragmentationCount()
{ //get how many free blocks exist separately
    int count = 0;
    bool inFreeBlock = false;

    for (int i = 0; i < memoryUnitCount; i++)
    {
        if (unitJobId[i] == -1)
        {
            if (!inFreeBlock)
            {
                inFreeBlock = true;
                count++;             //incrmt block count
            }
        }

        else inFreeBlock = false;
    }

    return count;
}

int MemoryAllocator::getLargestFreeBlockSize()
{
    int maxLength = 0; // for max store
    int length = 0; //for current

    for (int i = 0; i < memoryUnitCount; i++)
    {
        if (unitJobId[i] == -1) length++;

        else
        {
            if (length > maxLength) maxLength = length; //check if largest
            length = 0;
        }

    }

    if (length > maxLength) maxLength = length; // case if free block at end of memory


    return maxLength * memoryUnitSize;
}

int MemoryAllocator::getSmallestFreeBlockSize()
{
    int minLength = 0;
    int length = 0;

    for (int i = 0; i < memoryUnitCount; i++)
    {
        if (unitJobId[i] == -1) length++;


        else
        {
            if (length > 0 && (minLength == 0 || length < minLength)) minLength = length;
            length = 0;
            // end of free block, if not empty see if smallest
        }

    }


    if (length > 0 && (minLength == 0 || length < minLength)) minLength = length; //end of memory case



    return minLength * memoryUnitSize;
}


// getters for metrics

int MemoryAllocator::getNumberOfAllocations() {return allocationCount;}
int MemoryAllocator::getNumberOfAllocationOperations() { return allocationOperationCount;}
int MemoryAllocator::getNumberOfFreeRequests() {return freeRequestCount;}
int MemoryAllocator::getNumberOfFreeOperations() {return freeOperationCount; }


