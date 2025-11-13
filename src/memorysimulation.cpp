#include "memorysimulation.h"
#include <iostream>

SimulationConfig::SimulationConfig()
{
    testName = "defaultTest";

    percentSmall = 33;
    percentMedium = 33;
    percentLarge = 34;

    memoryUnitSize = 8;
    memoryUnitCount = 5000;

    seed = 125;

    summaryFilePath = "out/summary.csv";
    logFileFirstFit = "out/log_firstfit.csv";
    logFileNextFit = "out/log_nextfit.csv";
    logFileBestFit = "out/log_bestfit.csv";
    logFileWorstFit = "out/log_worstfit.csv";
}

MemorySimulation::MemorySimulation()
{
}

void MemorySimulation::configure(const SimulationConfig &cfg)
{
    config = cfg;
    rng.reseed(config.seed);
    initializeAllocators();
}

void MemorySimulation::initializeAllocators()
{
    allocatorFirstFit.configure(config.memoryUnitSize, config.memoryUnitCount, "First Fit");
    allocatorNextFit.configure(config.memoryUnitSize, config.memoryUnitCount, "Next Fit");
    allocatorBestFit.configure(config.memoryUnitSize, config.memoryUnitCount, "Best Fit");
    allocatorWorstFit.configure(config.memoryUnitSize, config.memoryUnitCount, "Worst Fit");
}

int MemorySimulation::chooseJobType()
{
    int r = rng.randomInclusive(1, 100);

    if (r <= config.percentSmall)
    {
        return 0; // small
    }
    else if (r <= config.percentSmall + config.percentMedium)
    {
        return 1; // medium
    }
    else
    {
        return 2; // large
    }
}

MemorySimulation::Job MemorySimulation::createRandomJob(int jobIdValue, int arrivalTime)
{
    Job job;
    job.jobId = jobIdValue;
    job.arrivalTime = arrivalTime;

    int typeCode = chooseJobType();

    if (typeCode == 0)
    {
        job.jobType = 'S';
        job.runTime = rng.randomInclusive(4, 6);          // 5 +/- 1
        job.codeSize = rng.randomInclusive(40, 80);       // 60 +/- 20
        job.stackSize = rng.randomInclusive(20, 40);      // 30 +/- 10
        job.heapElementCount = job.runTime * 5;
    }
    else if (typeCode == 1)
    {
        job.jobType = 'M';
        job.runTime = rng.randomInclusive(9, 11);         // 10 +/- 1
        job.codeSize = rng.randomInclusive(60, 120);      // 90 +/- 30
        job.stackSize = rng.randomInclusive(40, 80);      // 60 +/- 20
        job.heapElementCount = job.runTime * 10;
    }
    else
    {
        job.jobType = 'L';
        job.runTime = rng.randomInclusive(24, 26);        // 25 +/- 1
        job.codeSize = rng.randomInclusive(120, 220);     // 170 +/- 50
        job.stackSize = rng.randomInclusive(70, 110);     // 90 +/- 30
        job.heapElementCount = job.runTime * 25;
    }

    return job;
}

void MemorySimulation::createHeapElementsForJob(const Job &job)
{
    if (job.heapElementCount <= 0 || job.runTime <= 0) return;
    if ((int)heapElements.size() > 20000) return;
    // hard cap total heap element (its taking insanely long)


    int perTime = job.heapElementCount / job.runTime;
    if (perTime <= 0) perTime = 1;


    for (int t = 0; t < job.runTime; t++)
    {
        int arrivalTime = job.arrivalTime + t;

        for (int i = 0; i < perTime; i++)
        {
            HeapElement element;
            element.jobId = job.jobId;
            element.arrivalTime = arrivalTime;
            element.sizeBytes = rng.randomInclusive(20, 50); // 35 +/- 15
            element.lifetime = rng.randomInclusive(1, job.runTime);

            element.elementId = (int)heapElements.size();
            heapElements.push_back(element);

            // make sure heap location vectors keep up
            heapLocationFF.push_back(-1);
            heapLocationNF.push_back(-1);
            heapLocationBF.push_back(-1);
            heapLocationWF.push_back(-1);
        }
    }
}

