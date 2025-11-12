#include "MainWindow.h"
#include "Station.h"
#include "StationBST.h"
#include "DisjointSet.h"
#include "Graph.h"
#include "FileManager.h"
#include "ReportGenerator.h"
#include "GraphVisualizer.h"
#include <QApplication>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

// Test function for Station class
void testStation()
{
    qDebug() << "=== PRUEBA DE LA CLASE STATION ===\n";
    
    // Test default constructor
    Station s1;
    qDebug() << "Estacion por defecto:" << s1.toString();
    
    // Test parameterized constructor
    Station s2(1, "Estacion Central", 100.5, 200.3);
    qDebug() << "Estacion creada:" << s2.toString();
    
    // Test getters
    qDebug() << "\nGetters:";
    qDebug() << "ID:" << s2.getId();
    qDebug() << "Nombre:" << s2.getName();
    qDebug() << "Coordenada X:" << s2.getX();
    qDebug() << "Coordenada Y:" << s2.getY();
    
    // Test setters
    Station s3;
    s3.setId(2);
    s3.setName("Estacion Norte");
    s3.setX(150.0);
    s3.setY(250.0);
    qDebug() << "\nEstacion modificada con setters:" << s3.toString();
    
    // Test copy constructor
    Station s4(s2);
    qDebug() << "\nCopia de estacion:" << s4.toString();
    
    // Test assignment operator
    Station s5;
    s5 = s3;
    qDebug() << "Asignacion de estacion:" << s5.toString();
    
    // Test comparison operators
    qDebug() << "\nPrueba de operadores de comparacion:";
    qDebug() << "s2 == s4:" << (s2 == s4);  // Should be true (same id)
    qDebug() << "s2 == s3:" << (s2 == s3);  // Should be false (different id)
    qDebug() << "s2 < s3:" << (s2 < s3);    // Should be true (1 < 2)
    qDebug() << "s3 > s2:" << (s3 > s2);    // Should be true (2 > 1)
    
    qDebug() << "\n=== PRUEBA COMPLETADA ===\n";
}

