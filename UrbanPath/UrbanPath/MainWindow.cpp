#include "MainWindow.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QShowEvent>
#include <QSet>
#include <QStringList>
#include <QDir>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), dataLoaded(false)
{
    ui.setupUi(this);
    
    // Apply dark theme stylesheet
    applyDarkTheme();
    
    // Initialize graphics scene and view
    scene = new QGraphicsScene(this);
    ui.graphicsViewGraph->setScene(scene);
    
    // Initialize visualizer
    visualizer = new GraphVisualizer(scene, ui.graphicsViewGraph, &graph);
    
    // Install click event handler for map interaction
    visualizer->installClickEvent();
    
    // Configure callback for map clicks
    visualizer->setClickCallback([this](double x, double y) {
        // Generate new station ID (next available)
        int newId = graph.getStationCount() + 1;
        QString name = QString("Estacion %1").arg(newId);
        
        // Create station at clicked coordinates
        Station station(newId, name, x, y);
        bst.insert(station);
        graph.addStation(station);
        
        // Redraw the graph to show new station
        visualizer->drawGraph();
        
        // Update combo boxes
        updateComboBoxes();
        
        // Log the action
        QString log = QString("<span style='color:green'>[%1] Estacion agregada con clic: [%2] %3 (X:%4, Y:%5)</span>")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(newId)
            .arg(name)
            .arg(x, 0, 'f', 1)
            .arg(y, 0, 'f', 1);
        
        ui.txtGraphOutput->append(log);
        logBST(QString("Estacion %1 creada desde mapa").arg(newId), "green");
        
        statusBar()->showMessage(QString("Estacion %1 agregada en mapa (X:%2, Y:%3)")
            .arg(newId).arg(x, 0, 'f', 1).arg(y, 0, 'f', 1), 3000);
        
        qDebug() << "Estacion agregada con clic:" << newId << name << "en" << x << "," << y;
    });
    
    // Setup connections
    setupConnections();
    
    // Initial messages
    logBST("=== Sistema UrbanPath Iniciado ===", "#00BFFF");
    logBST("Listo para gestionar estaciones.", "white");
    logGraph("=== Red de Transporte Lista ===", "#00BFFF");
    logGraph("Listo para gestionar rutas.", "white");
    logGraph("Haz clic en el mapa para agregar estaciones!", "green");
    
    statusBar()->showMessage("Sistema UrbanPath listo para operar - Clic en mapa para agregar estaciones");
    
    // Try to load background image
    loadBackgroundImage();
    
    // Load initial data if files exist
    if (fileManager.fileExists("estaciones.txt"))
    {
        onActionCargarDatos();
    }
}

MainWindow::~MainWindow()
{
    delete visualizer;
    delete scene;
}

// Override showEvent to properly fit background after window is visible
void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    
    // After the window is shown and has its proper size, fit the background
    if (visualizer && scene)
    {
        visualizer->fitInView();
    }
}

// Apply dark theme stylesheet
void MainWindow::applyDarkTheme()
{
    QString darkTheme = R"(
        QMainWindow {
            background-color: #101820;
        }
        
        QWidget {
            background-color: #101820;
            color: #E0E0E0;
            font-family: 'Segoe UI';
            font-size: 10pt;
        }
        
        QPushButton {
            background-color: #1F2B3A;
            border-radius: 8px;
            padding: 10px;
            color: #E0E0E0;
            border: 1px solid #2A3C4F;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #2E3B4E;
            border: 1px solid #00CC88;
        }
        
        QPushButton:pressed {
            background-color: #0E1A2B;
        }
        
        QLineEdit, QComboBox, QDoubleSpinBox {
            background-color: #1C2835;
            border: 2px solid #2A3C4F;
            border-radius: 6px;
            padding: 8px;
            color: #E0E0E0;
        }
        
        QLineEdit:focus, QComboBox:focus, QDoubleSpinBox:focus {
            border: 2px solid #00CC88;
        }
        
        QTextEdit {
            background-color: #0E1A2B;
            border: 1px solid #2A3C4F;
            border-radius: 6px;
            color: #00CC88;
            padding: 8px;
        }
        
        QGroupBox {
            background-color: #1C2835;
            border: 2px solid #2A3C4F;
            border-radius: 10px;
            margin-top: 10px;
            padding-top: 20px;
            font-weight: bold;
            color: #00CC88;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 5px;
        }
        
        QTabWidget::pane {
            border: 2px solid #2A3C4F;
            background-color: #1C2835;
            border-radius: 8px;
        }
        
        QTabBar::tab {
            background-color: #1F2B3A;
            color: #E0E0E0;
            padding: 12px 24px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            margin-right: 4px;
        }
        
        QTabBar::tab:selected {
            background-color: #00CC88;
            color: #0E1A2B;
            font-weight: bold;
        }
        
        QTabBar::tab:hover {
            background-color: #2E3B4E;
        }
        
        QLabel {
            color: #E0E0E0;
        }
        
        QStatusBar {
            background-color: #0E1A2B;
            color: #00CC88;
            border-top: 2px solid #2A3C4F;
        }
        
        QMenuBar {
            background-color: #0E1A2B;
            color: #E0E0E0;
            border-bottom: 2px solid #2A3C4F;
        }
        
        QMenuBar::item {
            padding: 8px 12px;
        }
        
        QMenuBar::item:selected {
            background-color: #1F2B3A;
        }
        
        QMenu {
            background-color: #1F2B3A;
            color: #E0E0E0;
            border: 2px solid #2A3C4F;
        }
        
        QMenu::item:selected {
            background-color: #00CC88;
            color: #0E1A2B;
        }
        
        QGraphicsView {
            background-color: #0E1A2B;
            border: 2px solid #2A3C4F;
            border-radius: 8px;
        }
        
        QScrollArea {
            background-color: #101820;
            border: 2px solid #2A3C4F;
            border-radius: 8px;
        }
        
        QScrollBar:vertical {
            background-color: #1C2835;
            width: 14px;
            border-radius: 7px;
            margin: 0px;
        }
        
        QScrollBar::handle:vertical {
            background-color: #00CC88;
            border-radius: 7px;
            min-height: 30px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: #00FF99;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QScrollBar:horizontal {
            background-color: #1C2835;
            height: 14px;
            border-radius: 7px;
            margin: 0px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: #00CC88;
            border-radius: 7px;
            min-width: 30px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: #00FF99;
        }
        
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
        
        QSplitter::handle {
            background-color: #2A3C4F;
            width: 3px;
        }
        
        QSplitter::handle:hover {
            background-color: #00CC88;
        }
    )";
    
    this->setStyleSheet(darkTheme);
}

