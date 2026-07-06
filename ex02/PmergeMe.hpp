#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <sys/time.h>

class PmergeMe {
private:
    std::vector<int> _vectorData;
    std::deque<int>  _dequeData;
    double           _vectorTime;
    double           _dequeTime;

    // ==================== VECTOR HELPER FUNCTIONS ====================
    // 参数2改为传入包含(大数, 小数)对的 sortedPairs 数组
    void _insertVector(std::vector<int>& mainChain, 
                       std::vector<std::pair<int, int> >& sortedPairs, 
                       const std::vector<int>& jacobsthal);
    void _sortVector(std::vector<int>& vec);

    // ==================== DEQUE HELPER FUNCTIONS ====================
    // 参数2同样改为包含(大数, 小数)对的 sortedPairs 数组
    void _insertDeque(std::deque<int>& mainChain, 
                      std::deque<std::pair<int, int> >& sortedPairs, 
                      const std::deque<int>& jacobsthal);
    void _sortDeque(std::deque<int>& deq);

    // 禁止外部拷贝和赋值（符合 Orthodox Canonical Form 规范）
    PmergeMe(const PmergeMe& src);
    PmergeMe& operator=(const PmergeMe& rhs);

public:
    PmergeMe();
    ~PmergeMe();

    bool parseArguments(int argc, char** argv);
    void run();
};

// ==================== GLOBAL HELPER FUNCTIONS ====================
// 在 C++98 中，如果想在算法类外部或内部使用更清晰的结构，可以在头文件中声明 pair 比较函数
bool comparePairsVector(const std::pair<int, int>& a, const std::pair<int, int>& b);
bool comparePairsDeque(const std::pair<int, int>& a, const std::pair<int, int>& b);

#endif