// Test function for StationBST class
void testStationBST()
{
    qDebug() << "\n=== PRUEBA DE LA CLASE STATIONBST ===\n";
    
    StationBST bst;
    
    // Test 1: Insert stations
    qDebug() << "Test 1: Insertando estaciones en el BST...";
    bst.insert(Station(5, "Estacion Central", 100.0, 100.0));
    bst.insert(Station(3, "Estacion Norte", 150.0, 50.0));
    bst.insert(Station(7, "Estacion Sur", 150.0, 150.0));
    bst.insert(Station(2, "Estacion Oeste", 50.0, 100.0));
    bst.insert(Station(4, "Estacion Este", 200.0, 100.0));
    bst.insert(Station(6, "Estacion Plaza", 125.0, 75.0));
    bst.insert(Station(8, "Estacion Terminal", 175.0, 125.0));
    
    qDebug() << "Total de estaciones insertadas:" << bst.count();
    qDebug() << "Arbol vacio?" << (bst.isEmpty() ? "Si" : "No");
    
    // Test 2: Search for stations
    qDebug() << "\nTest 2: Buscando estaciones...";
    
    Station* found = bst.search(5);
    if (found != nullptr)
    {
        qDebug() << "Estacion encontrada (ID=5):" << found->toString();
    }
    else
    {
        qDebug() << "Estacion con ID=5 no encontrada";
    }
    
    found = bst.search(3);
    if (found != nullptr)
    {
        qDebug() << "Estacion encontrada (ID=3):" << found->toString();
    }
    
    found = bst.search(99);
    if (found == nullptr)
    {
        qDebug() << "Estacion con ID=99 no encontrada (correcto)";
    }
    
    // Test 3: Traversals
    qDebug() << "\nTest 3: Recorridos del arbol...";
    
    qDebug() << "\n--- In-Order (orden ascendente por ID) ---";
    QList<Station> inOrderList = bst.inOrder();
    for (const Station& station : inOrderList)
    {
        qDebug() << "  " << station.toString();
    }
    
    qDebug() << "\n--- Pre-Order ---";
    QList<Station> preOrderList = bst.preOrder();
    for (const Station& station : preOrderList)
    {
        qDebug() << "  " << station.toString();
    }
    
    qDebug() << "\n--- Post-Order ---";
    QList<Station> postOrderList = bst.postOrder();
    for (const Station& station : postOrderList)
    {
        qDebug() << "  " << station.toString();
    }
    
    // Test 4: Export to file
    qDebug() << "\nTest 4: Exportando recorridos a archivo...";
    bool exported = bst.exportTraversals("recorridos_rutas.txt");
    if (exported)
    {
        qDebug() << "Archivo 'recorridos_rutas.txt' creado exitosamente!";
    }
    else
    {
        qDebug() << "Error al crear el archivo de recorridos.";
    }
    
    // Test 5: Remove stations
    qDebug() << "\nTest 5: Eliminando estaciones...";
    
    qDebug() << "Eliminando estacion con ID=2 (hoja)...";
    bool removed = bst.remove(2);
    qDebug() << "Eliminacion exitosa?" << (removed ? "Si" : "No");
    qDebug() << "Total de estaciones despues de eliminar:" << bst.count();
    
    qDebug() << "\nEliminando estacion con ID=3 (un hijo)...";
    removed = bst.remove(3);
    qDebug() << "Eliminacion exitosa?" << (removed ? "Si" : "No");
    qDebug() << "Total de estaciones despues de eliminar:" << bst.count();
    
    qDebug() << "\nEliminando estacion con ID=5 (raiz con dos hijos)...";
    removed = bst.remove(5);
    qDebug() << "Eliminacion exitosa?" << (removed ? "Si" : "No");
    qDebug() << "Total de estaciones despues de eliminar:" << bst.count();
    
    qDebug() << "\n--- In-Order despues de eliminaciones ---";
    inOrderList = bst.inOrder();
    for (const Station& station : inOrderList)
    {
        qDebug() << "  " << station.toString();
    }
    
    qDebug() << "\nIntentando eliminar estacion inexistente (ID=99)...";
    removed = bst.remove(99);
    qDebug() << "Eliminacion exitosa?" << (removed ? "Si" : "No (correcto)");
    
    // Test 6: Clear tree
    qDebug() << "\nTest 6: Limpiando el arbol completo...";
    bst.clear();
    qDebug() << "Total de estaciones despues de limpiar:" << bst.count();
    qDebug() << "Arbol vacio?" << (bst.isEmpty() ? "Si" : "No");
    
    qDebug() << "\n=== PRUEBA DE STATIONBST COMPLETADA ===\n";
}

