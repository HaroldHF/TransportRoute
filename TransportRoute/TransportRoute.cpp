#include "TransportRoute.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QToolBar>

TransportRoute::TransportRoute(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setupUI();
    setupConnections();

    // Cargar datos inicial
    onLoadData();
}

TransportRoute::~TransportRoute() {
}

void TransportRoute::setupUI() {
    setWindowTitle("Sistema de Transporte - La Mancha 🚌");
    resize(1400, 900);

    // Estilo moderno oscuro
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1e1e2e;
        }
        QToolBar {
            background-color: #2a2a3e;
            border-bottom: 2px solid #00d4ff;
            spacing: 8px;
            padding: 8px;
        }
        QPushButton {
            background-color: #3a3a4e;
            color: #e0e0e0;
            border: 2px solid #4a4a6e;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            font-size: 11px;
        }
        QPushButton:hover {
            background-color: #4a4a6e;
            border-color: #00d4ff;
        }
        QPushButton:pressed {
            background-color: #2a2a3e;
        }
        QLabel {
            color: #e0e0e0;
            font-size: 11px;
        }
        QStatusBar {
            background-color: #2a2a3e;
            color: #00d4ff;
            border-top: 1px solid #4a4a6e;
        }
    )");

    // Toolbar
    QToolBar* toolbar = addToolBar("Herramientas");
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(24, 24));

    btnLoad = new QPushButton("📂 Cargar", this);
    btnLoad->setToolTip("Cargar estaciones y rutas desde archivos");
    toolbar->addWidget(btnLoad);

    btnSave = new QPushButton("💾 Guardar", this);
    btnSave->setToolTip("Guardar cambios en archivos");
    toolbar->addWidget(btnSave);

    toolbar->addSeparator();

    btnAddStation = new QPushButton("➕ Nueva Estación", this);
    btnAddStation->setToolTip("Agregar nueva estación al mapa");
    toolbar->addWidget(btnAddStation);

    toolbar->addSeparator();

    lblStatus = new QLabel("Listo", this);
    toolbar->addWidget(lblStatus);

    // Canvas principal
    mapCanvas = new MapCanvas(this);
    mapCanvas->setController(&controller);

    // Layout central
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mapCanvas);

    setCentralWidget(centralWidget);

    // Status bar
    statusBar()->showMessage("Sistema iniciado correctamente");
}

void TransportRoute::setupConnections() {
    connect(btnLoad, &QPushButton::clicked, this, &TransportRoute::onLoadData);
    connect(btnSave, &QPushButton::clicked, this, &TransportRoute::onSaveData);
    connect(btnAddStation, &QPushButton::clicked, this, &TransportRoute::onAddStation);

    connect(mapCanvas, &MapCanvas::stationClicked, this, &TransportRoute::onStationClicked);
    connect(mapCanvas, &MapCanvas::stationMoved, this, &TransportRoute::onStationMoved);
}

void TransportRoute::onLoadData() {
    lblStatus->setText("Cargando datos...");

    bool success = controller.loadAll();

    if (success) {
        mapCanvas->refreshGraph();
        updateStatusBar();
        statusBar()->showMessage("Datos cargados correctamente", 3000);
        lblStatus->setText("✅ Datos cargados");
    }
    else {
        QMessageBox::warning(this, "Error",
            "No se pudieron cargar algunos archivos.\n"
            "Verifica que existan: estaciones.txt, rutas.txt");
        lblStatus->setText("⚠️ Error al cargar");
    }
}

void TransportRoute::onSaveData() {
    lblStatus->setText("Guardando...");

    bool ok1 = controller.saveStations();
    bool ok2 = controller.saveRoutes();

    if (ok1 && ok2) {
        statusBar()->showMessage("Datos guardados correctamente", 3000);
        lblStatus->setText("✅ Guardado");
        QMessageBox::information(this, "Éxito", "Datos guardados correctamente");
    }
    else {
        QMessageBox::warning(this, "Error", "Error al guardar algunos archivos");
        lblStatus->setText("⚠️ Error al guardar");
    }
}

void TransportRoute::onAddStation() {
    bool ok;
    int id = QInputDialog::getInt(this, "Nueva Estación",
        "ID de la estación:",
        1, 1, 9999, 1, &ok);
    if (!ok) return;

    QString name = QInputDialog::getText(this, "Nueva Estación",
        "Nombre de la estación:",
        QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    // Verificar que no exista
    auto stations = controller.stationsInOrder();
    for (const auto& s : stations) {
        if (s.id == id) {
            QMessageBox::warning(this, "Error",
                "Ya existe una estación con ID " + QString::number(id));
            return;
        }
    }

    // Agregar en posición central del canvas
    double x = mapCanvas->sceneRect().center().x();
    double y = mapCanvas->sceneRect().center().y();

    // Actualizar estructura Station con coordenadas
    transport::Station newStation(id, name.toStdString(), x, y);
    controller.stations.insert(newStation);
    controller.graph.addVertex(id);

    mapCanvas->refreshGraph();
    updateStatusBar();

    statusBar()->showMessage("Estación agregada: " + name, 3000);
    lblStatus->setText("✅ Estación agregada");
}

void TransportRoute::onStationClicked(int id) {
    auto station = controller.stations.find(id);
    if (station) {
        statusBar()->showMessage(
            QString("Estación seleccionada: %1 - %2")
            .arg(id)
            .arg(QString::fromStdString(station->name)),
            3000
        );
    }
}

void TransportRoute::onStationMoved(int id, double x, double y) {
    // Actualizar coordenadas en el BST
    auto station = controller.stations.find(id);
    if (station) {
        // Eliminar y re-insertar con nuevas coordenadas
        std::string name = station->name;
        controller.removeStation(id);

        transport::Station updated(id, name, x, y);
        controller.stations.insert(updated);

        statusBar()->showMessage(
            QString("Estación %1 movida a (%.0f, %.0f)")
            .arg(id).arg(x).arg(y),
            2000
        );
    }
}

void TransportRoute::updateStatusBar() {
    auto stations = controller.stationsInOrder();
    int numRoutes = 0;
    for (const auto& [u, edges] : controller.graph.data()) {
        numRoutes += edges.size();
    }
    numRoutes /= 2; // Grafo no dirigido

    lblStatus->setText(QString("📍 %1 estaciones | 🛣️ %2 rutas")
        .arg(stations.size())
        .arg(numRoutes));
}