#include <vector>
#include <string>
#include <iostream>

int main() {
     std::vector<std::string> vec;
     vec.push_back("hello");
     vec.push_back("world");
     for (auto iter : vec) {
         std::cout << iter << std::endl;
     }
}