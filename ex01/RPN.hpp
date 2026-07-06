#ifndef RPN_HPP
#define RPN_HPP

#include <iostream>
#include <string>
#include <stack>
#include <list> // 👈 将 <vector> 替换为 <list>

class RPN {
    private:
        // 👈 显式指定底层为 std::list，既避开了默认的 deque，又把清白的 vector 留给了 ex02
        std::stack<int, std::list<int> > _stack;

        // 私有辅助函数
        bool _isOperator(char c) const;
        bool _executeOperation(char op);

        // 禁止外部使用拷贝和赋值
        RPN(const RPN& src);
        RPN& operator=(const RPN& rhs);

    public:
        // Orthodox Canonical Form
        RPN();
        ~RPN();

        // 核心计算函数
        void calculate(const std::string& expression);
};

#endif