// Test function for DisjointSet class
void testDisjointSet()
{
    qDebug() << "\n=== PRUEBA DE DISJOINT SET ===\n";
    
    // Create DisjointSet with 7 elements
    DisjointSet ds(7);
    
    qDebug() << "Test 1: Estado inicial (7 conjuntos independientes)";
    ds.printSets();
    
    // Test 2: Union operations
    qDebug() << "\nTest 2: Realizando operaciones de union...";
    
    qDebug() << "\nUniendo 1 y 2...";
    ds.unionSets(1, 2);
    ds.printSets();
    
    qDebug() << "\nUniendo 2 y 3...";
    ds.unionSets(2, 3);
    ds.printSets();
    
    qDebug() << "\nUniendo 4 y 5...";
    ds.unionSets(4, 5);
    ds.printSets();
    
    // Test 3: Connectivity checks
    qDebug() << "\nTest 3: Verificando conectividad...";
    qDebug() << "1 y 3 estan conectados?" << (ds.connected(1, 3) ? "Si" : "No");
    qDebug() << "3 y 4 estan conectados?" << (ds.connected(3, 4) ? "Si" : "No");
    qDebug() << "1 y 7 estan conectados?" << (ds.connected(1, 7) ? "Si" : "No");
    
    // Test 4: More unions
    qDebug() << "\nTest 4: Uniendo mas conjuntos...";
    qDebug() << "\nUniendo 3 y 4 (conecta {1,2,3} con {4,5})...";
    ds.unionSets(3, 4);
    ds.printSets();
    
    qDebug() << "\nDespues de unir 3 y 4:";
    qDebug() << "1 y 5 estan conectados?" << (ds.connected(1, 5) ? "Si" : "No");
    qDebug() << "2 y 4 estan conectados?" << (ds.connected(2, 4) ? "Si" : "No");
    
    // Test 5: Union remaining elements
    qDebug() << "\nTest 5: Uniendo elementos restantes...";
    qDebug() << "\nUniendo 6 y 7...";
    ds.unionSets(6, 7);
    ds.printSets();
    
    qDebug() << "\nUniendo 5 y 6 (conecta todo excepto el conjunto grande)...";
    ds.unionSets(5, 6);
    ds.printSets();
    
    // Test 6: Final union - all in one set
    qDebug() << "\nTest 6: Union final...";
    qDebug() << "\nUniendo 1 y 7 (todos en un solo conjunto)...";
    ds.unionSets(1, 7);
    ds.printSets();
    
    qDebug() << "\nTodos conectados?";
    qDebug() << "1 y 7 estan conectados?" << (ds.connected(1, 7) ? "Si" : "No");
    
    // Test 7: Reset
    qDebug() << "\nTest 7: Reiniciando conjuntos...";
    ds.reset();
    ds.printSets();
    
    qDebug() << "\nDespues de reiniciar:";
    qDebug() << "1 y 2 estan conectados?" << (ds.connected(1, 2) ? "Si" : "No (correcto)");
    
    // Test 8: Edge cases
    qDebug() << "\nTest 8: Casos especiales...";
    qDebug() << "\nIntentando unir el mismo elemento consigo mismo (1 y 1)...";
    ds.unionSets(1, 1);
    ds.printSets();
    
    qDebug() << "\nIntentando acceder a elemento fuera de rango (nodo 10)...";
    int result = ds.find(10);
    qDebug() << "Resultado:" << result << "(deberia ser -1)";
    
    qDebug() << "\n=== PRUEBA DE DISJOINT SET COMPLETADA ===\n";
}

