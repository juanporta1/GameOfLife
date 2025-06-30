#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <utility>
#include <flat_hash_map.hpp>
#include <condition_variable>
#include <atomic>
using namespace std;
struct hasher {
	std::size_t operator()(std::pair<int, int> p) const {
		std::hash<int> hasher;
		return hasher(p.first) ^ (hasher(p.second) << 1); // XOR de los hashes de las coordenadas
	}
};

class WorkerPool {
public:
	WorkerPool(int t);
	~WorkerPool();
	ska::flat_hash_map<pair<int,int>, int, hasher> runProcess(ska::flat_hash_set<pair<int,int>,hasher>& aliveCells);
private:
	size_t t;
	size_t finishedCount = 0;
	vector<thread> threads;
	vector<ska::flat_hash_map<pair<int, int>, int, hasher>> chunksNeighbors;
	
	ska::flat_hash_set<pair<int, int>, hasher>* data;
	size_t chunkSize = 0;
	mutex mainMutex;
	mutex syncProcessMutex;
	condition_variable cv;
	condition_variable syncProcessCV;
	
	atomic<bool> stopProcessing = false;
	atomic<bool> startTask = false;


	void processChunk(size_t t);
};