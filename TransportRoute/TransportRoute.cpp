#include "TransportRoute.h"
#include <QGraphicsView>
#include <QMessageBox>
#include <QCloseEvent>

TransportRoute::TransportRoute(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
 
    // escena y view
    scene_ = new GraphScene(this);
    ui.graphicsView->setScene(scene_);

    // conexiones
    connect(ui.btnLoadAll, &QPushButton::clicked, this, &TransportRoute::onLoadAll);
    connect(ui.btnReloadClosures, &QPushButton::clicked, this, &TransportRoute::onReloadClosures);
    connect(ui.btnSaveStations, &QPushButton::clicked, this, &TransportRoute::onSaveStations);
    connect(ui.btnSaveRoutes, &QPushButton::clicked, this, &TransportRoute::onSaveRoutes);
    connect(ui.btnRun, &QPushButton::clicked, this, &TransportRoute::onRun);

    // valores por defecto
    ui.cmbAlgo->setCurrentIndex(2); // Dijkstra por defecto
    onLoadAll(); // auto-carga al iniciar
    controller_.exportTraversals(); // opcional: regenerar recorridos al iniciar
}

TransportRoute::~TransportRoute() {}

void TransportRoute::log(const QString& s) {
    ui.txtLog->appendPlainText(s);
    ui.statusBar->showMessage(s, 3000);
}

void TransportRoute::refreshScene() {
    scene_->setGraph(&controller_.getGraph());
    ui.graphicsView->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
}

void TransportRoute::onLoadAll() {
    if (controller_.loadAll()) {
        log("LoadAll ok");
        refreshScene();
    }
    else {
        QMessageBox::warning(this, "Error", "No se pudieron cargar rutas/estaciones.");
    }
}

void TransportRoute::onReloadClosures() {
    controller_.reloadClosures();
    refreshScene();
    log("Closures reapplied");
}

void TransportRoute::onSaveStations() {
    if (controller_.saveStations()) log("Estaciones guardadas");
    else QMessageBox::warning(this, "Error", "No se pudo guardar estaciones.txt");
}

void TransportRoute::onSaveRoutes() {
    if (controller_.saveRoutes()) log("Rutas guardadas");
    else QMessageBox::warning(this, "Error", "No se pudo guardar rutas.txt");
}

void TransportRoute::closeEvent(QCloseEvent* e) {
    controller_.saveStations();
    controller_.saveRoutes();
    log("Auto-saved on close");
    QMainWindow::closeEvent(e);
}

void TransportRoute::onRun() {
    const QString algo = ui.cmbAlgo->currentText();
    const int src = ui.spnSrc->value();
    const int dst = ui.spnDst->value();

    // redibujar base
    scene_->drawGraph();

    if (algo == "BFS") {
        auto r = controller_.runBFS(src);
        log(QString("BFS desde %1: %2 nodos").arg(src).arg((int)r.order.size()));
        // opcional: pintar orden conectando consecutivos
        if (r.order.size() >= 2) {
            std::vector<int> pseudoPath = r.order;
            scene_->highlightPath(pseudoPath);
        }
    }
    else if (algo == "DFS") {
        auto r = controller_.runDFS(src);
        log(QString("DFS desde %1: %2 nodos").arg(src).arg((int)r.order.size()));
        if (r.order.size() >= 2) {
            std::vector<int> pseudoPath = r.order;
            scene_->highlightPath(pseudoPath);
        }
    }
    else if (algo == "Dijkstra") {
        auto r = controller_.runDijkstra(src, dst);
        log(QString("Dijkstra %1->%2 reachable=%3 cost=%4")
            .arg(src).arg(dst).arg(r.reachable ? "1" : "0").arg(r.cost));
        if (r.reachable) scene_->highlightPath(r.path);
    }
    else if (algo == "Floyd") {
        auto r = controller_.runFloyd(src, dst);
        log(QString("Floyd %1->%2 reachable=%3 cost=%4")
            .arg(src).arg(dst).arg(r.reachable ? "1" : "0").arg(r.cost));
        if (r.reachable) scene_->highlightPath(r.path);
    }
    else if (algo == "Prim") {
        auto r = controller_.runPrim(src);
        log(QString("Prim desde %1: edges=%2 total=%3")
            .arg(src).arg((int)r.edges.size()).arg(r.totalWeight));
        scene_->highlightMST(r.edges);
    }
    else if (algo == "Kruskal") {
        auto r = controller_.runKruskal();
        log(QString("Kruskal: edges=%1 total=%2")
            .arg((int)r.edges.size()).arg(r.totalWeight));
        scene_->highlightMST(r.edges);
    }

    ui.graphicsView->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
}
