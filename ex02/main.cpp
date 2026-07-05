#include "PmergeMe.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Error: Please provide a positive integer sequence." << std::endl;
        return 1;
    }

    PmergeMe sorter;
    if (!sorter.parseArguments(argc, argv)) {
        std::cerr << "Error" << std::endl;
        return 1;
    }

    sorter.run();
    return 0;
}