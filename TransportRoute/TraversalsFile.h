#pragma once
#pragma once
#include <string>
#include <vector>
#include "Station.h"

namespace transport {
    class TraversalsFile {
    public:
        static bool writeAll(const std::string& path,
            const std::vector<Station>& inOrder,
            const std::vector<Station>& preOrder,
            const std::vector<Station>& postOrder);
    };
}
