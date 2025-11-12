#include "ReportGenerator.h"
#include "Graph.h"
#include "StationBST.h"
#include <QDebug>

// Get last error message
QString ReportGenerator::getLastError() const
{
    return lastError;
}

// Format current date and time
QString ReportGenerator::formatDateTime() const
{
    QDateTime now = QDateTime::currentDateTime();
    return now.toString("yyyy-MM-dd HH:mm:ss");
}

// Format station information
QString ReportGenerator::formatStationInfo(int id, const QString& name, double x, double y) const
{
    return QString("Estacion %1: %2 (X: %3, Y: %4)")
        .arg(id)
        .arg(name)
        .arg(x, 0, 'f', 1)
        .arg(y, 0, 'f', 1);
}

// Write report header
void ReportGenerator::writeHeader(QTextStream& out, const QString& title)
{
    out << "========================================\n";
    out << "  " << title << "\n";
    out << "========================================\n";
    out << "Fecha: " << QDateTime::currentDateTime().toString("yyyy-MM-dd") << "\n";
    out << "Hora: " << QDateTime::currentDateTime().toString("HH:mm:ss") << "\n";
    out << "========================================\n\n";
}

// Write report footer
void ReportGenerator::writeFooter(QTextStream& out)
{
    out << "\n========================================\n";
    out << "  Fin del reporte\n";
    out << "========================================\n";
}

// Write section title
void ReportGenerator::writeSectionTitle(QTextStream& out, const QString& title)
{
    out << "\n" << title << "\n";
    out << QString(title.length(), '-') << "\n";
}

// Write separator line
void ReportGenerator::writeSeparator(QTextStream& out)
{
    out << "----------------------------------------\n";
}

// Generate route report (shortest path or specific route)
bool ReportGenerator::generateRouteReport(const QString& filename, const QList<int>& route, const Graph& graph)
{
    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo crear el archivo %1.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    writeHeader(out, "REPORTE DE RUTA");
    
    // Route information
    out << "INFORMACION DE LA RUTA\n";
    writeSeparator(out);
    out << "Numero de estaciones en la ruta: " << route.size() << "\n";
    
    if (route.isEmpty())
    {
        out << "\nNo hay ruta disponible.\n";
        writeFooter(out);
        file.close();
        qDebug() << "Reporte de ruta generado:" << filename;
        return true;
    }
    
    out << "\nRecorrido:\n";
    
    double totalDistance = 0.0;
    
    for (int i = 0; i < route.size(); i++)
    {
        int stationId = route[i];
        Station* station = const_cast<Graph&>(graph).getStation(stationId);
        
        if (station != nullptr)
        {
            out << QString("  %1. %2\n")
                .arg(i + 1)
                .arg(formatStationInfo(station->getId(), station->getName(), 
                                      station->getX(), station->getY()));
        }
        else
        {
            out << QString("  %1. Estacion %2 (informacion no disponible)\n")
                .arg(i + 1)
                .arg(stationId);
        }
        
        // Calculate distance to next station
        if (i < route.size() - 1)
        {
            int nextId = route[i + 1];
            double weight = graph.getEdgeWeight(stationId, nextId);
            
            if (weight < std::numeric_limits<double>::infinity())
            {
                totalDistance += weight;
                out << QString("     -> Distancia al siguiente: %1\n").arg(weight, 0, 'f', 1);
            }
            else
            {
                out << "     -> No hay conexion directa\n";
            }
        }
    }
    
    writeSeparator(out);
    out << QString("Distancia total de la ruta: %1\n").arg(totalDistance, 0, 'f', 1);
    
    // Write footer
    writeFooter(out);
    
    file.close();
    
    qDebug() << "Reporte de ruta generado exitosamente:" << filename;
    return true;
}