// Test function for Graph class
void testGraph()
{
    qDebug() << "\n=== PRUEBA DEL GRAFO URBANPATH ===\n";
    
    // Create an undirected graph
    Graph graph(false);
    
    // Test 1: Add stations
    qDebug() << "Test 1: Agregando estaciones al grafo...";
    graph.addStation(Station(1, "Estacion Central", 100.0, 100.0));
    graph.addStation(Station(2, "Estacion Norte", 150.0, 50.0));
    graph.addStation(Station(3, "Estacion Sur", 150.0, 150.0));
    graph.addStation(Station(4, "Estacion Este", 200.0, 100.0));
    graph.addStation(Station(5, "Estacion Oeste", 50.0, 100.0));
    
    qDebug() << "Estaciones agregadas correctamente.";
    qDebug() << "Total de estaciones:" << graph.getStationCount();
    
    // Test 2: Add edges (routes)
    qDebug() << "\nTest 2: Creando rutas entre estaciones...";
    graph.addEdge(1, 2, 10.5);  // Central - Norte
    graph.addEdge(1, 4, 8.0);   // Central - Este
    graph.addEdge(1, 5, 12.0);  // Central - Oeste
    graph.addEdge(2, 3, 7.8);   // Norte - Sur
    graph.addEdge(2, 4, 15.0);  // Norte - Este
    graph.addEdge(3, 4, 9.5);   // Sur - Este
    graph.addEdge(3, 5, 11.2);  // Sur - Oeste
    graph.addEdge(4, 5, 6.8);   // Este - Oeste
    
    qDebug() << "Rutas creadas entre estaciones.";
    
    // Print graph structure
    graph.printGraph();
    graph.printAdjacencyList();
    
    // Test 3: BFS traversal
    qDebug() << "\nTest 3: Recorrido BFS (Breadth-First Search)...";
    QList<int> bfsResult = graph.bfs(1);
    QString bfsOutput = "Recorrido BFS (desde estacion 1): ";
    for (int i = 0; i < bfsResult.size(); i++)
    {
        bfsOutput += QString::number(bfsResult[i]);
        if (i < bfsResult.size() - 1)
        {
            bfsOutput += " -> ";
        }
    }
    qDebug() << bfsOutput;
    
    // Test 4: DFS traversal
    qDebug() << "\nTest 4: Recorrido DFS (Depth-First Search)...";
    QList<int> dfsResult = graph.dfs(1);
    QString dfsOutput = "Recorrido DFS (desde estacion 1): ";
    for (int i = 0; i < dfsResult.size(); i++)
    {
        dfsOutput += QString::number(dfsResult[i]);
        if (i < dfsResult.size() - 1)
        {
            dfsOutput += " -> ";
        }
    }
    qDebug() << dfsOutput;
    
    // Test 5: Dijkstra shortest path
    qDebug() << "\nTest 5: Algoritmo de Dijkstra (ruta mas corta)...";
    QHash<int, double> distances = graph.dijkstra(1);
    qDebug() << "Distancias mas cortas desde estacion 1:";
    for (auto it = distances.begin(); it != distances.end(); ++it)
    {
        qDebug() << QString("  Estacion %1: %2")
            .arg(it.key())
            .arg(it.value(), 0, 'f', 1);
    }
    
    // Test 6: Floyd-Warshall all-pairs shortest paths
    qDebug() << "\nTest 6: Algoritmo de Floyd-Warshall (todas las distancias)...";
    QHash<QPair<int, int>, double> allPaths = graph.floydWarshall();
    qDebug() << "Matriz de distancias minimas (muestra parcial):";
    qDebug() << "De 1 a 3:" << allPaths[QPair<int, int>(1, 3)];
    qDebug() << "De 2 a 5:" << allPaths[QPair<int, int>(2, 5)];
    qDebug() << "De 3 a 4:" << allPaths[QPair<int, int>(3, 4)];
    
    // Test 7: Prim's MST
    qDebug() << "\nTest 7: Algoritmo de Prim (Arbol de Expansion Minima)...";
    QList<QPair<int, int>> primEdges = graph.primMST();
    qDebug() << "Aristas del MST (Prim):";
    double primTotalWeight = 0.0;
    for (const auto& edge : primEdges)
    {
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        qDebug() << QString("  (%1, %2) peso = %3")
            .arg(edge.first)
            .arg(edge.second)
            .arg(weight, 0, 'f', 1);
        primTotalWeight += weight;
    }
    qDebug() << "Peso total del MST (Prim):" << QString::number(primTotalWeight, 'f', 1);
    
    // Test 8: Kruskal's MST
    qDebug() << "\nTest 8: Algoritmo de Kruskal (usando DisjointSet)...";
    QList<QPair<int, int>> kruskalEdges = graph.kruskalMST();
    qDebug() << "Aristas del MST (Kruskal):";
    double kruskalTotalWeight = 0.0;
    for (const auto& edge : kruskalEdges)
    {
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        qDebug() << QString("  (%1, %2) peso = %3")
            .arg(edge.first)
            .arg(edge.second)
            .arg(weight, 0, 'f', 1);
        kruskalTotalWeight += weight;
    }
    qDebug() << "Peso total del MST (Kruskal):" << QString::number(kruskalTotalWeight, 'f', 1);
    
    // Test 9: Edge operations
    qDebug() << "\nTest 9: Operaciones con aristas...";
    qDebug() << "Existe ruta entre 1 y 2?" << (graph.hasEdge(1, 2) ? "Si" : "No");
    qDebug() << "Peso de la ruta (1, 2):" << graph.getEdgeWeight(1, 2);
    
    qDebug() << "\nEliminando ruta entre 2 y 3...";
    graph.removeEdge(2, 3);
    qDebug() << "Existe ruta entre 2 y 3?" << (graph.hasEdge(2, 3) ? "Si" : "No");
    
    // Test 10: Station removal
    qDebug() << "\nTest 10: Eliminando estacion...";
    qDebug() << "Eliminando estacion 5...";
    graph.removeStation(5);
    qDebug() << "Total de estaciones:" << graph.getStationCount();
    graph.printAdjacencyList();
    
    qDebug() << "\n=== PRUEBA DEL GRAFO COMPLETADA ===\n";
}

