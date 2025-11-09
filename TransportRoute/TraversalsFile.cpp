#include "TraversalsFile.h"
#include <fstream>

namespace transport {
    static void writeOne(std::ofstream& out, const char* title, const std::vector<Station>& v) {
        out << title << ": ";
        bool first = true;
        for (auto& s : v) { if (!first) out << ", "; out << s.id << " " << s.name; first = false; }
        out << "\n";
    }

    bool TraversalsFile::writeAll(const std::string& path,
        const std::vector<Station>& inOrder,
        const std::vector<Station>& preOrder,
        const std::vector<Station>& postOrder) {
        std::ofstream out(path, std::ios::trunc);
        if (!out) return false;
        writeOne(out, "IN-ORDER", inOrder);
        writeOne(out, "PRE-ORDER", preOrder);
        writeOne(out, "POST-ORDER", postOrder);
        return true;
    }
}
