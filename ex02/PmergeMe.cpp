#include "PmergeMe.hpp"
#include <sstream>
#include <cstdlib>
#include <algorithm>

PmergeMe::PmergeMe() : _vectorTime(0), _dequeTime(0) {}

PmergeMe::~PmergeMe() {}

bool PmergeMe::parseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.empty()) return false;
        
        for (size_t j = 0; j < arg.length(); ++j) {
            if (!isdigit(arg[j])) return false;
        }
        
        // C++98 标准安全转化数字
        std::stringstream ss(arg);
        long val;
        if (!(ss >> val) || val < 0 || val > 2147483647) return false;
        
        _vectorData.push_back(static_cast<int>(val));
        _dequeData.push_back(static_cast<int>(val));
    }
    return true;
}

// ==================== VECTOR ALGORITHM ====================
void PmergeMe::_insertVector(std::vector<int>& mainChain, std::vector<int>& pendChain, const std::vector<int>& jacobsthal) {
    size_t pendIdx = 0;
    size_t jIdx = 1; // 从雅各布斯塔尔数的第2项开始跨步

    while (pendIdx < pendChain.size()) {
        size_t targetGroupSize = (jIdx < jacobsthal.size()) ? jacobsthal[jIdx] : pendChain.size();
        if (targetGroupSize > pendChain.size()) targetGroupSize = pendChain.size();

        // 按照雅各布斯塔尔数的跨度，从后往前插入
        for (size_t i = targetGroupSize; i > pendIdx; --i) {
            int item = pendChain[i - 1];
            std::vector<int>::iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), item);
            mainChain.insert(it, item);
        }
        pendIdx = targetGroupSize;
        jIdx++;
    }
}

void PmergeMe::_sortVector(std::vector<int>& vec) {
    if (vec.size() <= 1) return;

    // 1. 两两分组并比较
    std::vector<int> mainChain;
    std::vector<int> pendChain;
    bool hasStraggler = (vec.size() % 2 != 0);
    int straggler = hasStraggler ? vec.back() : 0;

    // 这里简化为成对抽取，为了维持伴随关系，可以采用 Pair，但在 C++98 中纯数对拆分也可实现
    for (size_t i = 0; i < vec.size() - (hasStraggler ? 1 : 0); i += 2) {
        if (vec[i] > vec[i + 1]) {
            mainChain.push_back(vec[i]);
            pendChain.push_back(vec[i + 1]);
        } else {
            mainChain.push_back(vec[i + 1]);
            pendChain.push_back(vec[i]);
        }
    }

    // 2. 递归排序主链
    _sortVector(mainChain);

    // 3. 生成雅各布斯塔尔数列 (预设足够应对 3000+ 数量级)
    std::vector<int> jacobsthal;
    jacobsthal.push_back(1); jacobsthal.push_back(3); jacobsthal.push_back(5);
    jacobsthal.push_back(11); jacobsthal.push_back(21); jacobsthal.push_back(43);
    jacobsthal.push_back(85); jacobsthal.push_back(171); jacobsthal.push_back(341);
    jacobsthal.push_back(683); jacobsthal.push_back(1365); jacobsthal.push_back(2731);

    // 4. 将副链和落单项用二分查找插入主链
    _insertVector(mainChain, pendChain, jacobsthal);
    if (hasStraggler) {
        std::vector<int>::iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(it, straggler);
    }

    vec = mainChain;
}

// ==================== DEQUE ALGORITHM ====================
void PmergeMe::_insertDeque(std::deque<int>& mainChain, std::deque<int>& pendChain, const std::deque<int>& jacobsthal) {
    size_t pendIdx = 0;
    size_t jIdx = 1;

    while (pendIdx < pendChain.size()) {
        size_t targetGroupSize = (jIdx < jacobsthal.size()) ? jacobsthal[jIdx] : pendChain.size();
        if (targetGroupSize > pendChain.size()) targetGroupSize = pendChain.size();

        for (size_t i = targetGroupSize; i > pendIdx; --i) {
            int item = pendChain[i - 1];
            std::deque<int>::iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), item);
            mainChain.insert(it, item);
        }
        pendIdx = targetGroupSize;
        jIdx++;
    }
}

void PmergeMe::_sortDeque(std::deque<int>& deq) {
    if (deq.size() <= 1) return;

    std::deque<int> mainChain;
    std::deque<int> pendChain;
    bool hasStraggler = (deq.size() % 2 != 0);
    int straggler = hasStraggler ? deq.back() : 0;

    for (size_t i = 0; i < deq.size() - (hasStraggler ? 1 : 0); i += 2) {
        if (deq[i] > deq[i + 1]) {
            mainChain.push_back(deq[i]);
            pendChain.push_back(deq[i + 1]);
        } else {
            mainChain.push_back(deq[i + 1]);
            pendChain.push_back(deq[i]);
        }
    }

    _sortDeque(mainChain);

    std::deque<int> jacobsthal;
    jacobsthal.push_back(1); jacobsthal.push_back(3); jacobsthal.push_back(5);
    jacobsthal.push_back(11); jacobsthal.push_back(21); jacobsthal.push_back(43);
    jacobsthal.push_back(85); jacobsthal.push_back(171); jacobsthal.push_back(341);
    jacobsthal.push_back(683); jacobsthal.push_back(1365); jacobsthal.push_back(2731);

    _insertDeque(mainChain, pendChain, jacobsthal);
    if (hasStraggler) {
        std::deque<int>::iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(it, straggler);
    }

    deq = mainChain;
}

// ==================== EXECUTION & DISPLAY ====================
void PmergeMe::run() {
    if (_vectorData.empty()) return;

    // 打印未排序序列
    std::cout << "Before: ";
    for (size_t i = 0; i < _vectorData.size(); ++i) {
        std::cout << _vectorData[i] << " ";
    }
    std::cout << std::endl;

    struct timeval start, end;

    // 1. 测试 Vector 耗时
    gettimeofday(&start, NULL);
    _sortVector(_vectorData);
    gettimeofday(&end, NULL);
    _vectorTime = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);

    // 2. 测试 Deque 耗时
    gettimeofday(&start, NULL);
    _sortDeque(_dequeData);
    gettimeofday(&end, NULL);
    _dequeTime = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);

    // 打印排序后序列
    std::cout << "After:  ";
    for (size_t i = 0; i < _vectorData.size(); ++i) {
        std::cout << _vectorData[i] << " ";
    }
    std::cout << std::endl;

    // 严格按题目要求的输出时间格式
    std::cout << "Time to process a range of " << _vectorData.size() 
              << " elements with std::vector : " << _vectorTime << " us" << std::endl;
    std::cout << "Time to process a range of " << _dequeData.size() 
              << " elements with std::deque  : " << _dequeTime << " us" << std::endl;
}