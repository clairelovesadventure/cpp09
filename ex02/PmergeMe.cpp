#include "PmergeMe.hpp"
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <sys/time.h>
#include <iostream>

PmergeMe::PmergeMe() : _vectorTime(0), _dequeTime(0) {}

PmergeMe::~PmergeMe() {}

bool PmergeMe::parseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.empty()) return false;
        
        for (size_t j = 0; j < arg.length(); ++j) {
            if (!isdigit(arg[j])) return false;
        }
        
        std::stringstream ss(arg);
        long val;
        if (!(ss >> val) || val < 0 || val > 2147483647) return false;
        
        _vectorData.push_back(static_cast<int>(val));
        _dequeData.push_back(static_cast<int>(val));
    }
    return true;
}

// ==================== VECTOR ALGORITHM ====================

// 专门为 pair 的 first（即大数主链）排序定义的比较函数，符合 C++98 标准
bool comparePairsVector(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.first < b.first;
}

void PmergeMe::_insertVector(std::vector<int>& mainChain, std::vector<std::pair<int, int> >& sortedPairs, const std::vector<int>& jacobsthal) {
    // 此时 sortedPairs 已经是根据 first(大数) 排好序的了
    // sortedPairs[k].first 已经按顺序存在于 mainChain 中
    // sortedPairs[k].second 就是我们需要插入的副链元素 Pk

    size_t pendSize = sortedPairs.size();
    if (pendSize == 0) return;

    // 【优化点 1】：体现 0 次比较插入 P1
    // 因为已知 sortedPairs[0].second (P1) <= sortedPairs[0].first (M1)
    // 而 M1 目前在 mainChain 的什么位置？因为 mainChain 一开始只有大数，M1 就是 mainChain[0]
    // 所以 P1 绝对小于或等于 mainChain[0]，直接插入到最前面，完全不需要 lower_bound 比较！
    mainChain.insert(mainChain.begin(), sortedPairs[0].second);

    // 用一个数组来记录主链中各个大数(first)当前所处的动态位置，用来精准限制二分查找的右边界(上限)
    // 这才真正发挥了雅各布塔尔数“掐住主链增长边界”的数学威力
    size_t pendIdx = 1; // P1 (索引0) 已经插过了，从索引1开始
    size_t jIdx = 1;    // 雅各布斯塔尔数索引，从 jacobsthal[1] = 3 开始跨步

    while (pendIdx < pendSize) {
        size_t targetGroupSize = (jIdx < jacobsthal.size()) ? jacobsthal[jIdx] : pendSize;
        if (targetGroupSize > pendSize) targetGroupSize = pendSize;

        // 雅各布斯塔尔区间跨步：从后往前倒序插入
        for (size_t i = targetGroupSize; i > pendIdx; --i) {
            int itemToInsert = sortedPairs[i - 1].second; // 当前要插入的 P_i
            int companionMain = sortedPairs[i - 1].first; // 它的伴随大数 M_i

            // 【雅各布塔尔核心逻辑】：二分查找的右边界不需要是 mainChain.end()！
            // 因为我们百分之百确定 P_i <= M_i，所以只需要在 mainChain 中找到 M_i 当前的位置即可！
            // 这样就死死掐住了搜索区间，保证比较次数绝不超标
            std::vector<int>::iterator endIt = std::find(mainChain.begin(), mainChain.end(), companionMain);
            
            std::vector<int>::iterator it = std::lower_bound(mainChain.begin(), endIt, itemToInsert);
            mainChain.insert(it, itemToInsert);
        }
        pendIdx = targetGroupSize;
        jIdx++;
    }
}

void PmergeMe::_sortVector(std::vector<int>& vec) {
    if (vec.size() <= 1) return;

    // 1. 两两分组并比较，使用 pair 牢牢绑定伴随关系
    std::vector<std::pair<int, int> > pairs;
    bool hasStraggler = (vec.size() % 2 != 0);
    int straggler = hasStraggler ? vec.back() : 0;

    for (size_t i = 0; i < vec.size() - (hasStraggler ? 1 : 0); i += 2) {
        if (vec[i] > vec[i + 1]) {
            pairs.push_back(std::make_pair(vec[i], vec[i + 1]));
        } else {
            pairs.push_back(std::make_pair(vec[i + 1], vec[i]));
        }
    }

    // 2. 提取出所有的大数，组成临时的主链进行递归排序
    std::vector<int> mainChainData;
    for (size_t i = 0; i < pairs.size(); ++i) {
        mainChainData.push_back(pairs[i].first);
    }
    _sortVector(mainChainData); // 递归让大数们有序

    // 3. 关键点：根据已经排序好的 mainChainData 顺序，重新调整 pairs 的顺序
    // 这样我们既得到了有序的主链，又让副链元素紧紧跟随着各自的大数，伴随关系没有乱！
    std::vector<std::pair<int, int> > sortedPairs;
    for (size_t i = 0; i < mainChainData.size(); ++i) {
        for (size_t j = 0; j < pairs.size(); ++j) {
            if (pairs[j].first == mainChainData[i]) {
                sortedPairs.push_back(pairs[j]);
                break;
            }
        }
    }

    // 4. 生成雅各布斯塔尔数列
    std::vector<int> jacobsthal;
    jacobsthal.push_back(1); jacobsthal.push_back(3); jacobsthal.push_back(5);
    jacobsthal.push_back(11); jacobsthal.push_back(21); jacobsthal.push_back(43);
    jacobsthal.push_back(85); jacobsthal.push_back(171); jacobsthal.push_back(341);
    jacobsthal.push_back(683); jacobsthal.push_back(1365); jacobsthal.push_back(2731);

    // 5. 将伴随副链用二分查找插入主链（此时 mainChainData 作为初始主链传入）
    _insertVector(mainChainData, sortedPairs, jacobsthal);

    // 6. 最后处理奇数落单的元素
    if (hasStraggler) {
        std::vector<int>::iterator it = std::lower_bound(mainChainData.begin(), mainChainData.end(), straggler);
        mainChainData.insert(it, straggler);
    }

    vec = mainChainData;
}