// Load background image
void MainWindow::loadBackgroundImage()
{
    QStringList possiblePaths = {
        "mapa.png",
        "../mapa.png",
        "../../mapa.png",
        "../../../mapa.png",
        "../../../../mapa.png",
        "data/mapa.png",
        "../data/mapa.png",
        "../../data/mapa.png"
    };
    
    for (const QString& path : possiblePaths)
    {
        if (QFile::exists(path))
        {
            visualizer->loadBackground(path);
            logGraph(QString("Mapa de fondo cargado correctamente desde: %1").arg(path), "green");
            return;
        }
    }
    
    logGraph("Advertencia: No se encontro imagen de fondo.", "orange");
}

// Update combo boxes with station IDs
void MainWindow::updateComboBoxes()
{
    ui.comboOrigin->clear();
    ui.comboDestination->clear();
    ui.comboClosureStation->clear();
    ui.comboClosureOrigin->clear();
    ui.comboClosureDest->clear();
    ui.comboAccidentOrigin->clear();
    ui.comboAccidentDest->clear();
    
    QList<Station> stations = graph.getAllStations();
    for (const Station& station : stations)
    {
        QString text = QString("%1 - %2").arg(station.getId()).arg(station.getName());
        ui.comboOrigin->addItem(text, station.getId());
        ui.comboDestination->addItem(text, station.getId());
        ui.comboClosureStation->addItem(text, station.getId());
        ui.comboClosureOrigin->addItem(text, station.getId());
        ui.comboClosureDest->addItem(text, station.getId());
        ui.comboAccidentOrigin->addItem(text, station.getId());
        ui.comboAccidentDest->addItem(text, station.getId());
    }
}

// Log message to BST console
void MainWindow::logBST(const QString& message, const QString& color)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString formattedMessage = QString("<span style='color:%1'>[%2] %3</span>")
        .arg(color)
        .arg(timestamp)
        .arg(message);
    
    ui.txtBSTOutput->append(formattedMessage);
}

// Log message to Graph console
void MainWindow::logGraph(const QString& message, const QString& color)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString formattedMessage = QString("<span style='color:%1'>[%2] %3</span>")
        .arg(color)
        .arg(timestamp)
        .arg(message);
                            
    ui.txtGraphOutput->append(formattedMessage);
}

// Show info message
void MainWindow::showInfoMessage(const QString& title, const QString& message)
{
    QMessageBox::information(this, title, message);
}

// Show error message
void MainWindow::showErrorMessage(const QString& title, const QString& message)
{
    QMessageBox::critical(this, title, message);
}

