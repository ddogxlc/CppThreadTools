#ifndef TOOLS_PARALLEL_ACCUMULATE_H
#define TOOLS_PARALLEL_ACCUMULATE_H

#include <thread>
#include <vector>
#include <numeric>
#include <algorithm>

template<typename Iterator,typename T>
struct AccumulateBlock {
    void operator()(Iterator first, Iterator last, T& result)
    {
        result = std::accumulate(first, last, 0);
    }
};


template<typename Iterator, typename T>
void AccumulateFunc(Iterator first, Iterator last, T& result)
{
    result = std::accumulate(first, last, 0);
}

unsigned GetThreadNum(uint32_t perBlockNum, uint32_t num)
{
    uint32_t hardwareThreadLimit = std::thread::hardware_concurrency();
    uint32_t logicLimit = (num + perBlockNum - 1) / perBlockNum;
    return std::min(hardwareThreadLimit ? hardwareThreadLimit : 2, logicLimit);
}
template<typename Iterator, typename T>
T ParallelAccumulate(Iterator first, Iterator last, T init)
{
    uint32_t length = std::distance(first, last);
    if (length == 0) {
        return init;
    }
    uint32_t threadNum = GetThreadNum(25, length);
    std::vector<std::thread> threads(threadNum);
    std::vector<T> results(threadNum);
    uint32_t blockSize = length / threadNum;
    Iterator start = first;
    Iterator end = start;
    for (int i = 0; i < threadNum - 1; i++) {
        std::advance(end, blockSize);
        threads[i] = std::thread(AccumulateBlock<Iterator, T>(), start, end,
            std::ref(results[i]));
        start = end;
    }
    if (end != last) {
        threads[threadNum - 1] = std::thread(AccumulateBlock<Iterator, T>(), start, last, std::ref(results[threadNum - 1]));
    }
    for (auto& t : threads) {
        t.join();
    }
    return std::accumulate(results.begin(), results.end(), init);
}
#endif // TOOLS_PARALLEL_ACCUMULATE_H
