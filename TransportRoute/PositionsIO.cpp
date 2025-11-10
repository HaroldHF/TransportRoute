#include "PositionsIO.h"
#include <fstream>
bool PositionsIO::save(const std::string& path, const std::unordered_map<int, QPointF>& pos) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) return false;
    for (const auto& [id, p] : pos) out << id << " " << p.x() << " " << p.y() << "\n";
    return true;
}
bool PositionsIO::load(const std::string& path, std::unordered_map<int, QPointF>& out) {
    std::ifstream in(path);
    if (!in) return false;
    out.clear(); int id; double x, y;
    while (in >> id >> x >> y) out[id] = QPointF(x, y);
    return true;
}