// Generate traversal report (InOrder, PreOrder, PostOrder)
bool ReportGenerator::generateTraversalReport(const QString& filename, const StationBST& bst)
{
    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo crear el archivo %1.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    writeHeader(out, "REPORTE DE RECORRIDOS DEL ARBOL BST");
    
    // InOrder traversal
    writeSectionTitle(out, "RECORRIDO IN-ORDER (Izquierda - Raiz - Derecha)");
    out << "Orden: Ascendente por ID de estacion\n\n";
    
    QList<Station> inOrderList = bst.inOrder();
    for (int i = 0; i < inOrderList.size(); i++)
    {
        const Station& station = inOrderList[i];
        out << QString("  %1. %2\n")
            .arg(i + 1)
            .arg(formatStationInfo(station.getId(), station.getName(), 
                                  station.getX(), station.getY()));
    }
    
    out << QString("\nTotal de estaciones (InOrder): %1\n").arg(inOrderList.size());
    
    // PreOrder traversal
    writeSectionTitle(out, "RECORRIDO PRE-ORDER (Raiz - Izquierda - Derecha)");
    out << "Orden: Visita raiz primero, luego subarboles\n\n";
    
    QList<Station> preOrderList = bst.preOrder();
    for (int i = 0; i < preOrderList.size(); i++)
    {
        const Station& station = preOrderList[i];
        out << QString("  %1. %2\n")
            .arg(i + 1)
            .arg(formatStationInfo(station.getId(), station.getName(), 
                                  station.getX(), station.getY()));
    }
    
    out << QString("\nTotal de estaciones (PreOrder): %1\n").arg(preOrderList.size());
    
    // PostOrder traversal
    writeSectionTitle(out, "RECORRIDO POST-ORDER (Izquierda - Derecha - Raiz)");
    out << "Orden: Visita subarboles primero, luego raiz\n\n";
    
    QList<Station> postOrderList = bst.postOrder();
    for (int i = 0; i < postOrderList.size(); i++)
    {
        const Station& station = postOrderList[i];
        out << QString("  %1. %2\n")
            .arg(i + 1)
            .arg(formatStationInfo(station.getId(), station.getName(), 
                                  station.getX(), station.getY()));
    }
    
    out << QString("\nTotal de estaciones (PostOrder): %1\n").arg(postOrderList.size());
    
    // Write footer
    writeFooter(out);
    
    file.close();
    
    qDebug() << "Reporte de recorridos generado exitosamente:" << filename;
    return true;
}