void MemorySimulation::buildJobsAndHeapElements()
{
    jobs.clear();
    heapElements.clear();

    jobCodeLocationFF.clear();
    jobStackLocationFF.clear();
    jobCodeLocationNF.clear();
    jobStackLocationNF.clear();
    jobCodeLocationBF.clear();
    jobStackLocationBF.clear();
    jobCodeLocationWF.clear();
    jobStackLocationWF.clear();

    heapLocationFF.clear();
    heapLocationNF.clear();
    heapLocationBF.clear();
    heapLocationWF.clear();

    int jobIdValue = 0;
    int endTime = 2000;

    int baseTime = 1;
    int offset = rng.randomInclusive(0, 4);
    int nextArrival = baseTime + offset;

    while (nextArrival < endTime)
    {
        Job job = createRandomJob(jobIdValue, nextArrival);

        jobs.push_back(job);

        // keep job location vectors in sync
        jobCodeLocationFF.push_back(-1);
        jobStackLocationFF.push_back(-1);
        jobCodeLocationNF.push_back(-1);
        jobStackLocationNF.push_back(-1);
        jobCodeLocationBF.push_back(-1);
        jobStackLocationBF.push_back(-1);
        jobCodeLocationWF.push_back(-1);
        jobStackLocationWF.push_back(-1);

        createHeapElementsForJob(job);

        jobIdValue++;

        baseTime = baseTime + 3;
        offset = rng.randomInclusive(0, 4);
        nextArrival = baseTime + offset;
    }
}

void MemorySimulation::buildEvents()
{
    int lastEventTime = 0;
    if (!events.empty())
    {
        lastEventTime = events[(int)events.size() - 1].time;
    }
    int endTime = lastEventTime;
    int prefillTime = 2000;

    if (endTime < prefillTime)
    {
        endTime = prefillTime;
    }


    events.clear();

    int jobCount = (int)jobs.size();
    for (int i = 0; i < jobCount; i++)
    {
        const Job &job = jobs[i];

        Event arriveEvent;
        arriveEvent.time = job.arrivalTime;
        arriveEvent.jobId = job.jobId;
        arriveEvent.eventType = 0;
        events.push_back(arriveEvent);

        Event departEvent;
        departEvent.time = job.arrivalTime + job.runTime;
        departEvent.jobId = job.jobId;
        departEvent.eventType = 1;
        events.push_back(departEvent);
    }

    int heapCount = (int)heapElements.size();
    for (int i = 0; i < heapCount; i++)
    {
        const HeapElement &element = heapElements[i];

        Event allocEvent;
        allocEvent.time = element.arrivalTime;
        allocEvent.jobId = element.jobId;
        allocEvent.elementId = element.elementId;
        allocEvent.eventType = 2;
        events.push_back(allocEvent);

        Event freeEvent;
        freeEvent.time = element.getDepartureTime();
        freeEvent.jobId = element.jobId;
        freeEvent.elementId = element.elementId;
        freeEvent.eventType = 3;
        events.push_back(freeEvent);
    }

    // simple bubble-sort style sort: by time, then by eventType
    int n = (int)events.size();
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            bool shouldSwap = false;

            if (events[j].time < events[i].time)
            {
                shouldSwap = true;
            }
            else if (events[j].time == events[i].time && events[j].eventType < events[i].eventType)
            {
                shouldSwap = true;
            }

            if (shouldSwap)
            {
                Event temp = events[i];
                events[i] = events[j];
                events[j] = temp;
            }
        }
    }
}


void MemorySimulation::logAllocation(CsvWriter &writer, int time, int jobId, int sizeBytes, int location)
{
    vector<string> rowValues;
    rowValues.push_back(to_string(time));
    rowValues.push_back(to_string(jobId));
    rowValues.push_back("alloc");
    rowValues.push_back(to_string(sizeBytes));
    rowValues.push_back(to_string(location));
    writer.row(rowValues);
}

void MemorySimulation::logFree(CsvWriter &writer, int time, int jobId, int location)
{
    vector<string> rowValues;
    rowValues.push_back(to_string(time));
    rowValues.push_back(to_string(jobId));
    rowValues.push_back("free");
    rowValues.push_back("0");
    rowValues.push_back(to_string(location));
    writer.row(rowValues);
}

