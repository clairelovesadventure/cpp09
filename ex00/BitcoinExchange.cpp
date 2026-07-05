#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& src) {
    *this = src;
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& rhs) {
    if (this != &rhs) {
        this->_database = rhs._database;
    }
    return *this;
}

BitcoinExchange::~BitcoinExchange() {}

void BitcoinExchange::_trim(std::string& str) const {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        str = "";
        return;
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    str = str.substr(first, (last - first + 1));
}

bool BitcoinExchange::_isValidDate(const std::string& date) const {
    if (date.length() != 10 || date[4] != '-' || date[7] != '-')
        return false;

    std::string y = date.substr(0, 4);
    std::string m = date.substr(5, 2);
    std::string d = date.substr(8, 2);

    int year = std::atoi(y.c_str());
    int month = std::atoi(m.c_str());
    int day = std::atoi(d.c_str());

    if (year < 2008 || month < 1 || month > 12 || day < 1 || day > 31)
        return false;

    // 闰年判断与各月份天数检查
    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        daysInMonth[1] = 29;

    if (day > daysInMonth[month - 1])
        return false;

    return true;
}

bool BitcoinExchange::_isValidValue(const std::string& valueStr, float& value) const {
    if (valueStr.empty()) return false;
    
    char* endptr;
    // C++98 中没有 std::stof，通常使用 strtof 或 sstream
    value = std::strtof(valueStr.c_str(), &endptr);
    if (*endptr != '\0') return false;

    if (value < 0) {
        std::cerr << "Error: not a positive number." << std::endl;
        return false;
    }
    if (value > 1000) {
        std::cerr << "Error: too large a number." << std::endl;
        return false;
    }
    return true;
}

// 加载原始 data.csv
bool BitcoinExchange::loadDatabase(const std::string& dbPath) {
    std::ifstream file(dbPath.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: could not open database file: " << dbPath << std::endl;
        return false;
    }

    std::string line;
    // 跳过第一行（标题行）
    std::getline(file, line);

    while (std::getline(file, line)) {
        size_t commaPos = line.find(',');
        if (commaPos == std::string::npos) continue;

        std::string date = line.substr(0, commaPos);
        std::string rateStr = line.substr(commaPos + 1);
        _trim(date);
        _trim(rateStr);

        float rate = std::strtof(rateStr.c_str(), NULL);
        _database[date] = rate;
    }
    file.close();
    return true;
}

// 解析用户的 input.txt 并输出
void BitcoinExchange::processInput(const std::string& inputPath) const {
    std::ifstream file(inputPath.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }

    std::string line;
    // 读取第一行检查头部格式
    if (std::getline(file, line)) {
        _trim(line);
        if (line != "date | value") {
            // 如果第一行不是标准的头，需要当普通行处理或者特殊提示。这里按题目示例跳过。
        }
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t pipePos = line.find('|');
        if (pipePos == std::string::npos) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::string date = line.substr(0, pipePos);
        std::string valStr = line.substr(pipePos + 1);
        _trim(date);
        _trim(valStr);

        if (!_isValidDate(date)) {
            std::cerr << "Error: bad input => " << date << std::endl;
            continue;
        }

        float value;
        if (!_isValidValue(valStr, value)) {
            continue; // _isValidValue 内部已输出对应错误
        }

        // 查找比特币汇率（关键逻辑）
        // std::map 会自动根据 Key 排序。
        std::map<std::string, float>::const_iterator it = _database.find(date);
        if (it != _database.end()) {
            std::cout << date << " => " << value << " = " << (value * it->second) << std::endl;
        } else {
            // upper_bound 找到第一个“严格大于”给定 date 的元素
            it = _database.upper_bound(date);
            if (it == _database.begin()) {
                // 比数据库里最早的一天还要早
                std::cerr << "Error: date " << date << " is before the earliest record." << std::endl;
            } else {
                // 递减迭代器，即为“小于该日期的最接近的一天”
                --it;
                std::cout << date << " => " << value << " = " << (value * it->second) << std::endl;
            }
        }
    }
    file.close();
}