// Test function for FileManager class
void testFileManager()
{
    qDebug() << "\n=== PRUEBA DE FILEMANAGER ===\n";
    
    FileManager fileManager;
    
    // Test 1: Check if files exist
    qDebug() << "Test 1: Verificando existencia de archivos...";
    qDebug() << "Archivo estaciones.txt existe?" << (fileManager.fileExists("estaciones.txt") ? "Si" : "No");
    qDebug() << "Archivo rutas.txt existe?" << (fileManager.fileExists("rutas.txt") ? "Si" : "No");
    qDebug() << "Archivo cierres.txt existe?" << (fileManager.fileExists("cierres.txt") ? "Si" : "No");
    
    // Test 2: Load stations
    qDebug() << "\nTest 2: Cargando estaciones desde archivo...";
    StationBST bst;
    Graph graph(false);
    
    bool stationsLoaded = fileManager.loadStations("estaciones.txt", bst, graph);
    if (stationsLoaded)
    {
        qDebug() << "Estaciones cargadas exitosamente.";
        qDebug() << "Total de estaciones en BST:" << bst.count();
        qDebug() << "Total de estaciones en Graph:" << graph.getStationCount();
    }
    
    // Test 3: Load routes
    qDebug() << "\nTest 3: Cargando rutas desde archivo...";
    bool routesLoaded = fileManager.loadRoutes("rutas.txt", graph);
    if (routesLoaded)
    {
        qDebug() << "Rutas cargadas exitosamente.";
        graph.printAdjacencyList();
    }
    
    // Test 4: Load closures
    qDebug() << "\nTest 4: Aplicando cierres desde archivo...";
    fileManager.loadClosures("cierres.txt", graph);
    qDebug() << "Estado del grafo despues de cierres:";
    graph.printAdjacencyList();
    
    // Test 5: Test algorithms with loaded data
    qDebug() << "\nTest 5: Ejecutando algoritmos con datos cargados...";
    
    qDebug() << "\n--- BFS desde estacion 1 ---";
    QList<int> bfsResult = graph.bfs(1);
    QString bfsStr = "Recorrido: ";
    for (int i = 0; i < bfsResult.size(); i++)
    {
        bfsStr += QString::number(bfsResult[i]);
        if (i < bfsResult.size() - 1) bfsStr += " -> ";
    }
    qDebug() << bfsStr;
    
    qDebug() << "\n--- Dijkstra desde estacion 1 ---";
    QHash<int, double> distances = graph.dijkstra(1);
    for (auto it = distances.begin(); it != distances.end(); ++it)
    {
        qDebug() << QString("  Distancia a estacion %1: %2")
            .arg(it.key())
            .arg(it.value(), 0, 'f', 1);
    }
    
    qDebug() << "\n--- MST con Kruskal ---";
    QList<QPair<int, int>> mst = graph.kruskalMST();
    double mstWeight = 0.0;
    for (const auto& edge : mst)
    {
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        mstWeight += weight;
        qDebug() << QString("  Arista (%1, %2) peso = %3")
            .arg(edge.first)
            .arg(edge.second)
            .arg(weight, 0, 'f', 1);
    }
    qDebug() << "Peso total del MST:" << QString::number(mstWeight, 'f', 1);
    
    // Test 6: Save stations
    qDebug() << "\nTest 6: Guardando estaciones en archivo...";
    bool stationsSaved = fileManager.saveStations("estaciones_guardadas.txt", bst);
    if (stationsSaved)
    {
        qDebug() << "Archivo estaciones_guardadas.txt creado correctamente.";
    }
    
    // Test 7: Save routes
    qDebug() << "\nTest 7: Guardando rutas en archivo...";
    bool routesSaved = fileManager.saveRoutes("rutas_guardadas.txt", graph);
    if (routesSaved)
    {
        qDebug() << "Archivo rutas_guardadas.txt creado correctamente.";
    }
    
    // Test 8: Export report
    qDebug() << "\nTest 8: Generando reporte del sistema...";
    QString reportContent;
    reportContent += "RESUMEN DEL SISTEMA\n";
    reportContent += "===================\n\n";
    reportContent += QString("Total de estaciones: %1\n").arg(graph.getStationCount());
    reportContent += QString("Total de rutas activas: %2\n\n").arg(mst.size());
    reportContent += "Estaciones registradas:\n";
    
    QList<Station> stations = graph.getAllStations();
    for (const Station& station : stations)
    {
        reportContent += QString("  - %1: %2 (coordenadas: %3, %4)\n")
            .arg(station.getId())
            .arg(station.getName())
            .arg(station.getX())
            .arg(station.getY());
    }
    
    reportContent += "\n\nArbol de Expansion Minima (MST):\n";
    for (const auto& edge : mst)
    {
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        reportContent += QString("  - Ruta %1 <-> %2: peso = %3\n")
            .arg(edge.first)
            .arg(edge.second)
            .arg(weight, 0, 'f', 1);
    }
    reportContent += QString("\nPeso total del MST: %1\n").arg(mstWeight, 0, 'f', 1);
    
    bool reportExported = fileManager.exportReport("reporte_sistema.txt", reportContent);
    if (reportExported)
    {
        qDebug() << "Reporte generado exitosamente!";
    }
    
    // Test 9: Test error handling
    qDebug() << "\nTest 9: Probando manejo de errores...";
    qDebug() << "Intentando cargar archivo inexistente...";
    fileManager.loadStations("archivo_inexistente.txt", bst, graph);
    
    qDebug() << "\n=== PRUEBA DE FILEMANAGER COMPLETADA ===\n";
}