void MemorySimulation::processEventsAtTime(int currentTime,
                                           CsvWriter &logFF,
                                           CsvWriter &logNF,
                                           CsvWriter &logBF,
                                           CsvWriter &logWF)
{
    int eventCount = (int)events.size();

    for (int i = 0; i < eventCount; i++)
    {
        Event &ev = events[i];

        if (ev.time != currentTime)
        {
            continue;
        }

        if (ev.eventType == 0)
        {
            // job arrival: allocate code and stack for each algorithm
            const Job &job = jobs[ev.jobId];

            allocatorFirstFit.setCurrentJobId(job.jobId);
            int locCodeFF = allocatorFirstFit.mallocFF(job.codeSize);
            int locStackFF = allocatorFirstFit.mallocFF(job.stackSize);
            jobCodeLocationFF[job.jobId] = locCodeFF;
            jobStackLocationFF[job.jobId] = locStackFF;
            logAllocation(logFF, currentTime, job.jobId, job.codeSize, locCodeFF);
            logAllocation(logFF, currentTime, job.jobId, job.stackSize, locStackFF);

            allocatorNextFit.setCurrentJobId(job.jobId);
            int locCodeNF = allocatorNextFit.mallocNF(job.codeSize);
            int locStackNF = allocatorNextFit.mallocNF(job.stackSize);
            jobCodeLocationNF[job.jobId] = locCodeNF;
            jobStackLocationNF[job.jobId] = locStackNF;
            logAllocation(logNF, currentTime, job.jobId, job.codeSize, locCodeNF);
            logAllocation(logNF, currentTime, job.jobId, job.stackSize, locStackNF);

            allocatorBestFit.setCurrentJobId(job.jobId);
            int locCodeBF = allocatorBestFit.mallocBF(job.codeSize);
            int locStackBF = allocatorBestFit.mallocBF(job.stackSize);
            jobCodeLocationBF[job.jobId] = locCodeBF;
            jobStackLocationBF[job.jobId] = locStackBF;
            logAllocation(logBF, currentTime, job.jobId, job.codeSize, locCodeBF);
            logAllocation(logBF, currentTime, job.jobId, job.stackSize, locStackBF);

            allocatorWorstFit.setCurrentJobId(job.jobId);
            int locCodeWF = allocatorWorstFit.mallocWF(job.codeSize);
            int locStackWF = allocatorWorstFit.mallocWF(job.stackSize);
            jobCodeLocationWF[job.jobId] = locCodeWF;
            jobStackLocationWF[job.jobId] = locStackWF;
            logAllocation(logWF, currentTime, job.jobId, job.codeSize, locCodeWF);
            logAllocation(logWF, currentTime, job.jobId, job.stackSize, locStackWF);
        }
        else if (ev.eventType == 1)
        {
            // job departure: free code and stack
            const Job &job = jobs[ev.jobId];

            int locCodeFF = jobCodeLocationFF[job.jobId];
            int locStackFF = jobStackLocationFF[job.jobId];
            if (locCodeFF >= 0) { allocatorFirstFit.freeFF(locCodeFF); logFree(logFF, currentTime, job.jobId, locCodeFF); }
            if (locStackFF >= 0) { allocatorFirstFit.freeFF(locStackFF); logFree(logFF, currentTime, job.jobId, locStackFF); }

            int locCodeNF = jobCodeLocationNF[job.jobId];
            int locStackNF = jobStackLocationNF[job.jobId];
            if (locCodeNF >= 0) { allocatorNextFit.freeFF(locCodeNF); logFree(logNF, currentTime, job.jobId, locCodeNF); }
            if (locStackNF >= 0) { allocatorNextFit.freeFF(locStackNF); logFree(logNF, currentTime, job.jobId, locStackNF); }

            int locCodeBF = jobCodeLocationBF[job.jobId];
            int locStackBF = jobStackLocationBF[job.jobId];
            if (locCodeBF >= 0) { allocatorBestFit.freeFF(locCodeBF); logFree(logBF, currentTime, job.jobId, locCodeBF); }
            if (locStackBF >= 0) { allocatorBestFit.freeFF(locStackBF); logFree(logBF, currentTime, job.jobId, locStackBF); }

            int locCodeWF = jobCodeLocationWF[job.jobId];
            int locStackWF = jobStackLocationWF[job.jobId];
            if (locCodeWF >= 0) { allocatorWorstFit.freeFF(locCodeWF); logFree(logWF, currentTime, job.jobId, locCodeWF); }
            if (locStackWF >= 0) { allocatorWorstFit.freeFF(locStackWF); logFree(logWF, currentTime, job.jobId, locStackWF); }
        }
        else if (ev.eventType == 2)
        {
            // heap allocation
            const HeapElement &element = heapElements[ev.elementId];

            allocatorFirstFit.setCurrentJobId(element.jobId);
            int locFF = allocatorFirstFit.mallocFF(element.sizeBytes);
            heapLocationFF[element.elementId] = locFF;
            logAllocation(logFF, currentTime, element.jobId, element.sizeBytes, locFF);

            allocatorNextFit.setCurrentJobId(element.jobId);
            int locNF = allocatorNextFit.mallocNF(element.sizeBytes);
            heapLocationNF[element.elementId] = locNF;
            logAllocation(logNF, currentTime, element.jobId, element.sizeBytes, locNF);

            allocatorBestFit.setCurrentJobId(element.jobId);
            int locBF = allocatorBestFit.mallocBF(element.sizeBytes);
            heapLocationBF[element.elementId] = locBF;
            logAllocation(logBF, currentTime, element.jobId, element.sizeBytes, locBF);

            allocatorWorstFit.setCurrentJobId(element.jobId);
            int locWF = allocatorWorstFit.mallocWF(element.sizeBytes);
            heapLocationWF[element.elementId] = locWF;
            logAllocation(logWF, currentTime, element.jobId, element.sizeBytes, locWF);
        }
        else if (ev.eventType == 3)
        {
            // heap free
            int locFF = heapLocationFF[ev.elementId];
            int locNF = heapLocationNF[ev.elementId];
            int locBF = heapLocationBF[ev.elementId];
            int locWF = heapLocationWF[ev.elementId];

            if (locFF >= 0) { allocatorFirstFit.freeFF(locFF); logFree(logFF, currentTime, ev.jobId, locFF); }
            if (locNF >= 0) { allocatorNextFit.freeFF(locNF); logFree(logNF, currentTime, ev.jobId, locNF); }
            if (locBF >= 0) { allocatorBestFit.freeFF(locBF); logFree(logBF, currentTime, ev.jobId, locBF); }
            if (locWF >= 0) { allocatorWorstFit.freeFF(locWF); logFree(logWF, currentTime, ev.jobId, locWF); }

            heapLocationFF[ev.elementId] = -1;
            heapLocationNF[ev.elementId] = -1;
            heapLocationBF[ev.elementId] = -1;
            heapLocationWF[ev.elementId] = -1;
        }
    }
}

