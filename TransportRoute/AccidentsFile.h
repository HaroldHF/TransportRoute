#pragma once
#include <string>
#include "Graph.h"

namespace transport {
    class AccidentsFile {
    public:
        // suma delta a los pesos de ambas direcciones (si existen y no estan cerradas)
        static bool apply(const std::string& path, Graph& g);
    };
}
