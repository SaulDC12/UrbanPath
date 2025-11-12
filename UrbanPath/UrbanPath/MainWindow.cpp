#include "MainWindow.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>

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
    logBST("=== Sistema UrbanPath Iniciado ===", "blue");
    logBST("Listo para gestionar estaciones.");
    logGraph("=== Red de Transporte Lista ===", "blue");
    logGraph("Listo para gestionar rutas.");
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
    )";
    
    this->setStyleSheet(darkTheme);
}

// Load background image
void MainWindow::loadBackgroundImage()
{
    if (fileManager.fileExists("data/mapa.png"))
    {
        visualizer->loadBackground("data/mapa.png");
        logGraph("Mapa de fondo cargado correctamente.", "green");
    }
    else if (fileManager.fileExists("mapa.png"))
    {
        visualizer->loadBackground("mapa.png");
        logGraph("Mapa de fondo cargado correctamente.", "green");
    }
    else
    {
        logGraph("Advertencia: No se encontro imagen de fondo.", "orange");
    }
}

// Update combo boxes with station IDs
void MainWindow::updateComboBoxes()
{
    ui.comboOrigin->clear();
    ui.comboDestination->clear();
    
    QList<Station> stations = graph.getAllStations();
    for (const Station& station : stations)
    {
        QString text = QString("%1 - %2").arg(station.getId()).arg(station.getName());
        ui.comboOrigin->addItem(text, station.getId());
        ui.comboDestination->addItem(text, station.getId());
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
    
    Station station(id, name, x, y);
    bst.insert(station);
    graph.addStation(station);
    
    logBST(QString("Estacion agregada: [%1] %2 (X:%3, Y:%4)")
        .arg(id).arg(name).arg(x).arg(y), "green");
    
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
        logBST(QString("Estacion %1 eliminada correctamente.").arg(id), "red");
        updateComboBoxes();
        onClearStationClicked();
        statusBar()->showMessage(QString("Estacion %1 eliminada").arg(id), 3000);
    }
    else
    {
        logBST(QString("Error: Estacion %1 no encontrada.").arg(id), "red");
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
    
    logBST("=== Recorrido In-Order (Ascendente) ===", "blue");
    for (int i = 0; i < stations.size(); i++)
    {
        const Station& s = stations[i];
        logBST(QString("%1. [%2] %3").arg(i+1).arg(s.getId()).arg(s.getName()), "black");
    }
    logBST(QString("Total: %1 estaciones").arg(stations.size()), "green");
}

// Slot: Pre-Order Traversal
void MainWindow::onPreOrderClicked()
{
    QList<Station> stations = bst.preOrder();
    
    logBST("=== Recorrido Pre-Order ===", "blue");
    for (int i = 0; i < stations.size(); i++)
    {
        const Station& s = stations[i];
        logBST(QString("%1. [%2] %3").arg(i+1).arg(s.getId()).arg(s.getName()), "black");
    }
    logBST(QString("Total: %1 estaciones").arg(stations.size()), "green");
}

// Slot: Post-Order Traversal
void MainWindow::onPostOrderClicked()
{
    QList<Station> stations = bst.postOrder();
    
    logBST("=== Recorrido Post-Order ===", "blue");
    for (int i = 0; i < stations.size(); i++)
    {
        const Station& s = stations[i];
        logBST(QString("%1. [%2] %3").arg(i+1).arg(s.getId()).arg(s.getName()), "black");
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
        logBST("Error al exportar recorridos.", "red");
        showErrorMessage("Error", "No se pudieron exportar los recorridos.");
    }
}

// ====== GRAPH TAB SLOTS ======

// Slot: Add Route
void MainWindow::onAddRouteClicked()
{
    if (ui.comboOrigin->count() == 0 || ui.comboDestination->count() == 0)
    {
        showErrorMessage("Error", "Debe cargar estaciones primero.");
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
    
    statusBar()->showMessage(QString("Ruta %1-%2 agregada").arg(origin).arg(dest), 3000);
}

// Slot: Remove Route
void MainWindow::onRemoveRouteClicked()
{
    if (ui.comboOrigin->count() == 0 || ui.comboDestination->count() == 0)
    {
        showErrorMessage("Error", "Debe cargar estaciones primero.");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    int dest = ui.comboDestination->currentData().toInt();
    
    graph.removeEdge(origin, dest);
    
    logGraph(QString("Ruta eliminada: %1 <-> %2").arg(origin).arg(dest), "red");
    statusBar()->showMessage(QString("Ruta %1-%2 eliminada").arg(origin).arg(dest), 3000);
}

// Slot: Shortest Path (Dijkstra)
void MainWindow::onShortestPathClicked()
{
    if (ui.comboOrigin->count() == 0 || ui.comboDestination->count() == 0)
    {
        showErrorMessage("Error", "Debe cargar estaciones primero.");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    int dest = ui.comboDestination->currentData().toInt();
    
    logGraph(QString("Calculando ruta mas corta de %1 a %2...").arg(origin).arg(dest), "blue");
    
    QHash<int, double> distances = graph.dijkstra(origin);
    
    if (distances[dest] >= std::numeric_limits<double>::infinity())
    {
        logGraph("No existe ruta entre las estaciones seleccionadas.", "red");
        showInfoMessage("Sin Ruta", "No hay conexion entre las estaciones seleccionadas.");
        return;
    }
    
    QList<int> route;
    route.append(origin);
    route.append(dest);
    
    visualizer->drawOptimalRoute(route);
    
    logGraph(QString("Ruta mas corta: %1 -> %2 (distancia: %3)")
        .arg(origin).arg(dest).arg(distances[dest], 0, 'f', 1), "green");
    
    reportGenerator.generateRouteReport("reporte_ruta_corta.txt", route, graph);
    statusBar()->showMessage(QString("Distancia: %1").arg(distances[dest], 0, 'f', 1), 5000);
}

// Slot: Floyd-Warshall
void MainWindow::onFloydClicked()
{
    logGraph("Ejecutando algoritmo de Floyd-Warshall...", "blue");
    
    QHash<QPair<int, int>, double> allPaths = graph.floydWarshall();
    
    logGraph(QString("Floyd-Warshall completado. %1 pares de distancias calculadas.")
        .arg(allPaths.size()), "green");
    
    statusBar()->showMessage("Floyd-Warshall ejecutado correctamente", 3000);
}

// Slot: Prim MST
void MainWindow::onPrimMSTClicked()
{
    logGraph("Calculando MST con algoritmo de Prim...", "blue");
    
    QList<QPair<int, int>> mst = graph.primMST();
    
    if (mst.isEmpty())
    {
        logGraph("Error: No se pudo calcular el MST.", "red");
        return;
    }
    
    double totalWeight = 0.0;
    logGraph("Aristas del MST (Prim):", "blue");
    for (const auto& edge : mst)
    {
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        totalWeight += weight;
        logGraph(QString("  %1 <-> %2: %3")
            .arg(edge.first).arg(edge.second).arg(weight, 0, 'f', 1), "black");
    }
    
    logGraph(QString("Peso total del MST: %1").arg(totalWeight, 0, 'f', 1), "green");
    statusBar()->showMessage(QString("MST (Prim) - Peso: %1").arg(totalWeight, 0, 'f', 1), 5000);
}

// Slot: Kruskal MST
void MainWindow::onKruskalMSTClicked()
{
    logGraph("Calculando MST con algoritmo de Kruskal...", "blue");
    
    QList<QPair<int, int>> mst = graph.kruskalMST();
    
    if (mst.isEmpty())
    {
        logGraph("Error: No se pudo calcular el MST.", "red");
        return;
    }
    
    double totalWeight = 0.0;
    logGraph("Aristas del MST (Kruskal):", "blue");
    for (const auto& edge : mst)
    {
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        totalWeight += weight;
        logGraph(QString("  %1 <-> %2: %3")
            .arg(edge.first).arg(edge.second).arg(weight, 0, 'f', 1), "black");
    }
    
    logGraph(QString("Peso total del MST: %1").arg(totalWeight, 0, 'f', 1), "green");
    
    reportGenerator.generateMSTReport("reporte_mst.txt", graph);
    statusBar()->showMessage(QString("MST (Kruskal) - Peso: %1").arg(totalWeight, 0, 'f', 1), 5000);
}

// Slot: BFS
void MainWindow::onBFSClicked()
{
    if (ui.comboOrigin->count() == 0)
    {
        showErrorMessage("Error", "Debe cargar estaciones primero.");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    
    logGraph(QString("Ejecutando BFS desde estacion %1...").arg(origin), "blue");
    
    QList<int> bfsResult = graph.bfs(origin);
    
    QString result = "BFS: ";
    for (int i = 0; i < bfsResult.size(); i++)
    {
        result += QString::number(bfsResult[i]);
        if (i < bfsResult.size() - 1) result += " -> ";
    }
    
    logGraph(result, "green");
    statusBar()->showMessage(QString("BFS: %1 estaciones alcanzadas").arg(bfsResult.size()), 3000);
}

// Slot: DFS
void MainWindow::onDFSClicked()
{
    if (ui.comboOrigin->count() == 0)
    {
        showErrorMessage("Error", "Debe cargar estaciones primero.");
        return;
    }
    
    int origin = ui.comboOrigin->currentData().toInt();
    
    logGraph(QString("Ejecutando DFS desde estacion %1...").arg(origin), "blue");
    
    QList<int> dfsResult = graph.dfs(origin);
    
    QString result = "DFS: ";
    for (int i = 0; i < dfsResult.size(); i++)
    {
        result += QString::number(dfsResult[i]);
        if (i < dfsResult.size() - 1) result += " -> ";
    }
    
    logGraph(result, "green");
    statusBar()->showMessage(QString("DFS: %1 estaciones alcanzadas").arg(dfsResult.size()), 3000);
}

// Slot: Draw Graph
void MainWindow::onDrawGraphClicked()
{
    logGraph("Dibujando grafo...", "blue");
    visualizer->drawGraph();
    logGraph("Grafo dibujado correctamente.", "green");
    statusBar()->showMessage("Grafo visualizado", 2000);
}

// Slot: Clear Graph
void MainWindow::onClearGraphClicked()
{
    logGraph("Limpiando vista del grafo...", "blue");
    visualizer->clearScene();
    logGraph("Vista limpiada.", "green");
    statusBar()->showMessage("Vista limpiada", 2000);
}

// ====== MENU ACTIONS ======

// Menu Action: Cargar Datos
void MainWindow::onActionCargarDatos()
{
    logBST("Cargando datos del sistema...", "blue");
    logGraph("Cargando datos del sistema...", "blue");
    statusBar()->showMessage("Cargando datos...");
    
    // Clear existing data
    graph.clear();
    bst.clear();
    
    // Load stations
    bool stationsLoaded = fileManager.loadStations("estaciones.txt", bst, graph);
    
    if (!stationsLoaded)
    {
        logBST("Error: No se pudieron cargar las estaciones.", "red");
        logGraph("Error: No se pudieron cargar las estaciones.", "red");
        showErrorMessage("Error de Carga", 
            "No se pudo cargar el archivo estaciones.txt.\n"
            "Verifique que el archivo exista.");
        statusBar()->showMessage("Error al cargar datos");
        return;
    }
    
    // Load routes
    bool routesLoaded = fileManager.loadRoutes("rutas.txt", graph);
    
    if (!routesLoaded)
    {
        logGraph("Advertencia: No se pudieron cargar las rutas.", "orange");
    }
    
    dataLoaded = true;
    updateComboBoxes();
    
    logBST(QString("Datos cargados: %1 estaciones.").arg(bst.count()), "green");
    logGraph(QString("Datos cargados: %1 estaciones.").arg(graph.getStationCount()), "green");
    
    statusBar()->showMessage("Datos cargados correctamente", 3000);
    
    showInfoMessage("Carga Exitosa", 
        QString("Datos cargados correctamente:\n\nEstaciones: %1")
        .arg(graph.getStationCount()));
}

// Menu Action: Guardar Datos
void MainWindow::onActionGuardarDatos()
{
    if (!dataLoaded)
    {
        showInfoMessage("Informacion", "No hay datos para guardar.");
        return;
    }
    
    bool stationsSaved = fileManager.saveStations("estaciones_guardadas.txt", bst);
    bool routesSaved = fileManager.saveRoutes("rutas_guardadas.txt", graph);
    
    if (stationsSaved && routesSaved)
    {
        logBST("Datos guardados exitosamente.", "green");
        logGraph("Datos guardados exitosamente.", "green");
        statusBar()->showMessage("Datos guardados correctamente", 3000);
        showInfoMessage("Guardado Exitoso", "Los datos se han guardado correctamente.");
    }
    else
    {
        showErrorMessage("Error de Guardado", "No se pudieron guardar todos los archivos.");
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
    if (!dataLoaded)
    {
        showInfoMessage("Informacion", "Debe cargar datos primero.");
        return;
    }
    
    logGraph("Generando reportes del sistema...", "blue");
    statusBar()->showMessage("Generando reportes...");
    
    bool statsReport = reportGenerator.generateSystemStats("reporte_estadisticas.txt", graph, bst);
    bool mstReport = reportGenerator.generateMSTReport("reporte_mst.txt", graph);
    bool connectivityReport = reportGenerator.generateConnectivityReport("reporte_conectividad.txt", graph);
    bool traversalReport = reportGenerator.generateTraversalReport("reporte_recorridos.txt", bst);
    
    int successCount = (statsReport ? 1 : 0) + (mstReport ? 1 : 0) + 
                       (connectivityReport ? 1 : 0) + (traversalReport ? 1 : 0);
    
    logGraph(QString("Reportes generados exitosamente (%1/4).").arg(successCount), "green");
    statusBar()->showMessage("Reportes generados", 3000);
    
    showInfoMessage("Reportes Generados",
        QString("Se han generado %1 reportes del sistema.").arg(successCount));
}

// Menu Action: Ver Ultimo Reporte
void MainWindow::onActionVerUltimoReporte()
{
    if (fileManager.fileExists("reporte_estadisticas.txt"))
    {
        logGraph("Abriendo reporte_estadisticas.txt...", "blue");
        QDesktopServices::openUrl(QUrl::fromLocalFile("reporte_estadisticas.txt"));
    }
    else
    {
        showInfoMessage("Informacion", 
            "No se encontró ningun reporte.\n"
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
    
    // Menu action connections
    connect(ui.actionCargarDatos, &QAction::triggered, this, &MainWindow::onActionCargarDatos);
    connect(ui.actionGuardarDatos, &QAction::triggered, this, &MainWindow::onActionGuardarDatos);
    connect(ui.actionSalir, &QAction::triggered, this, &MainWindow::onActionSalir);
    connect(ui.actionGenerarReportes, &QAction::triggered, this, &MainWindow::onActionGenerarReportes);
    connect(ui.actionVerUltimoReporte, &QAction::triggered, this, &MainWindow::onActionVerUltimoReporte);
    connect(ui.actionAcercaDe, &QAction::triggered, this, &MainWindow::onActionAcercaDe);
}

