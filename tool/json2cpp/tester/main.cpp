#include <iostream>
#include <stdint.h>
#include "parser.h"

int32_t main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Please input JSON!" << std::endl;
        return -1;
    }

    std::string str_ut = json2cpp::buildStruct(argv[1]);

    std::cout << "// Generated according to JSON, for reference only!" << std::endl;
    std::cout << str_ut << std::endl;

    return 0;
}
