#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include "Station.h"
#include "BST.h"
#include "Graph.h"
#include "Result.h"
#include "AlgoFacade.h"

namespace transport {

    class TransportController {
    public:
        // archivos por defecto (puedes cambiarlos via setters)
        std::string estacionesPath = "estaciones.txt";
        std::string rutasPath = "rutas.txt";
        std::string cierresPath = "cierres.txt";
        std::string reportesPath = "reportes.txt";
        std::string recorridosPath = "recorridos_rutas.txt";
        std::string accidentesPath = "accidentes.txt";
        // estado en memoria
        BST<Station> stations;
        Graph graph;

        // cache de Floyd (se invalida si cambia el grafo)
        std::optional<FloydWarshall::AllPairs> floydCache;

        TransportController();

        // carga/guardado
        bool loadAll();                 // estaciones + rutas + cierres
        bool reloadClosures();          // solo re-aplicar cierres (si cambian)
        bool saveStations() const;      // opcional
        bool saveRoutes() const;        // opcional
        bool exportTraversals() const;
        bool reloadAccidents();
        bool addStation(int id, const std::string& name);
        bool removeStation(int id);
        bool addRoute(int u, int v, double w) { graph.addEdge(u, v, w, false); invalidateAllPairs(); return true; }
        bool exportGraphSummary();
        bool removeEdge(int u, int v);
        bool setClosed(int u, int v, bool closed);
        bool renameStation(int id, const std::string& newName);

        // consultas
        VisitResult   runBFS(int start);
        VisitResult   runDFS(int start);
        PathResult    runDijkstra(int src, int dst);
        PathResult    runFloyd(int src, int dst);       // usa cache
        MSTResult     runPrim(int start);
        MSTResult     runKruskal();

        // utilidades
        std::vector<Station> stationsOnPath(const std::vector<int>& path) const;
        std::vector<Station> stationsInOrder() const;   // para listas/reportes
        const Graph& getGraph() const { return graph; }

    private:
        void invalidateAllPairs();       // invalida cache de Floyd
        void ensureAllPairs();           // recalcula si falta
        void logLine(const std::string& line) const; // agrega a reportes.txt
    };

} // namespace transport
