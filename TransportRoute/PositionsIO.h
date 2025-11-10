#pragma once
#include <string>
#include <unordered_map>
#include <QPointF>

struct PositionsIO {
    static bool save(const std::string& path, const std::unordered_map<int, QPointF>& pos);
    static bool load(const std::string& path, std::unordered_map<int, QPointF>& out);
};