// ==================== DEQUE ALGORITHM ====================

bool comparePairsDeque(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.first < b.first;
}

void PmergeMe::_insertDeque(std::deque<int>& mainChain, std::deque<std::pair<int, int> >& sortedPairs, const std::deque<int>& jacobsthal) {
    size_t pendSize = sortedPairs.size();
    if (pendSize == 0) return;

    // 0次比较插入 P1
    mainChain.push_front(sortedPairs[0].second);

    size_t pendIdx = 1;
    size_t jIdx = 1;

    while (pendIdx < pendSize) {
        size_t targetGroupSize = (jIdx < jacobsthal.size()) ? jacobsthal[jIdx] : pendSize;
        if (targetGroupSize > pendSize) targetGroupSize = pendSize;

        for (size_t i = targetGroupSize; i > pendIdx; --i) {
            int itemToInsert = sortedPairs[i - 1].second;
            int companionMain = sortedPairs[i - 1].first;

            std::deque<int>::iterator endIt = std::find(mainChain.begin(), mainChain.end(), companionMain);
            std::deque<int>::iterator it = std::lower_bound(mainChain.begin(), endIt, itemToInsert);
            mainChain.insert(it, itemToInsert);
        }
        pendIdx = targetGroupSize;
        jIdx++;
    }
}

void PmergeMe::_sortDeque(std::deque<int>& deq) {
    if (deq.size() <= 1) return;

    std::deque<std::pair<int, int> > pairs;
    bool hasStraggler = (deq.size() % 2 != 0);
    int straggler = hasStraggler ? deq.back() : 0;

    for (size_t i = 0; i < deq.size() - (hasStraggler ? 1 : 0); i += 2) {
        if (deq[i] > deq[i + 1]) {
            pairs.push_back(std::make_pair(deq[i], deq[i + 1]));
        } else {
            pairs.push_back(std::make_pair(deq[i + 1], deq[i]));
        }
    }

    std::deque<int> mainChainData;
    for (size_t i = 0; i < pairs.size(); ++i) {
        mainChainData.push_back(pairs[i].first);
    }
    _sortDeque(mainChainData);

    std::deque<std::pair<int, int> > sortedPairs;
    for (size_t i = 0; i < mainChainData.size(); ++i) {
        for (size_t j = 0; j < pairs.size(); ++j) {
            if (pairs[j].first == mainChainData[i]) {
                sortedPairs.push_back(pairs[j]);
                break;
            }
        }
    }

    std::deque<int> jacobsthal;
    jacobsthal.push_back(1); jacobsthal.push_back(3); jacobsthal.push_back(5);
    jacobsthal.push_back(11); jacobsthal.push_back(21); jacobsthal.push_back(43);
    jacobsthal.push_back(85); jacobsthal.push_back(171); jacobsthal.push_back(341);
    jacobsthal.push_back(683); jacobsthal.push_back(1365); jacobsthal.push_back(2731);

    _insertDeque(mainChainData, sortedPairs, jacobsthal);
    if (hasStraggler) {
        std::deque<int>::iterator it = std::lower_bound(mainChainData.begin(), mainChainData.end(), straggler);
        mainChainData.insert(it, straggler);
    }

    deq = mainChainData;
}

// ==================== EXECUTION & DISPLAY ====================
void PmergeMe::run() {
    if (_vectorData.empty()) return;

    std::cout << "Before: ";
    for (size_t i = 0; i < _vectorData.size(); ++i) {
        std::cout << _vectorData[i] << " ";
    }
    std::cout << std::endl;

    struct timeval start, end;

    gettimeofday(&start, NULL);
    _sortVector(_vectorData);
    gettimeofday(&end, NULL);
    _vectorTime = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);

    gettimeofday(&start, NULL);
    _sortDeque(_dequeData);
    gettimeofday(&end, NULL);
    _dequeTime = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);

    std::cout << "After:  ";
    for (size_t i = 0; i < _vectorData.size(); ++i) {
        std::cout << _vectorData[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Time to process a range of " << _vectorData.size() 
              << " elements with std::vector : " << _vectorTime << " us" << std::endl;
    std::cout << "Time to process a range of " << _dequeData.size() 
              << " elements with std::deque  : " << _dequeTime << " us" << std::endl;
}