#include "TransportRoute.h"
#include <QGraphicsView>
#include <QMessageBox>
#include <QCloseEvent>
#include <QInputDialog>
#include <QMenu>
#include <QCursor>


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
    connect(scene_, &GraphScene::contextOnEmpty, this, [this](const QPointF& sp) {
        if (mode_ == EditMode::AddStation) {
            bool ok = false;
            int id = QInputDialog::getInt(this, "Nueva estacion", "ID:", 0, 0, 1000000, 1, &ok);
            if (!ok) return;
            QString name = QInputDialog::getText(this, "Nueva estacion", "Nombre:");
            controller_.addStation(id, name.toStdString());
            // reflejar posicion elegida
            auto pos = scene_->positions();
            auto pos2 = pos; pos2[id] = sp;
            scene_->setPositions(pos2);
            scene_->drawGraph();
            statusBar()->showMessage("Estacion agregada", 2000);
        }
        });

    connect(scene_, &GraphScene::nodeContext, this, [this](NodeItem* node, const QPointF& sp) {
        QMenu m;
        auto* actRename = m.addAction("Renombrar…");
        auto* actDelete = m.addAction("Eliminar estacion");
        auto* actConnect = m.addAction("Conectar…");

        QAction* a = m.exec(QCursor::pos());
        if (a == actRename) {
            QString name = QInputDialog::getText(this, "Renombrar", "Nuevo nombre:");
            if (!name.isEmpty()) {
                controller_.removeStation(node->id()); // simple: quitar y re-insertar con nombre
                controller_.addStation(node->id(), name.toStdString());
                scene_->drawGraph();
            }
        }
        else if (a == actDelete) {
            controller_.removeStation(node->id());
            // quitar aristas en archivo: opcional (puedes regenerar rutas.txt luego)
            scene_->drawGraph();
        }
        else if (a == actConnect) {
            mode_ = EditMode::Connect;
            pendingConnectFirst_ = node->id();
            statusBar()->showMessage("Selecciona la segunda estacion para conectar…");
        }
        });

    connect(scene_, &GraphScene::requestEdgeMenu, this, [this](EdgeItem* ed, const QPointF&) {
        QMenu m;
        auto* actWeight = m.addAction("Editar peso…");
        auto* actToggle = m.addAction(ed->closed() ? "Abrir via" : "Cerrar via");
        auto* actDelete = m.addAction("Eliminar via");
        QAction* a = m.exec(QCursor::pos());

        if (a == actWeight) {
            bool ok = false;
            double nw = QInputDialog::getDouble(this, "Peso", "Minutos:", ed->w(), 0, 1e9, 2, &ok);
            if (ok) {
                // simplificado: elimina y crea de nuevo con nuevo peso
                controller_.saveRoutes(); // por ahora guardamos luego de re-crear (si implementas addRoute/removeRoute mejor)
                // para el demo: reescribe el archivo rutas.txt a mano si gustas
                statusBar()->showMessage("Peso actualizado (recarga rutas para ver cambios)", 2500);
            }
        }
        else if (a == actToggle) {
            controller_.reloadClosures(); // si usas cierres.txt; para toggle dinamico deberias exponer setClosed en controller
            scene_->drawGraph();
        }
        else if (a == actDelete) {
            // necesitaras un removeEdge(u,v) en el controller/graph para borrarla realmente
            QMessageBox::information(this, "Aviso", "Implementa removeEdge(u,v) en el Core para borrar permanentemente.");
        }
        });

    connect(scene_, &GraphScene::nodeMoved, this, [this](int, QPointF) {
        // auto-guardar posiciones opcional
        });
    connect(scene_, &GraphScene::nodeContext, this, [this](NodeItem* node, const QPointF&) {
        // Si estamos conectando y ya tenemos la primera seleccionada:
        if (mode_ == EditMode::Connect && pendingConnectFirst_ != -1 && pendingConnectFirst_ != node->id()) {
            bool ok = false;
            double w = QInputDialog::getDouble(this, "Peso", "Minutos:", 5.0, 0, 1e9, 2, &ok);
            if (ok) {
                // requiere controller_.addRoute(u,v,w)
                if (controller_.addRoute(pendingConnectFirst_, node->id(), w)) {
                    scene_->drawGraph();
                    statusBar()->showMessage("Conectadas", 2000);
                }
                else {
                    QMessageBox::warning(this, "Error", "No se pudo agregar la via.");
                }
            }
            mode_ = EditMode::Move;
            pendingConnectFirst_ = -1;
            return; // no abrir menu contextual normal
        }

        // --- menu contextual normal de la estacion ---
        QMenu m;
        auto* actRename = m.addAction("Renombrar…");
        auto* actDelete = m.addAction("Eliminar estacion");
        auto* actConnect = m.addAction("Conectar…");
        QAction* a = m.exec(QCursor::pos());

        if (a == actRename) {
            QString name = QInputDialog::getText(this, "Renombrar", "Nuevo nombre:");
            if (!name.isEmpty()) {
                controller_.renameStation(node->id(), name.toStdString()); // mejor que remove+add
                scene_->drawGraph();
            }
        }
        else if (a == actDelete) {
            // opcional: controller_.removeVertexAndEdges(node->id()) si lo implementas
            controller_.removeStation(node->id());
            scene_->drawGraph();
        }
        else if (a == actConnect) {
            mode_ = EditMode::Connect;
            pendingConnectFirst_ = node->id();
            statusBar()->showMessage("Selecciona la segunda estacion para conectar…");
        }
        });

    // valores por defecto
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
