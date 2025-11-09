#pragma once
#include <string>

namespace transport {

    struct Station {
        int id = -1;
        std::string name;

        Station() = default;
        Station(int id_, std::string name_) : id(id_), name(std::move(name_)) {}
        bool operator<(const Station& other) const { return id < other.id; }
        bool operator==(const Station& other) const { return id == other.id; }
    };

}