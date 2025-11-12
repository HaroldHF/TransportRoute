#pragma once
#include <QtWidgets/QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include "ui_TransportRoute.h"
#include "TransportController.h"
#include "MapCanvas.h"

class TransportRoute : public QMainWindow {
    Q_OBJECT

public:
    TransportRoute(QWidget* parent = nullptr);
    ~TransportRoute();

private slots:
    void onLoadData();
    void onSaveData();
    void onAddStation();
    void onStationClicked(int id);
    void onStationMoved(int id, double x, double y);

private:
    void setupUI();
    void setupConnections();
    void updateStatusBar();

    Ui::TransportRouteClass ui;

    // Core
    transport::TransportController controller;

    // Widgets
    MapCanvas* mapCanvas;
    QPushButton* btnLoad;
    QPushButton* btnSave;
    QPushButton* btnAddStation;
    QLabel* lblStatus;
};