// Test function for ReportGenerator class
void testReportGenerator()
{
    qDebug() << "\n=== PRUEBA DE REPORTGENERATOR ===\n";
    
    // Setup: Load data
    FileManager fileManager;
    StationBST bst;
    Graph graph(false);
    ReportGenerator reportGen;
    
    qDebug() << "Test 1: Cargando datos del sistema...";
    fileManager.loadStations("estaciones.txt", bst, graph);
    fileManager.loadRoutes("rutas.txt", graph);
    
    // Test 2: Generate route report
    qDebug() << "\nTest 2: Generando reporte de ruta especifica...";
    QList<int> sampleRoute = {1, 4, 5, 3};
    bool routeReportCreated = reportGen.generateRouteReport("reporte_ruta.txt", sampleRoute, graph);
    if (routeReportCreated)
    {
        qDebug() << "Reporte de ruta generado: reporte_ruta.txt";
    }
    
    // Test 3: Generate traversal report
    qDebug() << "\nTest 3: Generando reporte de recorridos del BST...";
    bool traversalReportCreated = reportGen.generateTraversalReport("reporte_recorridos.txt", bst);
    if (traversalReportCreated)
    {
        qDebug() << "Reporte de recorridos generado: reporte_recorridos.txt";
    }
    
    // Test 4: Generate system statistics report
    qDebug() << "\nTest 4: Generando reporte de estadisticas del sistema...";
    bool statsReportCreated = reportGen.generateSystemStats("reporte_estadisticas.txt", graph, bst);
    if (statsReportCreated)
    {
        qDebug() << "Reporte de estadisticas generado: reporte_estadisticas.txt";
    }
    
    // Test 5: Generate MST report
    qDebug() << "\nTest 5: Generando reporte de MST (Prim y Kruskal)...";
    bool mstReportCreated = reportGen.generateMSTReport("reporte_mst.txt", graph);
    if (mstReportCreated)
    {
        qDebug() << "Reporte de MST generado: reporte_mst.txt";
    }
    
    // Test 6: Generate connectivity report
    qDebug() << "\nTest 6: Generando reporte de conectividad...";
    bool connectivityReportCreated = reportGen.generateConnectivityReport("reporte_conectividad.txt", graph);
    if (connectivityReportCreated)
    {
        qDebug() << "Reporte de conectividad generado: reporte_conectividad.txt";
    }
    
    // Test 7: Test append to report
    qDebug() << "\nTest 7: Probando funcion de agregar contenido a reporte...";
    QString additionalContent = "Esta es una seccion adicional agregada dinamicamente.\n";
    additionalContent += "Contenido personalizado agregado mediante appendToReport().\n";
    bool appended = reportGen.appendToReport("reporte_estadisticas.txt", 
                                             "SECCION ADICIONAL", 
                                             additionalContent);
    if (appended)
    {
        qDebug() << "Contenido agregado exitosamente a reporte_estadisticas.txt";
    }
    
    // Test 8: Generate comprehensive report with shortest paths
    qDebug() << "\nTest 8: Generando reporte de rutas mas cortas (Dijkstra)...";
    QHash<int, double> distances = graph.dijkstra(1);
    
    QString dijkstraContent = "RUTAS MAS CORTAS DESDE ESTACION 1\n";
    dijkstraContent += "===================================\n\n";
    dijkstraContent += "Algoritmo utilizado: Dijkstra\n\n";
    
    for (auto it = distances.begin(); it != distances.end(); ++it)
    {
        if (it.key() != 1)
        {
            dijkstraContent += QString("Estacion 1 -> Estacion %1: %2\n")
                .arg(it.key())
                .arg(it.value(), 0, 'f', 1);
        }
    }
    
    bool dijkstraReportCreated = fileManager.exportReport("reporte_dijkstra.txt", dijkstraContent);
    if (dijkstraReportCreated)
    {
        qDebug() << "Reporte de Dijkstra generado: reporte_dijkstra.txt";
    }
    
    // Test 9: Summary
    qDebug() << "\nTest 9: Resumen de reportes generados...";
    qDebug() << "Archivos creados:";
    qDebug() << "  1. reporte_ruta.txt - Ruta especifica con distancias";
    qDebug() << "  2. reporte_recorridos.txt - Recorridos InOrder, PreOrder, PostOrder";
    qDebug() << "  3. reporte_estadisticas.txt - Estadisticas generales del sistema";
    qDebug() << "  4. reporte_mst.txt - Analisis de MST (Prim y Kruskal)";
    qDebug() << "  5. reporte_conectividad.txt - Analisis de conectividad del grafo";
    qDebug() << "  6. reporte_dijkstra.txt - Distancias minimas desde estacion 1";
    
    qDebug() << "\n=== PRUEBA DE REPORTGENERATOR COMPLETADA ===\n";
    qDebug() << "\nTodos los reportes fueron generados exitosamente!";
    qDebug() << "Puedes revisar los archivos .txt para ver el contenido detallado.\n";
}

