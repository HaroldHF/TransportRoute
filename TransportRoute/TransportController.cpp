#include "TransportController.h"
#include "StationsFile.h"
#include "RoutesFile.h"
#include "ReportsFile.h"
#include "TraversalsFile.h"
#include "AccidentsFile.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace transport {

    static std::string nowStamp() {
        using namespace std::chrono;
        auto tp = system_clock::now();
        std::time_t t = system_clock::to_time_t(tp);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::ostringstream os;
        os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return os.str();
    }

    TransportController::TransportController()
        : stations([](const Station& s) { return s.id; }) {
    }

    bool TransportController::loadAll() {
        // limpiar estado
        stations.~BST<Station>();
        new (&stations) BST<Station>([](const Station& s) { return s.id; });
        graph.clear();
        invalidateAllPairs();

        // cargar estaciones
        StationsFile::loadIntoBST(estacionesPath, stations);

        // cargar rutas
        if (!RoutesFile::load(rutasPath, graph)) {
            return false;
        }
        // aplicar cierres (si el archivo existe)
        reloadClosures();

        // log simple
        auto ordered = stationsInOrder();
        ReportsFile::appendStationsInOrder(reportesPath, ordered);
        logLine("[" + nowStamp() + "] LoadAll: estaciones=" + std::to_string(ordered.size())
            + " verticesGraficados=" + std::to_string((int)graph.data().size()));
        return true;
    }

    bool TransportController::reloadClosures() {
        bool ok = ClosuresFile::applyClosures(cierresPath, graph);
        if (ok) invalidateAllPairs();
        logLine("[" + nowStamp() + "] ReloadClosures: applied=" + std::string(ok ? "true" : "false"));
        return ok;
    }

    bool TransportController::saveStations() const {
        auto ordered = stationsInOrder();
        return StationsFile::save(estacionesPath, ordered);
    }

    bool TransportController::saveRoutes() const {
        return RoutesFile::save(rutasPath, graph);
    }

    bool TransportController::exportTraversals() const {
        auto inO = stations.inOrder();
        auto preO = stations.preOrder();
        auto postO = stations.postOrder();
        return TraversalsFile::writeAll(recorridosPath, inO, preO, postO);
    }

    bool TransportController::reloadAccidents() {
        bool ok = AccidentsFile::apply(accidentesPath, graph);
        if (ok) invalidateAllPairs(); // cambian costos -> recomputar Floyd
        logLine("[" + nowStamp() + "] ReloadAccidents: applied=" + std::string(ok ? "true" : "false"));
        return ok;
    }

    bool TransportController::addStation(int id, const std::string& name) {
        // insert in BST
        stations.insert(Station{ id,name });
        // ensure vertex in graph
        graph.addVertex(id);
        exportTraversals(); // mantener recorridos al dia
        logLine("[" + nowStamp() + "] AddStation id=" + std::to_string(id) + " name=" + name);
        return true;
    }

    bool TransportController::removeStation(int id) {
        bool ok = stations.erase(id);
        // opcional: podrías eliminar (o vaciar) las aristas del grafo que lo usen
        // por simplicidad aquí solo eliminas del BST:
        logLine("[" + nowStamp() + "] RemoveStation id=" + std::to_string(id) + " ok=" + (ok ? "1" : "0"));
        exportTraversals();
        return ok;
    }

    bool TransportController::exportGraphSummary() {
        // conexiones
        logLine("=== GRAPH SUMMARY ===");
        for (const auto& [u, vec] : graph.data()) {
            for (const auto& e : vec) {
                if (u < e.to) { // una vez
                    std::ostringstream os;
                    os << "Edge " << u << " - " << e.to
                        << " w=" << e.w << (e.closed ? " [CLOSED]" : "");
                    logLine(os.str());
                }
            }
        }
        // estaciones ordenadas alfabeticamente
        auto inO = stations.inOrder();
        std::sort(inO.begin(), inO.end(), [](const Station& a, const Station& b) {
            return a.name < b.name;
            });
        logLine("Estaciones (alfabetico):");
        std::ostringstream os;
        for (size_t i = 0; i < inO.size(); ++i) { if (i) os << ", "; os << inO[i].id << " " << inO[i].name; }
        logLine(os.str());
        logLine("=====================");
        return true;
    }

    VisitResult TransportController::runBFS(int start) {
        auto r = AlgoFacade::runBFS(graph, start);
        std::ostringstream os; os << "[" << nowStamp() << "] BFS start=" << start << " order=";
        for (size_t i = 0; i < r.order.size(); ++i) { if (i) os << ","; os << r.order[i]; }
        logLine(os.str());
        return r;
    }

    VisitResult TransportController::runDFS(int start) {
        auto r = AlgoFacade::runDFS(graph, start);
        std::ostringstream os; os << "[" << nowStamp() << "] DFS start=" << start << " order=";
        for (size_t i = 0; i < r.order.size(); ++i) { if (i) os << ","; os << r.order[i]; }
        logLine(os.str());
        return r;
    }

    PathResult TransportController::runDijkstra(int src, int dst) {
        auto r = AlgoFacade::runDijkstra(graph, src, dst);
        auto list = stationsOnPath(r.path);
        std::ostringstream os2; os2 << "Ruta (" << r.algo << "): ";
        for (size_t i = 0; i < list.size(); ++i) { if (i) os2 << " -> "; os2 << list[i].id << " " << list[i].name; }
        logLine(os2.str()); // queda en reportes.txt
        std::ostringstream os; os << "[" << nowStamp() << "] Dijkstra " << src << "->" << dst
            << " reachable=" << (r.reachable ? "1" : "0")
            << " cost=" << r.cost << " path=";
        for (size_t i = 0; i < r.path.size(); ++i) { if (i) os << "-"; os << r.path[i]; }
        logLine(os.str());
        return r;
    }

    PathResult TransportController::runFloyd(int src, int dst) {
        ensureAllPairs();
        auto r = AlgoFacade::runFloyd(*floydCache, src, dst);
        auto list = stationsOnPath(r.path);
        std::ostringstream os2; os2 << "Ruta (" << r.algo << "): ";
        for (size_t i = 0; i < list.size(); ++i) { if (i) os2 << " -> "; os2 << list[i].id << " " << list[i].name; }
        logLine(os2.str()); // queda en reportes.txt
        std::ostringstream os; os << "[" << nowStamp() << "] Floyd " << src << "->" << dst
            << " reachable=" << (r.reachable ? "1" : "0")
            << " cost=" << r.cost << " path=";
        for (size_t i = 0; i < r.path.size(); ++i) { if (i) os << "-"; os << r.path[i]; }
        logLine(os.str());
        return r;
    }

    MSTResult TransportController::runPrim(int start) {
        auto r = AlgoFacade::runPrim(graph, start);
        std::ostringstream os; os << "[" << nowStamp() << "] Prim start=" << start
            << " edges=" << r.edges.size()
            << " total=" << r.totalWeight;
        logLine(os.str());
        return r;
    }

    MSTResult TransportController::runKruskal() {
        auto r = AlgoFacade::runKruskal(graph);
        std::ostringstream os; os << "[" << nowStamp() << "] Kruskal edges=" << r.edges.size()
            << " total=" << r.totalWeight;
        logLine(os.str());
        return r;
    }

    std::vector<Station> TransportController::stationsInOrder() const {
        return stations.inOrder();
    }

    std::vector<Station> TransportController::stationsOnPath(const std::vector<int>& path) const {
        std::vector<Station> out; out.reserve(path.size());
        for (int id : path) {
            if (auto s = stations.find(id)) out.push_back(*s);
            else out.push_back(Station{ id, "Unknown" });
        }
        return out;
    }

    void TransportController::invalidateAllPairs() {
        floydCache.reset();
    }

    void TransportController::ensureAllPairs() {
        if (!floydCache.has_value()) {
            floydCache = AlgoFacade::computeFloyd(graph);
        }
    }

    void TransportController::logLine(const std::string& line) const {
        ReportsFile::appendLine(reportesPath, line);
    }

} // namespace transport
