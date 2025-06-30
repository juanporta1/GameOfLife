#include <WorkerPool.h>

WorkerPool::WorkerPool(int t) : t(t), chunksNeighbors(t) {
    for (size_t i = 0; i < t; ++i) {
        threads.emplace_back([this, i]() { this->processChunk(i); });
    }
}

WorkerPool::~WorkerPool() {
    {
        std::unique_lock<std::mutex> lock(mainMutex);
        stopProcessing = true;
        startTask = true;
    }
    cv.notify_all();
    for (auto& t : threads) {
        t.join();
    }
}

ska::flat_hash_map<pair<int, int>, int, hasher> WorkerPool::runProcess(ska::flat_hash_set<pair<int, int>, hasher>& aliveCells) {
    size_t chunk_size = (aliveCells.size() + t - 1) / t;

    
    {
        std::unique_lock<mutex> lock(mainMutex);
        startTask = false;
    }

    {
        std::unique_lock<mutex> lock(mainMutex);
        this->data = &aliveCells;
        this->chunkSize = chunk_size;
        this->finishedCount = 0;
        this->startTask = true; 
        for (auto& chunk : chunksNeighbors) {
            chunk.clear();
        }
    }

    cv.notify_all();

    std::unique_lock<mutex> lock(syncProcessMutex);
    syncProcessCV.wait(lock, [this]() {
        return finishedCount == t;
        });

    ska::flat_hash_map<pair<int, int>, int, hasher> neighbors;
    for (const auto& chunk : chunksNeighbors) {
        for (const auto& [cell, count] : chunk) {
            neighbors[cell] += count;
        }
    }

    return neighbors;
}

void WorkerPool::processChunk(size_t threadIndex) {
    while (true) {
        {
            std::unique_lock<mutex> lock(mainMutex);
            cv.wait(lock, [this]() {
                return stopProcessing || startTask;
                });

            if (stopProcessing) return;
        }

        auto startIt = data->begin();
        std::advance(startIt, threadIndex * chunkSize);

        auto endIt = data->end();
        if ((threadIndex + 1) * chunkSize < data->size()) {
            endIt = data->begin();
            std::advance(endIt, (threadIndex + 1) * chunkSize);
        }

        for (auto it = startIt; it != endIt; ++it) {
            auto cell = *it;

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    chunksNeighbors[threadIndex][{cell.first + dx, cell.second + dy}] += 1;
                }
            }
        }

        {
            std::unique_lock<std::mutex> sync_lock(syncProcessMutex);
            ++finishedCount;
            if (finishedCount == t) {
                syncProcessCV.notify_one();
            }
        }
    }
}