// Generate system statistics report
bool ReportGenerator::generateSystemStats(const QString& filename, const Graph& graph, const StationBST& bst)
{
    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo crear el archivo %1.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    writeHeader(out, "ESTADISTICAS DEL SISTEMA URBANPATH");
    
    // General statistics
    writeSectionTitle(out, "ESTADISTICAS GENERALES");
    
    int totalStations = graph.getStationCount();
    int totalStationsBST = bst.count();
    
    out << QString("Total de estaciones en el grafo: %1\n").arg(totalStations);
    out << QString("Total de estaciones en el BST: %1\n").arg(totalStationsBST);
    
    // Count total routes
    int totalRoutes = 0;
    QList<Station> stations = graph.getAllStations();
    for (const Station& station : stations)
    {
        QList<QPair<int, double>> neighbors = graph.getNeighbors(station.getId());
        totalRoutes += neighbors.size();
    }
    // For undirected graphs, divide by 2
    totalRoutes /= 2;
    
    out << QString("Total de rutas (conexiones): %1\n").arg(totalRoutes);
    out << QString("Tipo de grafo: No dirigido\n");
    
    // Calculate average connectivity
    double avgConnectivity = totalStations > 0 ? 
        (2.0 * totalRoutes) / totalStations : 0.0;
    out << QString("Conectividad promedio por estacion: %1\n")
        .arg(avgConnectivity, 0, 'f', 2);
    
    // Minimum Spanning Tree statistics
    writeSectionTitle(out, "ARBOL DE EXPANSION MINIMA (MST)");
    
    if (totalStations > 0)
    {
        QList<QPair<int, int>> mstEdges = const_cast<Graph&>(graph).kruskalMST();
        double mstWeight = 0.0;
        
        out << "Aristas del MST:\n";
        for (const auto& edge : mstEdges)
        {
            double weight = graph.getEdgeWeight(edge.first, edge.second);
            mstWeight += weight;
            out << QString("  Estacion %1 <-> Estacion %2: %3\n")
                .arg(edge.first)
                .arg(edge.second)
                .arg(weight, 0, 'f', 1);
        }
        
        out << QString("\nTotal de aristas en MST: %1\n").arg(mstEdges.size());
        out << QString("Peso total del MST: %1\n").arg(mstWeight, 0, 'f', 1);
        
        if (totalStations > 1)
        {
            double avgMSTWeight = mstWeight / mstEdges.size();
            out << QString("Peso promedio por arista: %1\n")
                .arg(avgMSTWeight, 0, 'f', 2);
        }
    }
    else
    {
        out << "No hay estaciones en el sistema.\n";
    }
    
    // Station list
    writeSectionTitle(out, "LISTADO DE ESTACIONES");
    
    QList<Station> allStations = graph.getAllStations();
    for (int i = 0; i < allStations.size(); i++)
    {
        const Station& station = allStations[i];
        out << QString("  %1. %2\n")
            .arg(i + 1)
            .arg(formatStationInfo(station.getId(), station.getName(), 
                                  station.getX(), station.getY()));
        
        // Show connections
        QList<QPair<int, double>> neighbors = graph.getNeighbors(station.getId());
        out << QString("     Conexiones: %1\n").arg(neighbors.size());
    }
    
    // Write footer
    writeFooter(out);
    
    file.close();
    
    qDebug() << "Reporte de estadisticas generado exitosamente:" << filename;
    return true;
}

// Generate MST-specific report
bool ReportGenerator::generateMSTReport(const QString& filename, const Graph& graph)
{
    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo crear el archivo %1.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    writeHeader(out, "REPORTE DE ARBOL DE EXPANSION MINIMA");
    
    // Kruskal MST
    writeSectionTitle(out, "ALGORITMO DE KRUSKAL");
    
    QList<QPair<int, int>> kruskalEdges = const_cast<Graph&>(graph).kruskalMST();
    double kruskalWeight = 0.0;
    
    out << "Aristas seleccionadas (ordenadas por peso):\n";
    for (int i = 0; i < kruskalEdges.size(); i++)
    {
        const auto& edge = kruskalEdges[i];
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        kruskalWeight += weight;
        
        out << QString("  %1. (%2, %3) - Peso: %4\n")
            .arg(i + 1)
            .arg(edge.first)
            .arg(edge.second)
            .arg(weight, 0, 'f', 1);
    }
    
    out << QString("\nPeso total (Kruskal): %1\n").arg(kruskalWeight, 0, 'f', 1);
    out << QString("Total de aristas: %1\n").arg(kruskalEdges.size());
    
    // Prim MST
    writeSectionTitle(out, "ALGORITMO DE PRIM");
    
    QList<QPair<int, int>> primEdges = const_cast<Graph&>(graph).primMST();
    double primWeight = 0.0;
    
    out << "Aristas seleccionadas (orden de construccion):\n";
    for (int i = 0; i < primEdges.size(); i++)
    {
        const auto& edge = primEdges[i];
        double weight = graph.getEdgeWeight(edge.first, edge.second);
        primWeight += weight;
        
        out << QString("  %1. (%2, %3) - Peso: %4\n")
            .arg(i + 1)
            .arg(edge.first)
            .arg(edge.second)
            .arg(weight, 0, 'f', 1);
    }
    
    out << QString("\nPeso total (Prim): %1\n").arg(primWeight, 0, 'f', 1);
    out << QString("Total de aristas: %1\n").arg(primEdges.size());
    
    // Comparison
    writeSectionTitle(out, "COMPARACION DE ALGORITMOS");
    out << QString("Peso Kruskal: %1\n").arg(kruskalWeight, 0, 'f', 1);
    out << QString("Peso Prim: %1\n").arg(primWeight, 0, 'f', 1);
    out << QString("Diferencia: %1\n").arg(qAbs(kruskalWeight - primWeight), 0, 'f', 4);
    out << "Nota: Ambos algoritmos deben producir el mismo peso total.\n";
    
    // Write footer
    writeFooter(out);
    
    file.close();
    
    qDebug() << "Reporte de MST generado exitosamente:" << filename;
    return true;
}

