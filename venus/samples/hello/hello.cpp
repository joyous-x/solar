#include <vector>
#include <string>
#include <iostream>

#include "common/utils.h"

int main() {
    std::vector<std::string> vec;
    FileUtils::enum_dir("/Users/jiao/Workspace/solar/venus/samples", vec);

    for (auto iter : vec) {
        std::cout << iter << std::endl;
    }
}