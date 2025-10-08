#include "terminal.h"
#include <fstream>
void readFile(const std::string &path, std::vector<double>& inputs){
    std::ifstream fin;
    fin.open(path);
    if (!fin.is_open()) {
        std::cerr<<"cannot open the file";
        return;
    }
    double tmp;
    while (fin >> tmp){
        inputs.push_back(tmp);
    }
    if (inputs.size() % 3 != 0)
    {
        std::cerr<<"num error";
    }
}