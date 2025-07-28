#include <WorkerPool.h>
#include <iostream>

using namespace std;
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
    vector<pair<int, int>> flatData(aliveCells.begin(), aliveCells.end()); 
    size_t chunk_size = (flatData.size() + t - 1) / t;

    {
       
        std::unique_lock<mutex> lock(mainMutex);
		this->flatData = std::move(flatData);
        this->chunkSize = chunk_size;
        this->finishedCount = 0;
        for (auto& chunk : chunksNeighbors) {
            chunk.clear();
        }
        this->startTask = true;  // Señal para que los hilos empiecen a trabajar
    }

    cv.notify_all();  // Despertar a todos los hilos

    {
        std::unique_lock<mutex> lock(syncProcessMutex);
        syncProcessCV.wait(lock, [this]() {
            return finishedCount == t;
            });
    }

    {
        std::unique_lock<mutex> lock(mainMutex);
        this->startTask = false;  // Resetear la bandera una vez terminada la tarea
    }

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
                return this->stopProcessing || this->startTask;
                });
            if (this->stopProcessing) return;
        }

        auto startIt = flatData.begin() + threadIndex * chunkSize;
        auto endIt = (threadIndex + 1) * chunkSize < flatData.size()
            ? flatData.begin() + (threadIndex + 1) * chunkSize
            : flatData.end();

        for (auto it = startIt; it != endIt; ++it) {
            auto& cell = *it;
            /*{
                std::unique_lock<mutex> lock(coutMutex);
                cout << "Celula: " << cell.first << ", " << cell.second << " en hilo " << threadIndex << endl;
            }*/
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