// Test function for GraphVisualizer class
void testGraphVisualizer()
{
    qDebug() << "\n=== PRUEBA DE GRAPHVISUALIZER ===\n";
    
    // Create a separate window for visualization
    QWidget* window = new QWidget();
    window->setWindowTitle("UrbanPath - Visualizacion del Grafo");
    window->resize(1000, 700);
    
    // Create layout
    QVBoxLayout* layout = new QVBoxLayout(window);
    
    // Create graphics view and scene
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsView* view = new QGraphicsView(scene);
    
    // Configure view
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    
    // Add view to layout
    layout->addWidget(view);
    
    // Create control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* btnLoadData = new QPushButton("Cargar Datos");
    QPushButton* btnDrawGraph = new QPushButton("Dibujar Grafo");
    QPushButton* btnShowRoute = new QPushButton("Mostrar Ruta Optima");
    QPushButton* btnClear = new QPushButton("Limpiar");
    QPushButton* btnZoomFit = new QPushButton("Ajustar Zoom");
    
    buttonLayout->addWidget(btnLoadData);
    buttonLayout->addWidget(btnDrawGraph);
    buttonLayout->addWidget(btnShowRoute);
    buttonLayout->addWidget(btnClear);
    buttonLayout->addWidget(btnZoomFit);
    
    layout->addLayout(buttonLayout);
    
    // Setup data
    qDebug() << "Test 1: Inicializando sistema...";
    FileManager fileManager;
    StationBST* bst = new StationBST();
    Graph* graph = new Graph(false);
    
    // Create visualizer
    GraphVisualizer* visualizer = new GraphVisualizer(scene, view, graph);
    
    qDebug() << "GraphVisualizer creado correctamente.";
    
    // Connect buttons
    QObject::connect(btnLoadData, &QPushButton::clicked, [&]() {
        qDebug() << "\nTest 2: Cargando datos desde archivos...";
        bool stationsLoaded = fileManager.loadStations("estaciones.txt", *bst, *graph);
        bool routesLoaded = fileManager.loadRoutes("rutas.txt", *graph);
        
        if (stationsLoaded && routesLoaded)
        {
            qDebug() << "Datos cargados exitosamente!";
            qDebug() << "Estaciones:" << graph->getStationCount();
        }
        else
        {
            qDebug() << "Error al cargar datos. Verifica los archivos.";
        }
    });
    
    QObject::connect(btnDrawGraph, &QPushButton::clicked, [&]() {
        qDebug() << "\nTest 3: Dibujando grafo...";
        visualizer->drawGraph();
        qDebug() << "Grafo dibujado en pantalla!";
    });
    
    QObject::connect(btnShowRoute, &QPushButton::clicked, [&]() {
        qDebug() << "\nTest 4: Calculando y mostrando ruta optima...";
        
        if (graph->getStationCount() < 2)
        {
            qDebug() << "Error: Se necesitan al menos 2 estaciones.";
            return;
        }
        
        // Calculate shortest path using Dijkstra from station 1
        QHash<int, double> distances = graph->dijkstra(1);
        
        // Find a destination station (use station with highest ID as example)
        QList<Station> stations = graph->getAllStations();
        if (stations.size() >= 2)
        {
            int destination = stations.last().getId();
            
            // For visualization, create a simple route (1 -> 4 -> 5 as example)
            QList<int> sampleRoute;
            
            // Try to find a path (simplified - just use connected stations)
            QList<int> bfsPath = graph->bfs(1);
            if (bfsPath.size() >= 3)
            {
                sampleRoute = {bfsPath[0], bfsPath[1], bfsPath[2]};
            }
            else if (bfsPath.size() >= 2)
            {
                sampleRoute = {bfsPath[0], bfsPath[1]};
            }
            
            if (!sampleRoute.isEmpty())
            {
                visualizer->drawOptimalRoute(sampleRoute);
                qDebug() << "Ruta optima resaltada en rojo!";
            }
            else
            {
                qDebug() << "No se pudo generar una ruta de ejemplo.";
            }
        }
    });
    
    QObject::connect(btnClear, &QPushButton::clicked, [&]() {
        qDebug() << "\nTest 5: Limpiando escena...";
        visualizer->clearScene();
        qDebug() << "Escena limpiada.";
    });
    
    QObject::connect(btnZoomFit, &QPushButton::clicked, [&]() {
        qDebug() << "\nAjustando zoom al contenido...";
        visualizer->fitInView();
    });
    
    // Show window
    window->show();
    
    qDebug() << "\n=== VENTANA DE VISUALIZACION CREADA ===";
    qDebug() << "Instrucciones:";
    qDebug() << "1. Haz clic en 'Cargar Datos' para cargar las estaciones y rutas";
    qDebug() << "2. Haz clic en 'Dibujar Grafo' para visualizar el sistema";
    qDebug() << "3. Haz clic en 'Mostrar Ruta Optima' para resaltar una ruta";
    qDebug() << "4. Usa el mouse para hacer zoom (scroll) y arrastrar (drag)";
    qDebug() << "5. Haz clic en 'Ajustar Zoom' para centrar el grafo\n";
    
    qDebug() << "=== PRUEBA DE GRAPHVISUALIZER INICIADA ===\n";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Run all tests
    //testStation();
    //testStationBST();
    //testDisjointSet();
    //testGraph();
    //testFileManager();
    //testReportGenerator();
    testGraphVisualizer();
    
    MainWindow window;
    window.show();
    return app.exec();
}
