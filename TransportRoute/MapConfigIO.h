#pragma once
#include <string>

struct MapConfigIO {
    static bool save(const std::string& path, const std::string& imagePath);
    static bool load(const std::string& path, std::string& imagePathOut);
};
