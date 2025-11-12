#pragma once
#include <string>

namespace transport {
    struct Station {
        int id = -1;
        std::string name;
        double x = 0.0;  // coordenada X para visualización
        double y = 0.0;  // coordenada Y para visualización

        Station() = default;
        Station(int id_, std::string name_)
            : id(id_), name(std::move(name_)), x(0.0), y(0.0) {
        }
        Station(int id_, std::string name_, double x_, double y_)
            : id(id_), name(std::move(name_)), x(x_), y(y_) {
        }

        bool operator<(const Station& other) const { return id < other.id; }
        bool operator==(const Station& other) const { return id == other.id; }
    };
}