void MemorySimulation::run()
{
    buildJobsAndHeapElements();
    buildEvents();


    int lastEventTime = 0;
    if (!events.empty())
    {
        lastEventTime = events[(int)events.size() - 1].time;
    }

    int prefillTime = 2000;     // keep prefill
    int endTime = lastEventTime;
    if (endTime < prefillTime)  // ensure we reach prefill for sampling
    {
        endTime = prefillTime;
    }


    CsvWriter summaryWriter(config.summaryFilePath);
    vector<string> summaryHeader;
    summaryHeader.push_back("time");
    summaryHeader.push_back("algorithm");
    summaryHeader.push_back("allocatedBytes");
    summaryHeader.push_back("freeBytes");
    summaryHeader.push_back("requiredBytes");
    summaryHeader.push_back("externalFragments");
    summaryHeader.push_back("percentMemoryInUse");
    summaryHeader.push_back("percentMemoryFree");
    summaryHeader.push_back("percentInternalFragmentation");

    summaryWriter.header(summaryHeader);


    CsvWriter logFF(config.logFileFirstFit);
    CsvWriter logNF(config.logFileNextFit);
    CsvWriter logBF(config.logFileBestFit);
    CsvWriter logWF(config.logFileWorstFit);

    vector<string> logHeader;
    logHeader.push_back("time");
    logHeader.push_back("jobId");
    logHeader.push_back("event");
    logHeader.push_back("sizeBytes");
    logHeader.push_back("location");
    logFF.header(logHeader);
    logNF.header(logHeader);
    logBF.header(logHeader);
    logWF.header(logHeader);


    int sampleInterval=200;

    for (int time = 0; time <= endTime; time++)
    {
        processEventsAtTime(time, logFF, logNF, logBF, logWF);

        if (time >= prefillTime && (time - prefillTime) % 20 == 0)
        {
            // ----- First Fit -----
            int totalBytesFF = allocatorFirstFit.getTotalMemorySize();
            int allocatedBytesFF = allocatorFirstFit.getAllocatedMemorySize();
            int freeBytesFF = allocatorFirstFit.getFreeMemorySize();
            int requiredBytesFF = allocatorFirstFit.getRequiredMemorySize();

            int percentInUseFF = 0;
            int percentFreeFF = 0;
            int percentInternalFF = 0;

            if (totalBytesFF > 0)
            {
                percentInUseFF = (100 * allocatedBytesFF) / totalBytesFF;
                percentFreeFF = (100 * freeBytesFF) / totalBytesFF;
            }

            if (allocatedBytesFF > 0)
            {
                int internalWasteFF = allocatedBytesFF - requiredBytesFF;
                if (internalWasteFF < 0)
                {
                    internalWasteFF = 0;
                }
                percentInternalFF = (100 * internalWasteFF) / allocatedBytesFF;
            }

            vector<string> rowFF;
            rowFF.push_back(to_string(time));
            rowFF.push_back("FF");
            rowFF.push_back(to_string(allocatedBytesFF));
            rowFF.push_back(to_string(freeBytesFF));
            rowFF.push_back(to_string(requiredBytesFF));
            rowFF.push_back(to_string(allocatorFirstFit.getExternalFragmentationCount()));
            rowFF.push_back(to_string(percentInUseFF));
            rowFF.push_back(to_string(percentFreeFF));
            rowFF.push_back(to_string(percentInternalFF));
            summaryWriter.row(rowFF);

            // ----- Next Fit -----
            int totalBytesNF = allocatorNextFit.getTotalMemorySize();
            int allocatedBytesNF = allocatorNextFit.getAllocatedMemorySize();
            int freeBytesNF = allocatorNextFit.getFreeMemorySize();
            int requiredBytesNF = allocatorNextFit.getRequiredMemorySize();

            int percentInUseNF = 0;
            int percentFreeNF = 0;
            int percentInternalNF = 0;

            if (totalBytesNF > 0)
            {
                percentInUseNF = (100 * allocatedBytesNF) / totalBytesNF;
                percentFreeNF = (100 * freeBytesNF) / totalBytesNF;
            }

            if (allocatedBytesNF > 0)
            {
                int internalWasteNF = allocatedBytesNF - requiredBytesNF;
                if (internalWasteNF < 0)
                {
                    internalWasteNF = 0;
                }
                percentInternalNF = (100 * internalWasteNF) / allocatedBytesNF;
            }

            vector<string> rowNF;
            rowNF.push_back(to_string(time));
            rowNF.push_back("NF");
            rowNF.push_back(to_string(allocatedBytesNF));
            rowNF.push_back(to_string(freeBytesNF));
            rowNF.push_back(to_string(requiredBytesNF));
            rowNF.push_back(to_string(allocatorNextFit.getExternalFragmentationCount()));
            rowNF.push_back(to_string(percentInUseNF));
            rowNF.push_back(to_string(percentFreeNF));
            rowNF.push_back(to_string(percentInternalNF));
            summaryWriter.row(rowNF);

            // ----- Best Fit -----
            int totalBytesBF = allocatorBestFit.getTotalMemorySize();
            int allocatedBytesBF = allocatorBestFit.getAllocatedMemorySize();
            int freeBytesBF = allocatorBestFit.getFreeMemorySize();
            int requiredBytesBF = allocatorBestFit.getRequiredMemorySize();

            int percentInUseBF = 0;
            int percentFreeBF = 0;
            int percentInternalBF = 0;

            if (totalBytesBF > 0)
            {
                percentInUseBF = (100 * allocatedBytesBF) / totalBytesBF;
                percentFreeBF = (100 * freeBytesBF) / totalBytesBF;
            }

            if (allocatedBytesBF > 0)
            {
                int internalWasteBF = allocatedBytesBF - requiredBytesBF;
                if (internalWasteBF < 0)
                {
                    internalWasteBF = 0;
                }
                percentInternalBF = (100 * internalWasteBF) / allocatedBytesBF;
            }

            vector<string> rowBF;
            rowBF.push_back(to_string(time));
            rowBF.push_back("BF");
            rowBF.push_back(to_string(allocatedBytesBF));
            rowBF.push_back(to_string(freeBytesBF));
            rowBF.push_back(to_string(requiredBytesBF));
            rowBF.push_back(to_string(allocatorBestFit.getExternalFragmentationCount()));
            rowBF.push_back(to_string(percentInUseBF));
            rowBF.push_back(to_string(percentFreeBF));
            rowBF.push_back(to_string(percentInternalBF));
            summaryWriter.row(rowBF);

            // ----- Worst Fit -----
            int totalBytesWF = allocatorWorstFit.getTotalMemorySize();
            int allocatedBytesWF = allocatorWorstFit.getAllocatedMemorySize();
            int freeBytesWF = allocatorWorstFit.getFreeMemorySize();
            int requiredBytesWF = allocatorWorstFit.getRequiredMemorySize();

            int percentInUseWF = 0;
            int percentFreeWF = 0;
            int percentInternalWF = 0;

            if (totalBytesWF > 0)
            {
                percentInUseWF = (100 * allocatedBytesWF) / totalBytesWF;
                percentFreeWF = (100 * freeBytesWF) / totalBytesWF;
            }

            if (allocatedBytesWF > 0)
            {
                int internalWasteWF = allocatedBytesWF - requiredBytesWF;
                if (internalWasteWF < 0)
                {
                    internalWasteWF = 0;
                }
                percentInternalWF = (100 * internalWasteWF) / allocatedBytesWF;
            }

            vector<string> rowWF;
            rowWF.push_back(to_string(time));
            rowWF.push_back("WF");
            rowWF.push_back(to_string(allocatedBytesWF));
            rowWF.push_back(to_string(freeBytesWF));
            rowWF.push_back(to_string(requiredBytesWF));
            rowWF.push_back(to_string(allocatorWorstFit.getExternalFragmentationCount()));
            rowWF.push_back(to_string(percentInUseWF));
            rowWF.push_back(to_string(percentFreeWF));
            rowWF.push_back(to_string(percentInternalWF));
            summaryWriter.row(rowWF);
        }

    }

    // final summary for each algorithm
    // ----- First Fit -----
    int totalBytesFF = allocatorFirstFit.getTotalMemorySize();
    int allocatedBytesFF = allocatorFirstFit.getAllocatedMemorySize();
    int freeBytesFF = allocatorFirstFit.getFreeMemorySize();
    int requiredBytesFF = allocatorFirstFit.getRequiredMemorySize();

    int numAllocFF = allocatorFirstFit.getNumberOfAllocations();
    int numAllocOpsFF = allocatorFirstFit.getNumberOfAllocationOperations();
    int numFreeFF = allocatorFirstFit.getNumberOfFreeRequests();
    int numFreeOpsFF = allocatorFirstFit.getNumberOfFreeOperations();

    int percentInUseFF = 0;
    int percentFreeFF = 0;
    int percentInternalFF = 0;

    if (totalBytesFF > 0)
    {
        percentInUseFF = (100 * allocatedBytesFF) / totalBytesFF;
        percentFreeFF = (100 * freeBytesFF) / totalBytesFF;
    }

    if (allocatedBytesFF > 0)
    {
        int internalWasteFF = allocatedBytesFF - requiredBytesFF;
        if (internalWasteFF < 0)
        {
            internalWasteFF = 0;
        }
        percentInternalFF = (100 * internalWasteFF) / allocatedBytesFF;
    }

    int avgAllocOpsFF = 0;
    int avgFreeOpsFF = 0;

    if (numAllocFF > 0)
    {
        avgAllocOpsFF = numAllocOpsFF / numAllocFF;
    }
    if (numFreeFF > 0)
    {
        avgFreeOpsFF = numFreeOpsFF / numFreeFF;
    }

    cout << "Final statistics for First Fit:" << endl;
    cout << "  Total memory bytes: " << totalBytesFF << endl;
    cout << "  Allocated bytes: " << allocatedBytesFF << endl;
    cout << "  Free bytes: " << freeBytesFF << endl;
    cout << "  Required bytes: " << requiredBytesFF << endl;
    cout << "  Percent memory in use: " << percentInUseFF << endl;
    cout << "  Percent memory free: " << percentFreeFF << endl;
    cout << "  Percent internal fragmentation: " << percentInternalFF << endl;
    cout << "  Number of allocations: " << numAllocFF << endl;
    cout << "  Number of allocation operations: " << numAllocOpsFF << endl;
    cout << "  Average allocation operations: " << avgAllocOpsFF << endl;
    cout << "  Number of free requests: " << numFreeFF << endl;
    cout << "  Number of free operations: " << numFreeOpsFF << endl;
    cout << "  Average free operations: " << avgFreeOpsFF << endl;
    cout << endl;

    // ----- Next Fit -----
    int totalBytesNF = allocatorNextFit.getTotalMemorySize();
    int allocatedBytesNF = allocatorNextFit.getAllocatedMemorySize();
    int freeBytesNF = allocatorNextFit.getFreeMemorySize();
    int requiredBytesNF = allocatorNextFit.getRequiredMemorySize();

    int numAllocNF = allocatorNextFit.getNumberOfAllocations();
    int numAllocOpsNF = allocatorNextFit.getNumberOfAllocationOperations();
    int numFreeNF = allocatorNextFit.getNumberOfFreeRequests();
    int numFreeOpsNF = allocatorNextFit.getNumberOfFreeOperations();

    int percentInUseNF = 0;
    int percentFreeNF = 0;
    int percentInternalNF = 0;

    if (totalBytesNF > 0)
    {
        percentInUseNF = (100 * allocatedBytesNF) / totalBytesNF;
        percentFreeNF = (100 * freeBytesNF) / totalBytesNF;
    }

    if (allocatedBytesNF > 0)
    {
        int internalWasteNF = allocatedBytesNF - requiredBytesNF;
        if (internalWasteNF < 0)
        {
            internalWasteNF = 0;
        }
        percentInternalNF = (100 * internalWasteNF) / allocatedBytesNF;
    }

    int avgAllocOpsNF = 0;
    int avgFreeOpsNF = 0;

    if (numAllocNF > 0)
    {
        avgAllocOpsNF = numAllocOpsNF / numAllocNF;
    }
    if (numFreeNF > 0)
    {
        avgFreeOpsNF = numFreeOpsNF / numFreeNF;
    }

    cout << "Final statistics for Next Fit:" << endl;
    cout << "  Total memory bytes: " << totalBytesNF << endl;
    cout << "  Allocated bytes: " << allocatedBytesNF << endl;
    cout << "  Free bytes: " << freeBytesNF << endl;
    cout << "  Required bytes: " << requiredBytesNF << endl;
    cout << "  Percent memory in use: " << percentInUseNF << endl;
    cout << "  Percent memory free: " << percentFreeNF << endl;
    cout << "  Percent internal fragmentation: " << percentInternalNF << endl;
    cout << "  Number of allocations: " << numAllocNF << endl;
    cout << "  Number of allocation operations: " << numAllocOpsNF << endl;
    cout << "  Average allocation operations: " << avgAllocOpsNF << endl;
    cout << "  Number of free requests: " << numFreeNF << endl;
    cout << "  Number of free operations: " << numFreeOpsNF << endl;
    cout << "  Average free operations: " << avgFreeOpsNF << endl;
    cout << endl;

    // ----- Best Fit -----
    int totalBytesBF = allocatorBestFit.getTotalMemorySize();
    int allocatedBytesBF = allocatorBestFit.getAllocatedMemorySize();
    int freeBytesBF = allocatorBestFit.getFreeMemorySize();
    int requiredBytesBF = allocatorBestFit.getRequiredMemorySize();

    int numAllocBF = allocatorBestFit.getNumberOfAllocations();
    int numAllocOpsBF = allocatorBestFit.getNumberOfAllocationOperations();
    int numFreeBF = allocatorBestFit.getNumberOfFreeRequests();
    int numFreeOpsBF = allocatorBestFit.getNumberOfFreeOperations();

    int percentInUseBF = 0;
    int percentFreeBF = 0;
    int percentInternalBF = 0;

    if (totalBytesBF > 0)
    {
        percentInUseBF = (100 * allocatedBytesBF) / totalBytesBF;
        percentFreeBF = (100 * freeBytesBF) / totalBytesBF;
    }

    if (allocatedBytesBF > 0)
    {
        int internalWasteBF = allocatedBytesBF - requiredBytesBF;
        if (internalWasteBF < 0)
        {
            internalWasteBF = 0;
        }
        percentInternalBF = (100 * internalWasteBF) / allocatedBytesBF;
    }

    int avgAllocOpsBF = 0;
    int avgFreeOpsBF = 0;

    if (numAllocBF > 0)
    {
        avgAllocOpsBF = numAllocOpsBF / numAllocBF;
    }
    if (numFreeBF > 0)
    {
        avgFreeOpsBF = numFreeOpsBF / numFreeBF;
    }

    cout << "Final statistics for Best Fit:" << endl;
    cout << "  Total memory bytes: " << totalBytesBF << endl;
    cout << "  Allocated bytes: " << allocatedBytesBF << endl;
    cout << "  Free bytes: " << freeBytesBF << endl;
    cout << "  Required bytes: " << requiredBytesBF << endl;
    cout << "  Percent memory in use: " << percentInUseBF << endl;
    cout << "  Percent memory free: " << percentFreeBF << endl;
    cout << "  Percent internal fragmentation: " << percentInternalBF << endl;
    cout << "  Number of allocations: " << numAllocBF << endl;
    cout << "  Number of allocation operations: " << numAllocOpsBF << endl;
    cout << "  Average allocation operations: " << avgAllocOpsBF << endl;
    cout << "  Number of free requests: " << numFreeBF << endl;
    cout << "  Number of free operations: " << numFreeOpsBF << endl;
    cout << "  Average free operations: " << avgFreeOpsBF << endl;
    cout << endl;

    // ----- Worst Fit -----
    int totalBytesWF = allocatorWorstFit.getTotalMemorySize();
    int allocatedBytesWF = allocatorWorstFit.getAllocatedMemorySize();
    int freeBytesWF = allocatorWorstFit.getFreeMemorySize();
    int requiredBytesWF = allocatorWorstFit.getRequiredMemorySize();

    int numAllocWF = allocatorWorstFit.getNumberOfAllocations();
    int numAllocOpsWF = allocatorWorstFit.getNumberOfAllocationOperations();
    int numFreeWF = allocatorWorstFit.getNumberOfFreeRequests();
    int numFreeOpsWF = allocatorWorstFit.getNumberOfFreeOperations();

    int percentInUseWF = 0;
    int percentFreeWF = 0;
    int percentInternalWF = 0;

    if (totalBytesWF > 0)
    {
        percentInUseWF = (100 * allocatedBytesWF) / totalBytesWF;
        percentFreeWF = (100 * freeBytesWF) / totalBytesWF;
    }

    if (allocatedBytesWF > 0)
    {
        int internalWasteWF = allocatedBytesWF - requiredBytesWF;
        if (internalWasteWF < 0)
        {
            internalWasteWF = 0;
        }
        percentInternalWF = (100 * internalWasteWF) / allocatedBytesWF;
    }

    int avgAllocOpsWF = 0;
    int avgFreeOpsWF = 0;

    if (numAllocWF > 0)
    {
        avgAllocOpsWF = numAllocOpsWF / numAllocWF;
    }
    if (numFreeWF > 0)
    {
        avgFreeOpsWF = numFreeOpsWF / numFreeWF;
    }

    cout << "Final statistics for Worst Fit:" << endl;
    cout << "  Total memory bytes: " << totalBytesWF << endl;
    cout << "  Allocated bytes: " << allocatedBytesWF << endl;
    cout << "  Free bytes: " << freeBytesWF << endl;
    cout << "  Required bytes: " << requiredBytesWF << endl;
    cout << "  Percent memory in use: " << percentInUseWF << endl;
    cout << "  Percent memory free: " << percentFreeWF << endl;
    cout << "  Percent internal fragmentation: " << percentInternalWF << endl;
    cout << "  Number of allocations: " << numAllocWF << endl;
    cout << "  Number of allocation operations: " << numAllocOpsWF << endl;
    cout << "  Average allocation operations: " << avgAllocOpsWF << endl;
    cout << "  Number of free requests: " << numFreeWF << endl;
    cout << "  Number of free operations: " << numFreeOpsWF << endl;
    cout << "  Average free operations: " << avgFreeOpsWF << endl;
    cout << endl;

    summaryWriter.close();
    logFF.close();
    logNF.close();
    logBF.close();
    logWF.close();

    cout << "Simulation complete for test: " << config.testName << endl;
}


