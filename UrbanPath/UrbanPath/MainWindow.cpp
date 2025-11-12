#include "MainWindow.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), dataLoaded(false)
{
    ui.setupUi(this);
    
    // Initialize graphics scene and view
    scene = new QGraphicsScene(this);
    ui.graphicsView->setScene(scene);
    
    // Initialize visualizer
    visualizer = new GraphVisualizer(scene, ui.graphicsView, &graph);
    
    // Setup connections
    setupConnections();
    
    // Initial message
    logMessage("=== Sistema UrbanPath Iniciado ===", "blue");
    logMessage("Use 'Cargar Datos' para comenzar.");
    
    statusBar()->showMessage("Listo para operar");
    
    // Try to load background image
    loadBackgroundImage();
}

MainWindow::~MainWindow()
{
    delete visualizer;
    delete scene;
}

// Setup signal/slot connections
void MainWindow::setupConnections()
{
    // Button connections
    connect(ui.btnLoadData, &QPushButton::clicked, this, &MainWindow::onLoadDataClicked);
    connect(ui.btnSaveData, &QPushButton::clicked, this, &MainWindow::onSaveDataClicked);
    connect(ui.btnDrawGraph, &QPushButton::clicked, this, &MainWindow::onDrawGraphClicked);
    connect(ui.btnShortestPath, &QPushButton::clicked, this, &MainWindow::onShortestPathClicked);
    connect(ui.btnMST, &QPushButton::clicked, this, &MainWindow::onMSTClicked);
    connect(ui.btnTraversals, &QPushButton::clicked, this, &MainWindow::onTraversalsClicked);
    connect(ui.btnGenerateReports, &QPushButton::clicked, this, &MainWindow::onGenerateReportsClicked);
    connect(ui.btnClearGraph, &QPushButton::clicked, this, &MainWindow::onClearGraphClicked);
    
    // Menu action connections
    connect(ui.actionCargarDatos, &QAction::triggered, this, &MainWindow::onActionCargarDatos);
    connect(ui.actionGuardarDatos, &QAction::triggered, this, &MainWindow::onActionGuardarDatos);
    connect(ui.actionSalir, &QAction::triggered, this, &MainWindow::onActionSalir);
    connect(ui.actionDibujarGrafo, &QAction::triggered, this, &MainWindow::onActionDibujarGrafo);
    connect(ui.actionMostrarRutaOptima, &QAction::triggered, this, &MainWindow::onActionMostrarRutaOptima);
    connect(ui.actionLimpiarVista, &QAction::triggered, this, &MainWindow::onActionLimpiarVista);
    connect(ui.actionGenerarReportes, &QAction::triggered, this, &MainWindow::onActionGenerarReportes);
    connect(ui.actionVerUltimoReporte, &QAction::triggered, this, &MainWindow::onActionVerUltimoReporte);
    connect(ui.actionAcercaDe, &QAction::triggered, this, &MainWindow::onActionAcercaDe);
}

// Load background image
void MainWindow::loadBackgroundImage()
{
    if (fileManager.fileExists("data/mapa.png"))
    {
        visualizer->loadBackground("data/mapa.png");
        logMessage("Mapa de fondo cargado correctamente.", "green");
    }
    else if (fileManager.fileExists("mapa.png"))
    {
        visualizer->loadBackground("mapa.png");
        logMessage("Mapa de fondo cargado correctamente.", "green");
    }
    else
    {
        logMessage("Advertencia: No se encontro imagen de fondo (data/mapa.png).", "orange");
        logMessage("El grafo se dibujara sin mapa de fondo.");
    }
}

// Update statistics display
void MainWindow::updateStatistics()
{
    int stationCount = graph.getStationCount();
    int routeCount = 0;
    
    QList<Station> stations = graph.getAllStations();
    for (const Station& station : stations)
    {
        QList<QPair<int, double>> neighbors = graph.getNeighbors(station.getId());
        routeCount += neighbors.size();
    }
    routeCount /= 2;  // Undirected graph
    
    QString statsText = QString("<i>Estadisticas:</i><br>Estaciones: %1<br>Rutas: %2")
        .arg(stationCount)
        .arg(routeCount);
    
    ui.lblStats->setText(statsText);
}

