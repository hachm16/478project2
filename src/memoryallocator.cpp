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
    failedAllocationCount = 0;
}

void MemoryAllocator::configure(int memoryUnitSizeValue, int memoryUnitCountValue, const string &algorithmNameValue)
{
    memoryUnitSize = memoryUnitSizeValue;
    memoryUnitCount = memoryUnitCountValue;
    algorithmName = algorithmNameValue;

    unitJobId.clear();
    unitJobId.resize(memoryUnitCount, -1);

    allocations.clear();

    nextFitSearchStart = 0;

    allocationCount = 0;
    allocationOperationCount = 0;
    freeRequestCount = 0;
    freeOperationCount = 0;
    failedAllocationCount = 0;
}

void MemoryAllocator::setCurrentJobId(int jobIdValue)
{
    currentJobId = jobIdValue;
}

int MemoryAllocator::mallocFF(int sizeBytes)
{
    if (memoryUnitSize <= 0)
    {
        return -1;
    }

    int requiredUnits = (sizeBytes + memoryUnitSize - 1) / memoryUnitSize;
    int location = allocateFirstFit(requiredUnits);

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
    else
    {
        failedAllocationCount++;
    }

    return location;
}

int MemoryAllocator::mallocNF(int sizeBytes)
{
    if (memoryUnitSize <= 0)
    {
        return -1;
    }

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
    else
    {
        failedAllocationCount++;
    }

    return location;
}

int MemoryAllocator::mallocBF(int sizeBytes)
{
    if (memoryUnitSize <= 0)
    {
        return -1;
    }

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
    else
    {
        failedAllocationCount++;
    }

    return location;
}

int MemoryAllocator::mallocWF(int sizeBytes)
{
    if (memoryUnitSize <= 0)
    {
        return -1;
    }

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
    else
    {
        failedAllocationCount++;
    }

    return location;
}

void MemoryAllocator::freeFF(int startIndex)
{
    if (startIndex < 0)
    {
        return;
    }

    freeRequestCount++;

    int count = (int)allocations.size();
    for (int i = 0; i < count; i++)
    {
        freeOperationCount++;

        AllocationRecord &record = allocations[i];
        if (record.isActive && record.startIndex == startIndex)
        {
            markUnitsFree(record.startIndex, record.unitCount);
            record.isActive = false;
            break;
        }
    }
}

int MemoryAllocator::allocateFirstFit(int requiredUnits)
{
    int start = -1;
    int length = 0;

    for (int i = 0; i < memoryUnitCount; i++)
    {
        allocationOperationCount++;

        if (unitJobId[i] == -1)
        {
            if (length == 0)
            {
                start = i;
            }
            length++;
            if (length >= requiredUnits)
            {
                markUnitsAllocated(start, requiredUnits);
                return start;
            }
        }
        else
        {
            start = -1;
            length = 0;
        }
    }

    return -1;
}

