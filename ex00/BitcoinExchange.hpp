#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <iostream>
#include <string>
#include <map>

class BitcoinExchange {
private:
    // 使用 std::map 存储比特币数据库，Key 为日期字符串，Value 为汇率
    std::map<std::string, float> _database;

    // 私有辅助函数
    bool    _isValidDate(const std::string& date) const;
    bool    _isValidValue(const std::string& valueStr, float& value) const;
    void    _trim(std::string& str) const;

public:
    // Orthodox Canonical Form (正统经典类格式)
    BitcoinExchange();
    BitcoinExchange(const BitcoinExchange& src);
    BitcoinExchange& operator=(const BitcoinExchange& rhs);
    ~BitcoinExchange();

    // 核心功能
    bool    loadDatabase(const std::string& dbPath);
    void    processInput(const std::string& inputPath) const;
};

#endif