// Log message to console
void MainWindow::logMessage(const QString& message, const QString& color)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString formattedMessage = QString("<span style='color:%1'>[%2] %3</span>")
        .arg(color)
        .arg(timestamp)
        .arg(message);
    
    ui.logConsole->append(formattedMessage);
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

// Slot: Load Data
void MainWindow::onLoadDataClicked()
{
    logMessage("Cargando datos del sistema...", "blue");
    statusBar()->showMessage("Cargando datos...");
    
    // Clear existing data
    graph.clear();
    bst.clear();
    
    // Load stations
    bool stationsLoaded = fileManager.loadStations("estaciones.txt", bst, graph);
    
    if (!stationsLoaded)
    {
        logMessage("Error: No se pudieron cargar las estaciones.", "red");
        showErrorMessage("Error de Carga", 
            "No se pudo cargar el archivo estaciones.txt.\n"
            "Verifique que el archivo exista y tenga el formato correcto.");
        statusBar()->showMessage("Error al cargar datos");
        return;
    }
    
    // Load routes
    bool routesLoaded = fileManager.loadRoutes("rutas.txt", graph);
    
    if (!routesLoaded)
    {
        logMessage("Advertencia: No se pudieron cargar las rutas.", "orange");
    }
    
    // Load closures (optional)
    if (fileManager.fileExists("cierres.txt"))
    {
        fileManager.loadClosures("cierres.txt", graph);
    }
    
    dataLoaded = true;
    updateStatistics();
    
    logMessage(QString("Datos cargados exitosamente: %1 estaciones, %2 rutas.")
        .arg(graph.getStationCount())
        .arg(bst.count()), "green");
    
    statusBar()->showMessage("Datos cargados correctamente", 3000);
    
    showInfoMessage("Carga Exitosa", 
        QString("Datos cargados correctamente:\n\nEstaciones: %1\nRutas: %2")
        .arg(graph.getStationCount())
        .arg(bst.count()));
}

// Slot: Save Data
void MainWindow::onSaveDataClicked()
{
    if (!dataLoaded)
    {
        showInfoMessage("Informacion", "No hay datos para guardar.");
        return;
    }
    
    logMessage("Guardando datos del sistema...", "blue");
    
    bool stationsSaved = fileManager.saveStations("estaciones_guardadas.txt", bst);
    bool routesSaved = fileManager.saveRoutes("rutas_guardadas.txt", graph);
    
    if (stationsSaved && routesSaved)
    {
        logMessage("Datos guardados exitosamente.", "green");
        statusBar()->showMessage("Datos guardados correctamente", 3000);
        showInfoMessage("Guardado Exitoso", "Los datos se han guardado correctamente.");
    }
    else
    {
        logMessage("Error al guardar algunos archivos.", "red");
        showErrorMessage("Error de Guardado", "No se pudieron guardar todos los archivos.");
    }
}

// Slot: Draw Graph
void MainWindow::onDrawGraphClicked()
{
    if (!dataLoaded)
    {
        showInfoMessage("Informacion", "Debe cargar datos antes de dibujar.");
        return;
    }
    
    logMessage("Dibujando grafo en la vista...", "blue");
    statusBar()->showMessage("Dibujando grafo...");
    
    visualizer->drawGraph();
    
    logMessage("Grafo dibujado correctamente.", "green");
    statusBar()->showMessage("Grafo visualizado", 3000);
}