int MemoryAllocator::allocateNextFit(int requiredUnits)
{
    int examined = 0;
    int index = nextFitSearchStart;

    int start = -1;
    int length = 0;

    while (examined < memoryUnitCount)
    {
        int realIndex = index % memoryUnitCount;
        allocationOperationCount++;

        if (unitJobId[realIndex] == -1)
        {
            if (length == 0)
            {
                start = realIndex;
            }
            length++;
            if (length >= requiredUnits)
            {
                markUnitsAllocated(start, requiredUnits);
                nextFitSearchStart = (start + requiredUnits) % memoryUnitCount;
                return start;
            }
        }
        else
        {
            start = -1;
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

        if (unitJobId[i] == -1)
        {
            if (length == 0)
            {
                start = i;
            }
            length++;
        }
        else
        {
            if (length >= requiredUnits && length < bestLength)
            {
                bestStart = start;
                bestLength = length;
            }
            start = -1;
            length = 0;
        }
    }

    if (length >= requiredUnits && length < bestLength)
    {
        bestStart = start;
        bestLength = length;
    }

    if (bestStart >= 0)
    {
        markUnitsAllocated(bestStart, requiredUnits);
    }

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
            if (length == 0)
            {
                start = i;
            }
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

    if (length >= requiredUnits && length > worstLength)
    {
        worstStart = start;
        worstLength = length;
    }

    if (worstStart >= 0)
    {
        markUnitsAllocated(worstStart, requiredUnits);
    }

    return worstStart;
}

void MemoryAllocator::markUnitsAllocated(int startIndex, int unitCount)
{
    int end = startIndex + unitCount;
    if (end > memoryUnitCount)
    {
        end = memoryUnitCount;
    }

    for (int i = startIndex; i < end; i++)
    {
        unitJobId[i] = currentJobId;
    }
}

void MemoryAllocator::markUnitsFree(int startIndex, int unitCount)
{
    int end = startIndex + unitCount;
    if (end > memoryUnitCount)
    {
        end = memoryUnitCount;
    }

    for (int i = startIndex; i < end; i++)
    {
        unitJobId[i] = -1;
    }
}

int MemoryAllocator::getTotalMemorySize()
{
    return memoryUnitCount * memoryUnitSize;
}

int MemoryAllocator::getAllocatedMemorySize()
{
    int usedUnits = 0;
    for (int i = 0; i < memoryUnitCount; i++)
    {
        if (unitJobId[i] != -1)
        {
            usedUnits++;
        }
    }
    return usedUnits * memoryUnitSize;
}

int MemoryAllocator::getFreeMemorySize()
{
    int freeUnits = 0;
    for (int i = 0; i < memoryUnitCount; i++)
    {
        if (unitJobId[i] == -1)
        {
            freeUnits++;
        }
    }
    return freeUnits * memoryUnitSize;
}

int MemoryAllocator::getRequiredMemorySize()
{
    int total = 0;
    int count = (int)allocations.size();
    for (int i = 0; i < count; i++)
    {
        const AllocationRecord &record = allocations[i];
        if (record.isActive)
        {
            total += record.requestedBytes;
        }
    }
    return total;
}

int MemoryAllocator::getExternalFragmentationCount()
{
    int count = 0;
    bool inFreeBlock = false;

    for (int i = 0; i < memoryUnitCount; i++)
    {
        if (unitJobId[i] == -1)
        {
            if (!inFreeBlock)
            {
                inFreeBlock = true;
                count++;
            }
        }
        else
        {
            inFreeBlock = false;
        }
    }

    return count;
}

int MemoryAllocator::getLargestFreeBlockSize()
{
    int maxLength = 0;
    int length = 0;

    for (int i = 0; i < memoryUnitCount; i++)
    {
        if (unitJobId[i] == -1)
        {
            length++;
        }
        else
        {
            if (length > maxLength)
            {
                maxLength = length;
            }
            length = 0;
        }
    }

    if (length > maxLength)
    {
        maxLength = length;
    }

    return maxLength * memoryUnitSize;
}

int MemoryAllocator::getSmallestFreeBlockSize()
{
    int minLength = 0;
    int length = 0;

    for (int i = 0; i < memoryUnitCount; i++)
    {
        if (unitJobId[i] == -1)
        {
            length++;
        }
        else
        {
            if (length > 0)
            {
                if (minLength == 0 || length < minLength)
                {
                    minLength = length;
                }
            }
            length = 0;
        }
    }

    if (length > 0)
    {
        if (minLength == 0 || length < minLength)
        {
            minLength = length;
        }
    }

    return minLength * memoryUnitSize;
}

int MemoryAllocator::getNumberOfAllocations()
{
    return allocationCount;
}

int MemoryAllocator::getNumberOfAllocationOperations()
{
    return allocationOperationCount;
}

int MemoryAllocator::getNumberOfFreeRequests()
{
    return freeRequestCount;
}

int MemoryAllocator::getNumberOfFreeOperations()
{
    return freeOperationCount;
}

int MemoryAllocator::getFailedAllocationCount()
{
    return failedAllocationCount;
}
