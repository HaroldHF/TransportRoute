#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_TransportRoute.h"

#include "GraphScene.h"

// incluye el controlador del Core (ajusta la ruta al instalar TransportCore en include paths)
#include "TransportController.h"

class TransportRoute : public QMainWindow
{
    Q_OBJECT
public:
    TransportRoute(QWidget* parent = nullptr);
    ~TransportRoute();

private slots:
    void onLoadAll();
    void onReloadClosures();
    void onSaveStations();
    void onSaveRoutes();
    void onRun();

private:
    enum class EditMode { Move, AddStation, Connect };
    EditMode mode_ = EditMode::Move;
    int pendingConnectFirst_ = -1;
    QString positionsPath_ = "positions.txt";
    QString mapCfgPath_ = "map.cfg";
    Ui::TransportRouteClass ui;
    GraphScene* scene_ = nullptr;
    transport::TransportController controller_;
    void closeEvent(QCloseEvent* e) override;
    void log(const QString& s);
    void refreshScene();
};
