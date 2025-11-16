# 478project2

Memory Allocation Simulation

This program simulates dynamic memory allocation using four algorithms:

First Fit (FF)

Next Fit (NF)

Best Fit (BF)

Worst Fit (WF)

Jobs, stack space, and heap elements are generated randomly based on the input seed and percentage distribution.

How to Run

From the build directory:

478project2.exe --seed <int> --memoryUnitSize <bytes> --memoryUnitCount <units> \
 --percentSmall <int> --percentMedium <int> --percentLarge <int> \
 --timeBetweenArrivalSampleCount <int> --testName <name>

Example
478project2.exe --seed 0 --memoryUnitSize 8 --memoryUnitCount 5000 \
 --percentSmall 33 --percentMedium 33 --percentLarge 34 \
 --timeBetweenArrivalSampleCount 10 --testName MU8

Output Files (in /out/)

summary.csv
Contains periodic memory statistics for FF, NF, BF, and WF:

allocatedBytes

freeBytes

requiredBytes

externalFragments

percentMemoryInUse

percentMemoryFree

percentInternalFragmentation

log_firstfit.csv / log_nextfit.csv / log_bestfit.csv / log_worstfit.csv
Detailed logs of each allocation and free event per algorithm.