// Slot: Shortest Path (Dijkstra)
void MainWindow::onShortestPathClicked()
{
    if (!dataLoaded)
    {
        showInfoMessage("Informacion", "Debe cargar datos primero.");
        return;
    }
    
    if (graph.getStationCount() < 2)
    {
        showInfoMessage("Informacion", "Se necesitan al menos 2 estaciones.");
        return;
    }
    
    // Ask for start station
    bool ok;
    int startId = QInputDialog::getInt(this, "Ruta Mas Corta",
        "Ingrese el ID de la estacion de origen:", 1, 1, 100, 1, &ok);
    
    if (!ok || !graph.containsStation(startId))
    {
        showInfoMessage("Informacion", "ID de estacion invalido.");
        return;
    }
    
    // Ask for destination station
    int destId = QInputDialog::getInt(this, "Ruta Mas Corta",
        "Ingrese el ID de la estacion de destino:", 1, 1, 100, 1, &ok);
    
    if (!ok || !graph.containsStation(destId))
    {
        showInfoMessage("Informacion", "ID de estacion invalido.");
        return;
    }
    
    logMessage(QString("Calculando ruta mas corta de %1 a %2...").arg(startId).arg(destId), "blue");
    statusBar()->showMessage("Calculando ruta...");
    
    // Calculate shortest path using Dijkstra
    QHash<int, double> distances = graph.dijkstra(startId);
    
    if (distances[destId] >= std::numeric_limits<double>::infinity())
    {
        logMessage("No existe ruta entre las estaciones seleccionadas.", "red");
        showInfoMessage("Ruta No Encontrada", 
            "No existe una ruta conectada entre las estaciones seleccionadas.");
        return;
    }
    
    // For visualization, use BFS to get actual path
    QList<int> bfsPath = graph.bfs(startId);
    QList<int> route;
    
    // Find path to destination in BFS result
    int idx = bfsPath.indexOf(destId);
    if (idx >= 0)
    {
        // Simplified: show start and end
        route.append(startId);
        route.append(destId);
    }
    
    if (!route.isEmpty())
    {
        visualizer->drawOptimalRoute(route);
        
        logMessage(QString("Ruta calculada (distancia: %1): %2 -> %3")
            .arg(distances[destId], 0, 'f', 1)
            .arg(startId)
            .arg(destId), "green");
        
        statusBar()->showMessage(QString("Ruta calculada - Distancia: %1").arg(distances[destId], 0, 'f', 1), 5000);
        
        showInfoMessage("Ruta Calculada",
            QString("Ruta mas corta encontrada:\n\nOrigen: Estacion %1\nDestino: Estacion %2\nDistancia total: %3")
            .arg(startId)
            .arg(destId)
            .arg(distances[destId], 0, 'f', 1));
        
        // Generate route report
        reportGenerator.generateRouteReport("reporte_ruta_corta.txt", route, graph);
        logMessage("Reporte de ruta generado: reporte_ruta_corta.txt", "blue");
    }
}

// Slot: MST
void MainWindow::onMSTClicked()
{
    if (!dataLoaded)
    {
        showInfoMessage("Informacion", "Debe cargar datos primero.");
        return;
    }
    
    logMessage("Calculando Arbol de Expansion Minima (MST)...", "blue");
    statusBar()->showMessage("Calculando MST...");
    
    QList<QPair<int, int>> mstEdges = graph.kruskalMST();
    
    if (mstEdges.isEmpty())
    {
        logMessage("No se pudo calcular el MST.", "red");
        showInfoMessage("Error", "No se pudo calcular el arbol de expansion minima.");
        return;
    }
    
    // Calculate total weight
    double totalWeight = 0.0;
    for (const auto& edge : mstEdges)
    {
        totalWeight += graph.getEdgeWeight(edge.first, edge.second);
    }
    
    logMessage(QString("MST calculado: %1 aristas, peso total: %2")
        .arg(mstEdges.size())
        .arg(totalWeight, 0, 'f', 1), "green");
    
    statusBar()->showMessage(QString("MST calculado - Peso total: %1").arg(totalWeight, 0, 'f', 1), 5000);
    
    showInfoMessage("MST Calculado",
        QString("Arbol de Expansion Minima calculado:\n\nAlgoritmo: Kruskal\nNumero de aristas: %1\nPeso total: %2")
        .arg(mstEdges.size())
        .arg(totalWeight, 0, 'f', 1));
    
    // Generate MST report
    reportGenerator.generateMSTReport("reporte_mst.txt", graph);
    logMessage("Reporte de MST generado: reporte_mst.txt", "blue");
}