// Generate connectivity report
bool ReportGenerator::generateConnectivityReport(const QString& filename, const Graph& graph)
{
    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo crear el archivo %1.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    writeHeader(out, "REPORTE DE CONECTIVIDAD DEL SISTEMA");
    
    QList<Station> stations = graph.getAllStations();
    
    if (stations.isEmpty())
    {
        out << "No hay estaciones en el sistema.\n";
        writeFooter(out);
        file.close();
        return true;
    }
    
    // Connectivity matrix
    writeSectionTitle(out, "ANALISIS DE CONECTIVIDAD");
    
    out << "Estaciones en el sistema: " << stations.size() << "\n\n";
    
    // For each station, show its connections
    for (const Station& station : stations)
    {
        int stationId = station.getId();
        QList<QPair<int, double>> neighbors = graph.getNeighbors(stationId);
        
        out << QString("Estacion %1 (%2):\n")
            .arg(stationId)
            .arg(station.getName());
        
        if (neighbors.isEmpty())
        {
            out << "  Sin conexiones directas\n";
        }
        else
        {
            out << QString("  Conexiones directas: %1\n").arg(neighbors.size());
            for (const auto& neighbor : neighbors)
            {
                out << QString("    -> Estacion %1 (peso: %2)\n")
                    .arg(neighbor.first)
                    .arg(neighbor.second, 0, 'f', 1);
            }
        }
        out << "\n";
    }
    
    // BFS from first station
    if (!stations.isEmpty())
    {
        int startId = stations.first().getId();
        writeSectionTitle(out, "PRUEBA DE ALCANZABILIDAD (BFS)");
        out << QString("Inicio desde estacion %1:\n\n").arg(startId);
        
        QList<int> bfsResult = const_cast<Graph&>(graph).bfs(startId);
        out << "Estaciones alcanzables:\n";
        for (int i = 0; i < bfsResult.size(); i++)
        {
            out << QString("  %1. Estacion %2\n")
                .arg(i + 1)
                .arg(bfsResult[i]);
        }
        
        out << QString("\nTotal alcanzable: %1 de %2\n")
            .arg(bfsResult.size())
            .arg(stations.size());
        
        if (bfsResult.size() == stations.size())
        {
            out << "Conclusion: El grafo esta completamente conectado.\n";
        }
        else
        {
            out << "Conclusion: El grafo NO esta completamente conectado.\n";
            out << "Existen estaciones aisladas o componentes desconectadas.\n";
        }
    }
    
    // Write footer
    writeFooter(out);
    
    file.close();
    
    qDebug() << "Reporte de conectividad generado exitosamente:" << filename;
    return true;
}

// Append to existing report
bool ReportGenerator::appendToReport(const QString& filename, const QString& sectionTitle, const QString& content)
{
    QFile file(filename);
    
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para agregar contenido.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write section
    out << "\n";
    writeSeparator(out);
    writeSectionTitle(out, sectionTitle);
    out << "Agregado: " << formatDateTime() << "\n\n";
    out << content;
    out << "\n";
    writeSeparator(out);
    
    file.close();
    
    qDebug() << "Seccion agregada al reporte:" << filename;
    return true;
}
