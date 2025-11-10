#include "MapConfigIO.h"
#include <fstream>
bool MapConfigIO::save(const std::string& path, const std::string& img) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) return false;
    out << img << "\n";
    return true;
}
bool MapConfigIO::load(const std::string& path, std::string& img) {
    std::ifstream in(path);
    if (!in) return false;
    std::getline(in, img);
    return !img.empty();
}