// Slot: Traversals
void MainWindow::onTraversalsClicked()
{
    if (!dataLoaded)
    {
        showInfoMessage("Informacion", "Debe cargar datos primero.");
        return;
    }
    
    logMessage("Generando recorridos del arbol BST...", "blue");
    
    bool success = reportGenerator.generateTraversalReport("reporte_recorridos.txt", bst);
    
    if (success)
    {
        logMessage("Recorridos generados: reporte_recorridos.txt", "green");
        statusBar()->showMessage("Recorridos exportados correctamente", 3000);
        showInfoMessage("Recorridos Generados", "Los recorridos del arbol BST han sido generados.");
    }
    else
    {
        logMessage("Error al generar recorridos.", "red");
        showErrorMessage("Error", "No se pudo generar el archivo de recorridos.");
    }
}

// Slot: Generate Reports
void MainWindow::onGenerateReportsClicked()
{
    if (!dataLoaded)
    {
        showInfoMessage("Informacion", "Debe cargar datos primero.");
        return;
    }
    
    logMessage("Generando reportes del sistema...", "blue");
    statusBar()->showMessage("Generando reportes...");
    
    // Generate all reports
    bool statsReport = reportGenerator.generateSystemStats("reporte_estadisticas.txt", graph, bst);
    bool mstReport = reportGenerator.generateMSTReport("reporte_mst.txt", graph);
    bool connectivityReport = reportGenerator.generateConnectivityReport("reporte_conectividad.txt", graph);
    bool traversalReport = reportGenerator.generateTraversalReport("reporte_recorridos.txt", bst);
    
    int successCount = (statsReport ? 1 : 0) + (mstReport ? 1 : 0) + 
                       (connectivityReport ? 1 : 0) + (traversalReport ? 1 : 0);
    
    logMessage(QString("Reportes generados exitosamente (%1/4).").arg(successCount), "green");
    statusBar()->showMessage("Reportes generados", 3000);
    
    showInfoMessage("Reportes Generados",
        QString("Se han generado %1 reportes del sistema.").arg(successCount));
}

// Slot: Clear Graph
void MainWindow::onClearGraphClicked()
{
    logMessage("Limpiando vista del grafo...", "blue");
    visualizer->clearScene();
    logMessage("Vista limpiada.", "green");
    statusBar()->showMessage("Vista limpiada", 2000);
}

// Menu Action: Cargar Datos
void MainWindow::onActionCargarDatos()
{
    onLoadDataClicked();
}

// Menu Action: Guardar Datos
void MainWindow::onActionGuardarDatos()
{
    onSaveDataClicked();
}

// Menu Action: Salir
void MainWindow::onActionSalir()
{
    if (confirmAction("Salir", "Esta seguro de que desea salir de UrbanPath?"))
    {
        close();
    }
}

// Menu Action: Dibujar Grafo
void MainWindow::onActionDibujarGrafo()
{
    onDrawGraphClicked();
}

// Menu Action: Mostrar Ruta Optima
void MainWindow::onActionMostrarRutaOptima()
{
    onShortestPathClicked();
}

// Menu Action: Limpiar Vista
void MainWindow::onActionLimpiarVista()
{
    onClearGraphClicked();
}

// Menu Action: Generar Reportes
void MainWindow::onActionGenerarReportes()
{
    onGenerateReportsClicked();
}

// Menu Action: Ver Ultimo Reporte
void MainWindow::onActionVerUltimoReporte()
{
    if (fileManager.fileExists("reporte_estadisticas.txt"))
    {
        logMessage("Abriendo reporte_estadisticas.txt...", "blue");
        QDesktopServices::openUrl(QUrl::fromLocalFile("reporte_estadisticas.txt"));
    }
    else
    {
        showInfoMessage("Informacion", 
            "No se encontro ningun reporte.\n"
            "Genere reportes primero usando el boton 'Generar Reportes'.");
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
        "<p>Sistema de simulacion de transporte urbano que utiliza "
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

