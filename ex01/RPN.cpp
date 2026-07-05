#include "RPN.hpp"
#include <sstream>
#include <cstdlib>

RPN::RPN() {}

RPN::~RPN() {}

bool RPN::_isOperator(char c) const {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

bool RPN::_executeOperation(char op) {
    if (_stack.size() < 2) {
        return false;
    }

    // 注意：先弹出来的是右操作数 (b)，后弹出来的是左操作数 (a)
    int b = _stack.top();
    _stack.pop();
    int a = _stack.top();
    _stack.pop();

    if (op == '+') _stack.push(a + b);
    else if (op == '-') _stack.push(a - b);
    else if (op == '*') _stack.push(a * b);
    else if (op == '/') {
        if (b == 0) { // 防御性编程：处理除以 0 的情况
            return false;
        }
        _stack.push(a / b);
    }
    return true;
}

void RPN::calculate(const std::string& expression) {
    std::stringstream ss(expression);
    std::string token;

    while (ss >> token) {
        // 约束 1：如果 token 长度大于 1，说明不是单个数字或单个符号，属于非法输入
        if (token.length() != 1) {
            std::cerr << "Error" << std::endl;
            return;
        }

        char c = token[0];

        if (isdigit(c)) {
            // 约束 2：题目要求输入的数字必须小于 10 (即 0-9)
            _stack.push(c - '0');
        } else if (_isOperator(c)) {
            // 约束 3：如果操作数不够弹，或者遇到了除以 0 错误
            if (!_executeOperation(c)) {
                std::cerr << "Error" << std::endl;
                return;
            }
        } else {
            // 约束 4：出现了非法字符
            std::cerr << "Error" << std::endl;
            return;
        }
    }

    // 约束 5：表达式结束后，栈内必须恰好只剩 1 个最终结果
    if (_stack.size() != 1) {
        std::cerr << "Error" << std::endl;
        return;
    }

    // 打印最终正确结果到标准输出
    std::cout << _stack.top() << std::endl;
}