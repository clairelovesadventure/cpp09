#ifndef RPN_HPP
#define RPN_HPP

#include <iostream>
#include <string>
#include <stack>
#include <vector>

class RPN {
    private:
        // 显式指定底层为 std::vector，防止“白嫖”了默认的 deque
        std::stack<int, std::vector<int> > _stack;

        // 私有辅助函数
        bool _isOperator(char c) const;
        bool _executeOperation(char op);

        // 禁止外部使用拷贝和赋值（或者你也可以像 Ex00 一样完整实现它们）
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