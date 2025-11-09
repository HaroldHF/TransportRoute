#pragma once
#include <string>
#include "Graph.h"

namespace transport {

    class RoutesFile {
    public:
        // formato: "u v peso"
        static bool load(const std::string& path, Graph& g);
        static bool save(const std::string& path, const Graph& g);
    };

    class ClosuresFile {
    public:
        // formato: "u v"
        static bool applyClosures(const std::string& path, Graph& g);
    };

} // namespace transport
