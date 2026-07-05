#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <sys/time.h>

class PmergeMe {
    private:
        std::vector<int>    _vectorData;
        std::deque<int>     _dequeData;
        
        double              _vectorTime;
        double              _dequeTime;

        // Forbidden Group in Orthodox Canonical Form (or privatized)
        PmergeMe(const PmergeMe& src);
        PmergeMe& operator=(const PmergeMe& rhs);

        // Vector 专属 Ford-Johnson 算法内部辅助函数
        void _sortVector(std::vector<int>& vec);
        void _insertVector(std::vector<int>& mainChain, std::vector<int>& pendChain, const std::vector<int>& jacobsthal);

        // Deque 专属 Ford-Johnson 算法内部辅助函数
        void _sortDeque(std::deque<int>& deq);
        void _insertDeque(std::deque<int>& mainChain, std::deque<int>& pendChain, const std::deque<int>& jacobsthal);

    public:
        PmergeMe();
        ~PmergeMe();

        bool parseArguments(int argc, char** argv);
        void run();
};

#endif