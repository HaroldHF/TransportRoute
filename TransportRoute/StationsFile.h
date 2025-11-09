#pragma once
#include <string>
#include <vector>
#include "Station.h"
#include "BST.h"

namespace transport {

    class StationsFile {
    public:
        // formato: "id;nombre"
        static std::vector<Station> load(const std::string& path);
        static bool save(const std::string& path, const std::vector<Station>& stations);

        // helper: cargar al BST
        static void loadIntoBST(const std::string& path, BST<Station>& bst);
    };

} // namespace transport