// Confirm action
bool MainWindow::confirmAction(const QString& title, const QString& message)
{
    return QMessageBox::question(this, title, message,
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
}

// ====== BST TAB SLOTS ======

// Slot: Add Station
void MainWindow::onAddStationClicked()
{
    int id = ui.txtStationID->text().toInt();
    QString name = ui.txtStationName->text();
    double x = ui.txtStationX->text().toDouble();
    double y = ui.txtStationY->text().toDouble();
    
    if (id <= 0 || name.isEmpty())
    {
        showErrorMessage("Error", "Por favor ingrese ID y nombre validos.");
        return;
    }

    if (visualizer && !visualizer->isPointWithinMap(x, y))
    {
        showErrorMessage("Error", "Las coordenadas deben estar dentro del mapa.");
        return;
    }
    
    Station station(id, name, x, y);
    bst.insert(station);
    graph.addStation(station);
    
    logBST(QString("Estacion agregada: [%1] %2 (X:%3, Y:%4)")
        .arg(id).arg(name).arg(x).arg(y), "green");
    
    // Mark data as loaded when adding stations manually
    dataLoaded = true;
    
    updateComboBoxes();
    onClearStationClicked();
    statusBar()->showMessage(QString("Estacion %1 agregada correctamente").arg(id), 3000);
}

// Slot: Remove Station
void MainWindow::onRemoveStationClicked()
{
    int id = ui.txtStationID->text().toInt();
    
    if (id <= 0)
    {
        showErrorMessage("Error", "Por favor ingrese un ID valido.");
        return;
    }
    
    if (bst.remove(id))
    {
        graph.removeStation(id);
        logBST(QString("Estacion %1 eliminada correctamente.").arg(id), "#FF6B6B");
        updateComboBoxes();
        onClearStationClicked();
        statusBar()->showMessage(QString("Estacion %1 eliminada").arg(id), 3000);
    }
    else
    {
        logBST(QString("Error: Estacion %1 no encontrada.").arg(id), "#FF6B6B");
        showErrorMessage("Error", QString("No se encontro la estacion con ID %1").arg(id));
    }
}

// Slot: Search Station
void MainWindow::onSearchStationClicked()
{
    int id = ui.txtStationID->text().toInt();
    
    if (id <= 0)
    {
        showErrorMessage("Error", "Por favor ingrese un ID valido.");
        return;
    }
    
    Station* station = bst.search(id);
    
    if (station)
    {
        ui.txtStationName->setText(station->getName());
        ui.txtStationX->setText(QString::number(station->getX()));
        ui.txtStationY->setText(QString::number(station->getY()));
        
        logBST(QString("Estacion encontrada: [%1] %2")
            .arg(station->getId()).arg(station->getName()), "blue");
        
        statusBar()->showMessage(QString("Estacion %1 encontrada").arg(id), 3000);
    }
    else
    {
        logBST(QString("Estacion con ID %1 no encontrada.").arg(id), "orange");
        showInfoMessage("Busqueda", QString("No se encontro la estacion con ID %1").arg(id));
    }
}

// Slot: Clear Station Fields
void MainWindow::onClearStationClicked()
{
    ui.txtStationID->clear();
    ui.txtStationName->clear();
    ui.txtStationX->clear();
    ui.txtStationY->clear();
    ui.txtStationID->setFocus();
}

// Slot: In-Order Traversal
void MainWindow::onInOrderClicked()
{
    QList<Station> stations = bst.inOrder();
    
    logBST("=== Recorrido In-Order (Ascendente) ===", "#00BFFF");
    for (int i = 0; i < stations.size(); i++)
    {
        const Station& s = stations[i];
        logBST(QString("%1. [%2] %3").arg(i+1).arg(s.getId()).arg(s.getName()), "white");
    }
    logBST(QString("Total: %1 estaciones").arg(stations.size()), "green");
}

// Slot: Pre-Order Traversal
void MainWindow::onPreOrderClicked()
{
    QList<Station> stations = bst.preOrder();
    
    logBST("=== Recorrido Pre-Order ===", "#00BFFF");
    for (int i = 0; i < stations.size(); i++)
    {
        const Station& s = stations[i];
        logBST(QString("%1. [%2] %3").arg(i+1).arg(s.getId()).arg(s.getName()), "white");
    }
    logBST(QString("Total: %1 estaciones").arg(stations.size()), "green");
}

// Slot: Post-Order Traversal
void MainWindow::onPostOrderClicked()
{
    QList<Station> stations = bst.postOrder();
    
    logBST("=== Recorrido Post-Order ===", "#00BFFF");
    for (int i = 0; i < stations.size(); i++)
    {
        const Station& s = stations[i];
        logBST(QString("%1. [%2] %3").arg(i+1).arg(s.getId()).arg(s.getName()), "white");
    }
    logBST(QString("Total: %1 estaciones").arg(stations.size()), "green");
}

// Slot: Export Traversals
void MainWindow::onExportTraversalsClicked()
{
    bool success = reportGenerator.generateTraversalReport("reporte_recorridos.txt", bst);
    
    if (success)
    {
        logBST("Recorridos exportados: reporte_recorridos.txt", "green");
        statusBar()->showMessage("Recorridos exportados correctamente", 3000);
        showInfoMessage("Exportacion Exitosa", "Los recorridos han sido exportados a reporte_recorridos.txt");
    }
    else
    {
        logBST("Error al exportar recorridos.", "#FF6B6B");
        showErrorMessage("Error", "No se pudieron exportar los recorridos.");
    }
}

// ====== GRAPH TAB SLOTS ======

// Slot: Add Route
void MainWindow::onAddRouteClicked()
{
    if (ui.comboOrigin->count() == 0 || ui.comboDestination->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    int dest = ui.comboDestination->currentData().toInt();
    double weight = ui.spinWeight->value();
    
    if (origin == dest)
    {
        showErrorMessage("Error", "Origen y destino deben ser diferentes.");
        return;
    }
    
    graph.addEdge(origin, dest, weight);
    
    logGraph(QString("Ruta agregada: %1 <-> %2 (distancia %3)")
        .arg(origin).arg(dest).arg(weight, 0, 'f', 1), "green");
    
    if (visualizer)
    {
        visualizer->drawGraph();
    }
    
    statusBar()->showMessage(QString("Ruta %1-%2 agregada").arg(origin).arg(dest), 3000);
}

// Slot: Remove Route
void MainWindow::onRemoveRouteClicked()
{
    if (ui.comboOrigin->count() == 0 || ui.comboDestination->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    int dest = ui.comboDestination->currentData().toInt();
    
    graph.removeEdge(origin, dest);
    
    logGraph(QString("Ruta eliminada: %1 <-> %2").arg(origin).arg(dest), "#FF6B6B");
    
    if (visualizer)
    {
        visualizer->drawGraph();
    }
    
    statusBar()->showMessage(QString("Ruta %1-%2 eliminada").arg(origin).arg(dest), 3000);
}

// Slot: Shortest Path (Dijkstra)
void MainWindow::onShortestPathClicked()
{
    if (ui.comboOrigin->count() == 0 || ui.comboDestination->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    // Check if graph is drawn
    if (!visualizer->isGraphDrawn())
    {
        showErrorMessage("Error", 
            "Debe dibujar el grafo primero.\n\n"
            "Haga clic en el boton 'Dibujar Grafo' antes de calcular rutas.");
        logGraph("Error: Intento de calcular ruta sin grafo dibujado.", "#FF6B6B");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    int dest = ui.comboDestination->currentData().toInt();
    
    if (origin == dest)
    {
        showInfoMessage("Informacion", "Origen y destino son la misma estacion.");
        return;
    }
    
    // Redraw graph to clear previous optimal route highlighting
    visualizer->drawGraph();
    
    logGraph(QString("Calculando ruta mas corta de %1 a %2...").arg(origin).arg(dest), "#00BFFF");
    
    // Get distances and predecessors
    QPair<QHash<int, double>, QHash<int, int>> result = graph.dijkstraWithPath(origin);
    QHash<int, double> distances = result.first;
    QHash<int, int> predecessors = result.second;
    
    if (distances[dest] >= std::numeric_limits<double>::infinity())
    {
        logGraph("No existe ruta entre las estaciones seleccionadas.", "#FF6B6B");
        showInfoMessage("Sin Ruta", "No hay conexion entre las estaciones seleccionadas.");
        return;
    }
    
    // Reconstruct the complete path
    QList<int> route;
    int current = dest;
    
    while (current != -1)
    {
        route.prepend(current);  // Add to front of list
        current = predecessors[current];
        
        // Break if we've reached the origin or if there's no predecessor
        if (current == origin)
        {
            route.prepend(origin);
            break;
        }
    }
    
    // Draw the optimal route
    visualizer->drawOptimalRoute(route);
    
    // Log the complete path
    QString pathStr = "Ruta: ";
    for (int i = 0; i < route.size(); i++)
    {
        pathStr += QString::number(route[i]);
        if (i < route.size() - 1)
        {
            pathStr += " -> ";
        }
    }
    
    logGraph(pathStr, "green");
    logGraph(QString("Distancia total: %1").arg(distances[dest], 0, 'f', 1), "green");
    
    reportGenerator.generateRouteReport("data/reportes/reporte_ruta_corta.txt", route, graph);
    statusBar()->showMessage(QString("Distancia: %1 | %2 estaciones").arg(distances[dest], 0, 'f', 1).arg(route.size()), 5000);
    
    // Show results in popup window
    QString resultMsg = QString("Ruta más corta encontrada (Dijkstra)\n\n%1\n\nDistancia total: %2\nEstaciones: %3")
        .arg(pathStr)
        .arg(distances[dest], 0, 'f', 1)
        .arg(route.size());
    showInfoMessage("Resultado - Dijkstra", resultMsg);
}

// Slot: Floyd-Warshall
void MainWindow::onFloydClicked()
{
    if (ui.comboOrigin->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    logGraph("Ejecutando algoritmo de Floyd-Warshall...", "#00BFFF");
    
    QHash<QPair<int, int>, double> allPaths = graph.floydWarshall();
    
    logGraph(QString("Floyd-Warshall completado. %1 pares de distancias calculadas.")
        .arg(allPaths.size()), "green");
    
    statusBar()->showMessage("Floyd-Warshall ejecutado correctamente", 3000);
    
    // Show results in popup window
    QString resultMsg = QString("Algoritmo de Floyd-Warshall completado exitosamente.\n\n"
                                "Pares de distancias calculadas: %1\n\n"
                                "Este algoritmo calcula las distancias más cortas\n"
                                "entre todos los pares de estaciones.")
        .arg(allPaths.size());
    showInfoMessage("Resultado - Floyd-Warshall", resultMsg);
}

// Slot: Prim MST
void MainWindow::onPrimMSTClicked()
{
    if (ui.comboOrigin->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    logGraph("Calculando MST con algoritmo de Prim...", "#00BFFF");
    
    QList<QPair<int, int>> mst = graph.primMST();
    
    if (mst.isEmpty())
    {
        logGraph("Error: No se pudo calcular el MST.", "#FF6B6B");
        return;
    }
    
    double totalWeight = 0.0;
    QString edgesStr = "Aristas del MST:\n\n";
    logGraph("Aristas del MST (Prim):", "#00BFFF");
    for (const auto& edge : mst)
    {
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        totalWeight += weight;
        logGraph(QString("  %1 <-> %2: %3")
            .arg(edge.first).arg(edge.second).arg(weight, 0, 'f', 1), "black");
        edgesStr += QString("  %1 ↔ %2: %3\n")
            .arg(edge.first).arg(edge.second).arg(weight, 0, 'f', 1);
    }
    
    logGraph(QString("Peso total del MST: %1").arg(totalWeight, 0, 'f', 1), "green");
    statusBar()->showMessage(QString("MST (Prim) - Peso: %1").arg(totalWeight, 0, 'f', 1), 5000);
    
    // Show results in popup window
    QString resultMsg = QString("Árbol de Expansión Mínima (Prim)\n\n%1\nPeso total: %2\nAristas: %3")
        .arg(edgesStr)
        .arg(totalWeight, 0, 'f', 1)
        .arg(mst.size());
    showInfoMessage("Resultado - MST (Prim)", resultMsg);
}

// Slot: Kruskal MST
void MainWindow::onKruskalMSTClicked()
{
    if (ui.comboOrigin->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    logGraph("Calculando MST con algoritmo de Kruskal...", "#00BFFF");
    
    QList<QPair<int, int>> mst = graph.kruskalMST();
    
    if (mst.isEmpty())
    {
        logGraph("Error: No se pudo calcular el MST.", "#FF6B6B");
        return;
    }
    
    double totalWeight = 0.0;
    QString edgesStr = "Aristas del MST:\n\n";
    logGraph("Aristas del MST (Kruskal):", "#00BFFF");
    for (const auto& edge : mst)
    {
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        totalWeight += weight;
        logGraph(QString("  %1 <-> %2: %3")
            .arg(edge.first).arg(edge.second).arg(weight, 0, 'f', 1), "black");
        edgesStr += QString("  %1 ↔ %2: %3\n")
            .arg(edge.first).arg(edge.second).arg(weight, 0, 'f', 1);
    }
    
    logGraph(QString("Peso total del MST: %1").arg(totalWeight, 0, 'f', 1), "green");
    
    reportGenerator.generateMSTReport("reporte_mst.txt", graph);
    statusBar()->showMessage(QString("MST (Kruskal) - Peso: %1").arg(totalWeight, 0, 'f', 1), 5000);
    
    // Show results in popup window
    QString resultMsg = QString("Árbol de Expansión Mínima (Kruskal)\n\n%1\nPeso total: %2\nAristas: %3")
        .arg(edgesStr)
        .arg(totalWeight, 0, 'f', 1)
        .arg(mst.size());
    showInfoMessage("Resultado - MST (Kruskal)", resultMsg);
}

// Slot: BFS
void MainWindow::onBFSClicked()
{
    if (ui.comboOrigin->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    
    logGraph(QString("Ejecutando BFS desde estacion %1...").arg(origin), "#00BFFF");
    
    QList<int> bfsResult = graph.bfs(origin);
    
    QString result = "BFS: ";
    for (int i = 0; i < bfsResult.size(); i++)
    {
        result += QString::number(bfsResult[i]);
        if (i < bfsResult.size() - 1) result += " -> ";
    }
    
    logGraph(result, "green");
    statusBar()->showMessage(QString("BFS: %1 estaciones alcanzadas").arg(bfsResult.size()), 3000);
    
    // Show results in popup window
    QString resultMsg = QString("Recorrido en Anchura (BFS)\n\nOrigen: Estación %1\n\n%2\n\nEstaciones alcanzadas: %3")
        .arg(origin)
        .arg(result)
        .arg(bfsResult.size());
    showInfoMessage("Resultado - BFS", resultMsg);
}

// Slot: DFS
void MainWindow::onDFSClicked()
{
    if (ui.comboOrigin->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    
    logGraph(QString("Ejecutando DFS desde estacion %1...").arg(origin), "#00BFFF");
    
    QList<int> dfsResult = graph.dfs(origin);
    
    QString result = "DFS: ";
    for (int i = 0; i < dfsResult.size(); i++)
    {
        result += QString::number(dfsResult[i]);
        if (i < dfsResult.size() - 1) result += " -> ";
    }
    
    logGraph(result, "green");
    statusBar()->showMessage(QString("DFS: %1 estaciones alcanzadas").arg(dfsResult.size()), 3000);
    
    // Show results in popup window
    QString resultMsg = QString("Recorrido en Profundidad (DFS)\n\nOrigen: Estación %1\n\n%2\n\nEstaciones alcanzadas: %3")
        .arg(origin)
        .arg(result)
        .arg(dfsResult.size());
    showInfoMessage("Resultado - DFS", resultMsg);
}

// Slot: Draw Graph
void MainWindow::onDrawGraphClicked()
{
    logGraph("Dibujando grafo...", "#00BFFF");
    visualizer->drawGraph();
    logGraph("Grafo dibujado correctamente.", "green");
    statusBar()->showMessage("Grafo visualizado", 2000);
}

// Slot: Clear Graph
void MainWindow::onClearGraphClicked()
{
    logGraph("Limpiando vista del grafo...", "#00BFFF");
    visualizer->clearScene();
    logGraph("Vista limpiada.", "green");
    statusBar()->showMessage("Vista limpiada", 2000);
}

// ====== CLOSURE MANAGEMENT ======

// Slot: Close Station Manually
void MainWindow::onCloseStationClicked()
{
    if (ui.comboClosureStation->count() == 0)
    {
        showErrorMessage("Error", "No hay estaciones disponibles para cerrar.");
        return;
    }
    
    int stationId = ui.comboClosureStation->currentData().toInt();
    QString stationName = ui.comboClosureStation->currentText();
    
    if (graph.isStationClosed(stationId))
    {
        showInfoMessage("Información", 
            QString("La estación %1 ya está cerrada.").arg(stationName));
        return;
    }
    
    if (!confirmAction("Cerrar Estación", 
        QString("¿Está seguro de que desea CERRAR la estación?\n\n%1\n\n"
                "La estación se mostrará en GRIS y los algoritmos la ignorarán.")
        .arg(stationName)))
    {
        return;
    }
    
    graph.closeStation(stationId);
    
    logGraph(QString("🚧 Estación cerrada manualmente: %1").arg(stationName), "orange");
    
    // Redraw to show closure
    if (visualizer)
    {
        visualizer->drawGraph();
    }
    
    statusBar()->showMessage(QString("Estación %1 cerrada").arg(stationId), 3000);
    showInfoMessage("Estación Cerrada", 
        QString("La estación %1 ha sido cerrada.\n\n"
                "• Aparecerá en GRIS en el mapa\n"
                "• Los algoritmos la ignorarán\n\n"
                "Use 'Quitar Todos los Cierres' para reactivarla.")
        .arg(stationName));
}

// Slot: Close Route Manually
void MainWindow::onCloseRouteClicked()
{
    if (ui.comboClosureOrigin->count() == 0 || ui.comboClosureDest->count() == 0)
    {
        showErrorMessage("Error", "Deben de haber estaciones y rutas primero.");
        return;
    }
    
    int origin = ui.comboClosureOrigin->currentData().toInt();
    int dest = ui.comboClosureDest->currentData().toInt();
    
    if (origin == dest)
    {
        showErrorMessage("Error", "Origen y destino deben ser diferentes.");
        return;
    }
    
    if (!graph.hasEdge(origin, dest))
    {
        showErrorMessage("Error", 
            QString("No existe una ruta entre las estaciones %1 y %2.").arg(origin).arg(dest));
        return;
    }
    
    if (graph.isRouteClosed(origin, dest))
    {
        showInfoMessage("Información", 
            QString("La ruta entre %1 y %2 ya está cerrada.").arg(origin).arg(dest));
        return;
    }
    
    if (!confirmAction("Cerrar Ruta", 
        QString("¿Está seguro de que desea CERRAR la ruta?\n\n"
                "Estación %1 ↔ Estación %2\n\n"
                "La ruta se mostrará en ROJO y los algoritmos la ignorarán.")
        .arg(origin).arg(dest)))
    {
        return;
    }
    
    graph.closeRoute(origin, dest);
    
    logGraph(QString("🚧 Ruta cerrada manualmente: %1 ↔ %2").arg(origin).arg(dest), "orange");
    
    // Redraw to show closure
    if (visualizer)
    {
        visualizer->drawGraph();
    }
    
    statusBar()->showMessage(QString("Ruta %1-%2 cerrada").arg(origin).arg(dest), 3000);
    showInfoMessage("Ruta Cerrada", 
        QString("La ruta entre estaciones %1 y %2 ha sido cerrada.\n\n"
                "• Aparecerá en ROJO en el mapa\n"
                "• Los algoritmos la ignorarán\n\n"
                "Use 'Quitar Todos los Cierres' para reactivarla.")
        .arg(origin).arg(dest));
}

// Slot: Apply Closures from File
void MainWindow::onApplyClosuresClicked()
{
    logGraph("Aplicando cierres y accidentes desde archivo...", "#00BFFF");
    statusBar()->showMessage("Aplicando cierres y accidentes...");
    
    // Check if there are stations to apply closures to
    if (graph.getStationCount() == 0)
    {
        showErrorMessage("Error", "No hay estaciones. Cargue o agregue estaciones primero.");
        return;
    }
    
    // Load closures
    bool closuresLoaded = fileManager.loadClosures("data/datos/cierres.txt", graph);
    
    // Load accidents
    bool accidentsLoaded = fileManager.loadAccidents(graph, "data/datos/accidentes.txt");
    
    if (closuresLoaded || accidentsLoaded)
    {
        QSet<int> closedStations = graph.getClosedStations();
        QList<QPair<int, int>> closedRoutes = graph.getClosedRoutes();
        int affectedRoutes = graph.getAffectedRoutes().size() / 2; // Divide by 2 for undirected
        
        if (closuresLoaded)
        {
            logGraph(QString("🚧 Cierres aplicados: %1 estaciones, %2 rutas bloqueadas")
                .arg(closedStations.size())
                .arg(closedRoutes.size()), "orange");
        }
        
        if (accidentsLoaded)
        {
            logGraph(QString("🚧 Accidentes aplicados: %1 rutas con peso incrementado")
                .arg(affectedRoutes), "#FFD700");
        }
        
        // Redraw graph to show closures and accidents visually
        if (visualizer)
        {
            visualizer->drawGraph();
        }
        
        QString statusMsg = QString("Aplicados: %1 estaciones cerradas, %2 rutas cerradas, %3 accidentes")
            .arg(closedStations.size()).arg(closedRoutes.size()).arg(affectedRoutes);
        statusBar()->showMessage(statusMsg, 5000);
        
        QString infoMsg = "Se han aplicado correctamente:\n\n";
        if (closuresLoaded)
        {
            infoMsg += QString("• Estaciones cerradas: %1\n").arg(closedStations.size());
            infoMsg += QString("• Rutas cerradas: %2\n\n").arg(closedRoutes.size());
        }
        if (accidentsLoaded)
        {
            infoMsg += QString("• Rutas con accidentes: %1\n\n").arg(affectedRoutes);
        }
        infoMsg += "Las estaciones cerradas aparecen en GRIS.\n";
        infoMsg += "Las rutas cerradas aparecen en ROJO.\n";
        infoMsg += "Los accidentes incrementan el peso de las rutas.";
        
        showInfoMessage("Cierres y Accidentes Aplicados", infoMsg);
    }
    else
    {
        QString error = "No se pudieron cargar los archivos.\n\n";
        error += "Verifique que existan en data/datos/:\n";
        error += "• cierres.txt (para estaciones/rutas bloqueadas)\n";
        error += "• accidentes.txt (para incrementos de peso)";
        logGraph("Error: No se encontraron archivos de cierres o accidentes en data/datos/", "#FF6B6B");
        showErrorMessage("Error al Aplicar", error);
        statusBar()->showMessage("Error: archivos no encontrados");
    }
}

// Slot: Clear Closures Only
void MainWindow::onClearClosuresClicked()
{
    // Check if there are stations in the graph
    if (graph.getStationCount() == 0)
    {
        showErrorMessage("Error", "No hay estaciones cargadas.");
        return;
    }
    
    if (!confirmAction("Limpiar Cierres", 
        "¿Está seguro de que desea eliminar todos los cierres?\n\n"
        "Esto restaurará:\n"
        "• Todas las estaciones bloqueadas\n"
        "• Todas las rutas cerradas\n\n"
        "NOTA: Los accidentes NO se eliminarán."))
    {
        return;
    }
    
    graph.clearClosures();
    
    logGraph("✅ Todos los cierres han sido eliminados.", "green");
    
    // Redraw graph to show all routes/stations active again
    if (visualizer)
    {
        visualizer->drawGraph();
    }
    
    statusBar()->showMessage("Cierres eliminados - Estaciones y rutas reactivadas", 3000);
    showInfoMessage("Cierres Eliminados", 
        "Todos los cierres han sido eliminados.\n\n"
        "• Estaciones bloqueadas reactivadas\n"
        "• Rutas cerradas reabiertas\n\n"
        "Los accidentes permanecen activos.");
}

// ====== MENU ACTIONS ======

// Menu Action: Cargar Datos
void MainWindow::onActionCargarDatos()
{
    logBST("Cargando datos del sistema...", "#00BFFF");
    logGraph("Cargando datos del sistema...", "#00BFFF");
    statusBar()->showMessage("Cargando datos...");
    
    // Clear existing data
    graph.clear();
    bst.clear();
    
    // Load stations
    bool stationsLoaded = fileManager.loadStations("data/datos/estaciones.txt", bst, graph);
    
    if (!stationsLoaded)
    {
        QString error = fileManager.getLastError();
        if (error.isEmpty())
        {
            error = "No se pudo cargar el archivo data/datos/estaciones.txt. Verifique que exista.";
        }
        logBST(QString("Error: %1").arg(error), "#FF6B6B");
        logGraph(QString("Error: %1").arg(error), "#FF6B6B");
        showErrorMessage("Error de Carga", error);
        statusBar()->showMessage("Error al cargar datos");
        return;
    }
    
    // Load routes
    bool routesLoaded = fileManager.loadRoutes("data/datos/rutas.txt", graph);
    
    dataLoaded = true;
    updateComboBoxes();
    
    logBST(QString("Datos cargados: %1 estaciones.").arg(bst.count()), "green");
    if (graph.getStationCount() == 0)
    {
        logGraph("No se encontraron estaciones en el archivo.", "orange");
    }
    else
    {
        logGraph(QString("Datos cargados: %1 estaciones.").arg(graph.getStationCount()), "green");
    }
    
    // Count unique routes loaded (avoid duplicates in undirected graph)
    QSet<QPair<int, int>> countedEdges;
    int routeCount = 0;
    QList<Station> loadedStations = graph.getAllStations();
    for (const Station& station : loadedStations)
    {
        int fromId = station.getId();
        QList<QPair<int, double>> neighbors = graph.getNeighbors(fromId);
        for (const auto& neighbor : neighbors)
        {
            int toId = neighbor.first;
            QPair<int, int> edgeKey(std::min(fromId, toId), std::max(fromId, toId));
            if (!countedEdges.contains(edgeKey))
            {
                countedEdges.insert(edgeKey);
                routeCount++;
            }
        }
    }
    
    if (!routesLoaded)
    {
        QString error = fileManager.getLastError();
        if (!error.isEmpty())
        {
            logGraph(QString("Advertencia: %1").arg(error), "orange");
        }
    }
    
    if (routeCount == 0)
    {
        logGraph("No se encontraron rutas en el archivo.", "orange");
    }
    else
    {
        logGraph(QString("Rutas cargadas: %1").arg(routeCount), "green");
    }
    
    // Load closures automatically if file exists
    bool closuresLoaded = false;
    if (fileManager.fileExists("data/datos/cierres.txt"))
    {
        closuresLoaded = fileManager.loadClosures("data/datos/cierres.txt", graph);
        
        if (closuresLoaded)
        {
            QSet<int> closedStations = graph.getClosedStations();
            QList<QPair<int, int>> closedRoutes = graph.getClosedRoutes();
            int totalClosures = closedStations.size() + closedRoutes.size();
            
            if (totalClosures > 0)
            {
                logGraph(QString("🚧 Cierres cargados: %1 estaciones, %2 rutas cerradas")
                    .arg(closedStations.size())
                    .arg(closedRoutes.size()), "orange");
            }
        }
    }
    
    // Load accidents automatically if file exists
    bool accidentsLoaded = false;
    if (fileManager.fileExists("data/datos/accidentes.txt"))
    {
        accidentsLoaded = fileManager.loadAccidents(graph, "data/datos/accidentes.txt");
        
        if (accidentsLoaded)
        {
            int totalAccidents = graph.getAffectedRoutes().size() / 2;
            
            if (totalAccidents > 0)
            {
                logGraph(QString("🚧 Accidentes cargados: %1 rutas con peso incrementado")
                    .arg(totalAccidents), "#FFD700");
            }
        }
    }
    
    logGraph("Presiona 'Dibujar Grafo' para visualizar la red.", "orange");
    
    statusBar()->showMessage("Datos cargados correctamente", 3000);
    
    QString closureInfo = "";
    QSet<int> closedStations = graph.getClosedStations();
    QList<QPair<int, int>> closedRoutes = graph.getClosedRoutes();
    int totalClosures = closedStations.size() + closedRoutes.size();
    int totalAccidents = graph.getAffectedRoutes().size() / 2;
    
    if (totalClosures > 0)
    {
        closureInfo = QString("\n\n🚧 Cierres activos:\n• Estaciones cerradas: %1\n• Rutas cerradas: %2")
            .arg(closedStations.size())
            .arg(closedRoutes.size());
    }
    
    if (totalAccidents > 0)
    {
        closureInfo += QString("\n\n🚧 Accidentes activos:\n• Rutas afectadas: %1")
            .arg(totalAccidents);
    }
    
    showInfoMessage("Carga Exitosa", 
        QString("Datos cargados correctamente:\n\nEstaciones: %1\nRutas: %2%3")
        .arg(graph.getStationCount())
        .arg(routeCount)
        .arg(closureInfo));
}

// Menu Action: Guardar Datos
void MainWindow::onActionGuardarDatos()
{
    // Check if there's actual data to save (stations or routes)
    if (bst.isEmpty() && graph.getStationCount() == 0)
    {
        showInfoMessage("Informacion", "No hay datos para guardar.");
        return;
    }
    
    // Create data directories if they don't exist
    QDir dir;
    if (!dir.exists("data"))
    {
        dir.mkdir("data");
    }
    if (!dir.exists("data/datos"))
    {
        dir.mkpath("data/datos");
    }
    
    bool stationsSaved = fileManager.saveStations("data/datos/estaciones.txt", bst);
    QString stationError = stationsSaved ? QString() : fileManager.getLastError();
    
    bool routesSaved = fileManager.saveRoutes("data/datos/rutas.txt", graph);
    QString routeError = routesSaved ? QString() : fileManager.getLastError();
    
    bool closuresSaved = fileManager.saveClosures("data/datos/cierres.txt", graph);
    QString closureError = closuresSaved ? QString() : fileManager.getLastError();
    
    bool accidentsSaved = fileManager.saveAccidents("data/datos/accidentes.txt", graph);
    QString accidentError = accidentsSaved ? QString() : fileManager.getLastError();
    
    if (stationsSaved && routesSaved && closuresSaved && accidentsSaved)
    {
        logBST("Estaciones guardadas en data/datos/estaciones.txt", "green");
        logGraph("Rutas guardadas en data/datos/rutas.txt", "green");
        
        QSet<int> closedStations = graph.getClosedStations();
        QList<QPair<int, int>> closedRoutes = graph.getClosedRoutes();
        int totalClosures = closedStations.size() + closedRoutes.size();
        int totalAccidents = graph.getAffectedRoutes().size() / 2;
        
        if (totalClosures > 0)
        {
            logGraph(QString("Cierres guardados en data/datos/cierres.txt (%1 cierres)").arg(totalClosures), "green");
        }
        else
        {
            logGraph("Sin cierres activos (archivo data/datos/cierres.txt limpiado)", "green");
        }
        
        if (totalAccidents > 0)
        {
            logGraph(QString("Accidentes guardados en data/datos/accidentes.txt (%1 accidentes)").arg(totalAccidents), "green");
        }
        else
        {
            logGraph("Sin accidentes activos (archivo data/datos/accidentes.txt limpiado)", "green");
        }
        
        statusBar()->showMessage("Datos guardados correctamente", 3000);
        showInfoMessage("Guardado Exitoso", 
            QString("Los datos se han guardado correctamente en data/datos/:\n\n"
                    "• Estaciones: estaciones.txt\n"
                    "• Rutas: rutas.txt\n"
                    "• Cierres: cierres.txt (%1)\n"
                    "• Accidentes: accidentes.txt (%2)").arg(totalClosures).arg(totalAccidents));
    }
    else
    {
        QStringList errors;
        if (!stationError.isEmpty())
        {
            errors << stationError;
        }
        if (!routeError.isEmpty())
        {
            errors << routeError;
        }
        if (!closureError.isEmpty())
        {
            errors << closureError;
        }
        if (!accidentError.isEmpty())
        {
            errors << accidentError;
        }
        QString errorMessage = errors.isEmpty() ? "No se pudieron guardar todos los archivos." : errors.join("\n");
        showErrorMessage("Error de Guardado", errorMessage);
    }
}

// Menu Action: Salir
void MainWindow::onActionSalir()
{
    if (confirmAction("Salir", "Esta seguro de que desea salir de UrbanPath?"))
    {
        close();
    }
}

// Menu Action: Generar Reportes
void MainWindow::onActionGenerarReportes()
{
    // Check if there's data to generate reports from
    if (bst.isEmpty() && graph.getStationCount() == 0)
    {
        showInfoMessage("Informacion", "No hay datos para generar reportes.");
        return;
    }
    
    // Create reports directory if it doesn't exist
    QDir dir;
    if (!dir.exists("data"))
    {
        dir.mkdir("data");
    }
    if (!dir.exists("data/reportes"))
    {
        dir.mkpath("data/reportes");
    }
    
    logGraph("Generando reportes del sistema...", "#00BFFF");
    statusBar()->showMessage("Generando reportes...");
    
    bool statsReport = reportGenerator.generateSystemStats("data/reportes/reporte_estadisticas.txt", graph, bst);
    bool mstReport = reportGenerator.generateMSTReport("data/reportes/reporte_mst.txt", graph);
    bool connectivityReport = reportGenerator.generateConnectivityReport("data/reportes/reporte_conectividad.txt", graph);
    bool traversalReport = reportGenerator.generateTraversalReport("data/reportes/reporte_recorridos.txt", bst);
    bool accidentReport = reportGenerator.generateAccidentReport("data/reportes/reporte_accidentes.txt", graph);
    
    int successCount = (statsReport ? 1 : 0) + (mstReport ? 1 : 0) + 
                       (connectivityReport ? 1 : 0) + (traversalReport ? 1 : 0) +
                       (accidentReport ? 1 : 0);
    
    logGraph(QString("Reportes generados exitosamente en data/reportes/ (%1/5).").arg(successCount), "green");
    statusBar()->showMessage("Reportes generados en data/reportes/", 3000);
    
    showInfoMessage("Reportes Generados",
        QString("Se han generado %1 reportes del sistema en data/reportes/:\n\n"
                "• reporte_estadisticas.txt\n"
                "• reporte_mst.txt\n"
                "• reporte_conectividad.txt\n"
                "• reporte_recorridos.txt\n"
                "• reporte_accidentes.txt").arg(successCount));
}

// Menu Action: Ver Ultimo Reporte
void MainWindow::onActionVerUltimoReporte()
{
    if (fileManager.fileExists("data/reportes/reporte_estadisticas.txt"))
    {
        logGraph("Abriendo data/reportes/reporte_estadisticas.txt...", "#00BFFF");
        QDesktopServices::openUrl(QUrl::fromLocalFile("data/reportes/reporte_estadisticas.txt"));
    }
    else
    {
        showInfoMessage("Informacion", 
            "No se encontro ningun reporte.\n"
            "Genere reportes primero.");
    }
}

// Menu Action: Acerca De
void MainWindow::onActionAcercaDe()
{
    QMessageBox::about(this, "Acerca de UrbanPath",
        "<h2>UrbanPath - Sistema de Transporte Urbano</h2>"
        "<p><b>Version:</b> 1.0</p>"
        "<p><b>Proyecto:</b> Estructura de Datos - Proyecto #2</p>"
        "<hr>"
        "<p><b>Descripcion:</b></p>"
        "<p>Sistema de gestion de transporte urbano que utiliza "
        "arboles binarios de busqueda y grafos ponderados.</p>"
        "<hr>"
        "<p><b>Algoritmos Implementados:</b></p>"
        "<ul>"
        "<li>Arbol Binario de Busqueda (BST)</li>"
        "<li>BFS y DFS</li>"
        "<li>Dijkstra (Ruta mas corta)</li>"
        "<li>Floyd-Warshall</li>"
        "<li>Prim y Kruskal (MST)</li>"
        "<li>Union-Find (Disjoint Set)</li>"
        "</ul>"
        "<hr>"
        "<p><b>Tecnologias:</b> C++14, Qt 6.9.2</p>"
        "<p><b>Desarrollado con:</b> Visual Studio 2022</p>");
}

// Setup signal/slot connections
void MainWindow::setupConnections()
{
    // BST Tab connections
    connect(ui.btnAddStation, &QPushButton::clicked, this, &MainWindow::onAddStationClicked);
    connect(ui.btnRemoveStation, &QPushButton::clicked, this, &MainWindow::onRemoveStationClicked);
    connect(ui.btnSearchStation, &QPushButton::clicked, this, &MainWindow::onSearchStationClicked);
    connect(ui.btnClearStation, &QPushButton::clicked, this, &MainWindow::onClearStationClicked);
    connect(ui.btnInOrder, &QPushButton::clicked, this, &MainWindow::onInOrderClicked);
    connect(ui.btnPreOrder, &QPushButton::clicked, this, &MainWindow::onPreOrderClicked);
    connect(ui.btnPostOrder, &QPushButton::clicked, this, &MainWindow::onPostOrderClicked);
    connect(ui.btnExportTraversals, &QPushButton::clicked, this, &MainWindow::onExportTraversalsClicked);
    
    // Graph Tab connections
    connect(ui.btnAddRoute, &QPushButton::clicked, this, &MainWindow::onAddRouteClicked);
    connect(ui.btnRemoveRoute, &QPushButton::clicked, this, &MainWindow::onRemoveRouteClicked);
    connect(ui.btnShortestPath, &QPushButton::clicked, this, &MainWindow::onShortestPathClicked);
    connect(ui.btnFloyd, &QPushButton::clicked, this, &MainWindow::onFloydClicked);
    connect(ui.btnPrimMST, &QPushButton::clicked, this, &MainWindow::onPrimMSTClicked);
    connect(ui.btnKruskalMST, &QPushButton::clicked, this, &MainWindow::onKruskalMSTClicked);
    connect(ui.btnBFS, &QPushButton::clicked, this, &MainWindow::onBFSClicked);
    connect(ui.btnDFS, &QPushButton::clicked, this, &MainWindow::onDFSClicked);
    connect(ui.btnDrawGraph, &QPushButton::clicked, this, &MainWindow::onDrawGraphClicked);
    connect(ui.btnClearGraph, &QPushButton::clicked, this, &MainWindow::onClearGraphClicked);
    
    // Closure management connections
    connect(ui.btnCloseStation, &QPushButton::clicked, this, &MainWindow::onCloseStationClicked);
    connect(ui.btnCloseRoute, &QPushButton::clicked, this, &MainWindow::onCloseRouteClicked);
    connect(ui.btnApplyClosures, &QPushButton::clicked, this, &MainWindow::onApplyClosuresClicked);
    connect(ui.btnClearClosures, &QPushButton::clicked, this, &MainWindow::onClearClosuresClicked);
    
    // Accident management connections
    connect(ui.btnAddAccident, &QPushButton::clicked, this, &MainWindow::onAddAccidentClicked);
    connect(ui.btnClearAccidents, &QPushButton::clicked, this, &MainWindow::onClearAccidentsClicked);
    
    connect(ui.tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        // Auto-fit map when Graph tab becomes visible
        if (index == ui.tabWidget->indexOf(ui.tabGraph) && visualizer)
        {
            visualizer->fitInView();
        }
    });
    
    // Menu action connections
    connect(ui.actionCargarDatos, &QAction::triggered, this, &MainWindow::onActionCargarDatos);
    connect(ui.actionGuardarDatos, &QAction::triggered, this, &MainWindow::onActionGuardarDatos);
    connect(ui.actionSalir, &QAction::triggered, this, &MainWindow::onActionSalir);
    connect(ui.actionGenerarReportes, &QAction::triggered, this, &MainWindow::onActionGenerarReportes);
    connect(ui.actionVerUltimoReporte, &QAction::triggered, this, &MainWindow::onActionVerUltimoReporte);
    connect(ui.actionAcercaDe, &QAction::triggered, this, &MainWindow::onActionAcercaDe);
}

// ==================== Accident Management Slots ====================

// Add accident manually (from selected route)
void MainWindow::onAddAccidentClicked()
{
    // Check if there are stations and routes
    if (graph.getStationCount() == 0)
    {
        showErrorMessage("Error", "No hay estaciones. Agregue estaciones primero.");
        return;
    }
    
    if (graph.isEmpty())
    {
        showErrorMessage("Error", "No hay rutas. Agregue rutas primero.");
        return;
    }
    
    // Get selected stations
    if (ui.comboAccidentOrigin->count() == 0 || ui.comboAccidentDest->count() == 0)
    {
        showErrorMessage("Error", "No hay estaciones disponibles.");
        return;
    }
    
    int originId = ui.comboAccidentOrigin->currentData().toInt();
    int destId = ui.comboAccidentDest->currentData().toInt();
    double increment = ui.spinAccidentPercent->value();
    
    // Validate different stations
    if (originId == destId)
    {
        showErrorMessage("Error", "Debe seleccionar dos estaciones diferentes.");
        return;
    }
    
    // Validate route exists
    if (!graph.hasEdge(originId, destId))
    {
        showErrorMessage("Error", 
            QString("No existe una ruta directa entre las estaciones %1 y %2.").arg(originId).arg(destId));
        return;
    }
    
    // Get station names for confirmation
    Station* originStation = graph.getStation(originId);
    Station* destStation = graph.getStation(destId);
    QString originName = originStation ? originStation->getName() : QString::number(originId);
    QString destName = destStation ? destStation->getName() : QString::number(destId);
    
    double currentWeight = graph.getEdgeWeight(originId, destId);
    double newWeight = currentWeight + (currentWeight * (increment / 100.0));
    
    // Confirm action
    QString confirmMsg = QString(
        "¿Desea agregar un accidente a esta ruta?\n\n"
        "Origen: %1 (%2)\n"
        "Destino: %3 (%4)\n"
        "Incremento: +%5%\n\n"
        "Peso actual: %6\n"
        "Peso nuevo: %7"
    ).arg(originId).arg(originName).arg(destId).arg(destName)
     .arg(increment, 0, 'f', 0)
     .arg(currentWeight, 0, 'f', 1)
     .arg(newWeight, 0, 'f', 1);
    
    if (!confirmAction("Agregar Accidente", confirmMsg))
    {
        return;
    }
    
    // Apply accident
    bool success = graph.applyAccident(originId, destId, increment);
    
    if (success)
    {
        logGraph(QString("[OK] Accidente agregado manualmente: Ruta %1 <-> %2 (+%3%)")
            .arg(originId).arg(destId).arg(increment, 0, 'f', 0), "#00CC88");
        
        logGraph(QString("[INFO] Peso actualizado: %1 -> %2")
            .arg(currentWeight, 0, 'f', 1).arg(newWeight, 0, 'f', 1), "#FFD700");
        
        // Redraw graph if visible
        if (visualizer->isGraphDrawn())
        {
            visualizer->drawGraph();
            logGraph("[INFO] Grafo redibujado con peso actualizado", "#00CC88");
        }
        
        showInfoMessage("Éxito", 
            QString("Accidente agregado exitosamente.\n\n"
                    "La ruta %1 <-> %2 ahora tiene un peso de %3 (+%4%)")
            .arg(originId).arg(destId).arg(newWeight, 0, 'f', 1).arg(increment, 0, 'f', 0));
    }
    else
    {
        logGraph("[ERROR] No se pudo agregar el accidente (posiblemente ya existe)", "#FF6B6B");
        showErrorMessage("Error", 
            "No se pudo agregar el accidente.\n\n"
            "Es posible que esta ruta ya tenga un accidente aplicado.\n"
            "Use 'Limpiar Accidentes' primero si desea modificarlo.");
    }
}

// Clear all accidents and restore original weights
void MainWindow::onClearAccidentsClicked()
{
    // Check if there are stations in the graph
    if (graph.getStationCount() == 0)
    {
        showErrorMessage("Error", "No hay estaciones cargadas.");
        return;
    }
    
    // Check if there are active accidents
    if (graph.getAffectedRoutes().isEmpty())
    {
        showInfoMessage("Información", "No hay accidentes activos para limpiar.");
        logGraph("[INFO] No hay accidentes activos", "#FFD700");
        return;
    }
    
    // Confirm action
    if (!confirmAction("Limpiar Accidentes", 
        "Esto restaurara los pesos originales de todas las rutas afectadas.\n\n"
        "¿Desea continuar?"))
    {
        return;
    }
    
    int affectedCount = graph.getAffectedRoutes().size() / 2; // Divide by 2 for undirected
    
    // Clear accidents
    graph.clearAccidents();
    
    logGraph(QString("[INFO] Accidentes limpiados. %1 rutas restauradas.").arg(affectedCount), "#00CC88");
    
    // Redraw graph to show changes (if already drawn)
    if (visualizer->isGraphDrawn())
    {
        visualizer->drawGraph();
        logGraph("[INFO] Grafo redibujado con pesos originales", "#00CC88");
    }
    
    showInfoMessage("Éxito", QString("Accidentes limpiados exitosamente.\n\n"
        "%1 rutas restauradas a sus pesos originales.").arg(affectedCount));
}

