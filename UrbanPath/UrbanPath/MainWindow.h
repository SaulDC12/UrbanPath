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
    // Button slots
    void onLoadDataClicked();
    void onSaveDataClicked();
    void onDrawGraphClicked();
    void onShortestPathClicked();
    void onMSTClicked();
    void onTraversalsClicked();
    void onGenerateReportsClicked();
    void onClearGraphClicked();
    
    // Menu action slots
    void onActionCargarDatos();
    void onActionGuardarDatos();
    void onActionSalir();
    void onActionDibujarGrafo();
    void onActionMostrarRutaOptima();
    void onActionLimpiarVista();
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
    void updateStatistics();
    void logMessage(const QString& message, const QString& color = "black");
    void showInfoMessage(const QString& title, const QString& message);
    void showErrorMessage(const QString& title, const QString& message);
    bool confirmAction(const QString& title, const QString& message);
    
    // Data loaded flag
    bool dataLoaded;
};

