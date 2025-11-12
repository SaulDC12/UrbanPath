#pragma once

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include "ui_MainWindow.h"
#include "Graph.h"
#include "StationBST.h"
#include "FileManager.h"
#include "ReportGenerator.h"
#include "GraphVisualizer.h"

using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // BST Tab slots
    void onAddStationClicked();
    void onRemoveStationClicked();
    void onSearchStationClicked();
    void onClearStationClicked();
    void onInOrderClicked();
    void onPreOrderClicked();
    void onPostOrderClicked();
    void onExportTraversalsClicked();
    
    // Graph Tab slots
    void onAddRouteClicked();
    void onRemoveRouteClicked();
    void onShortestPathClicked();
    void onFloydClicked();
    void onPrimMSTClicked();
    void onKruskalMSTClicked();
    void onBFSClicked();
    void onDFSClicked();
    void onDrawGraphClicked();
    void onClearGraphClicked();
    
    // Menu action slots
    void onActionCargarDatos();
    void onActionGuardarDatos();
    void onActionSalir();
    void onActionGenerarReportes();
    void onActionVerUltimoReporte();
    void onActionAcercaDe();

private:
    Ui::MainWindowClass ui;
    
    // Core components
    Graph graph;
    StationBST bst;
    FileManager fileManager;
    ReportGenerator reportGenerator;
    GraphVisualizer* visualizer;
    QGraphicsScene* scene;
    
    // Helper methods
    void setupConnections();
    void loadBackgroundImage();
    void updateComboBoxes();
    void logBST(const QString& message, const QString& color = "black");
    void logGraph(const QString& message, const QString& color = "black");
    void showInfoMessage(const QString& title, const QString& message);
    void showErrorMessage(const QString& title, const QString& message);
    bool confirmAction(const QString& title, const QString& message);
    void applyDarkTheme();
    
    // Data loaded flag
    bool dataLoaded;
};

