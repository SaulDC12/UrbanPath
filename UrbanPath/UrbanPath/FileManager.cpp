#include "FileManager.h"
#include "Graph.h"
#include "StationBST.h"
#include <QDebug>
#include <QDateTime>

// Check if file exists
bool FileManager::fileExists(const QString& path) const
{
    QString filePath = findFile(path);
    return !filePath.isEmpty();
}

// Clear file contents
void FileManager::clearFile(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        file.close();
        qDebug() << "Archivo" << filename << "limpiado correctamente.";
    }
    else
    {
        qDebug() << "Error: No se pudo limpiar el archivo" << filename;
    }
}

// Get last error message
QString FileManager::getLastError() const
{
    return lastError;
}

// Trim whitespace from string
QString FileManager::trim(const QString& str) const
{
    return str.trimmed();
}

// Split line by separator and trim each part
QStringList FileManager::splitLine(const QString& line, const QString& separator) const
{
    QStringList parts = line.split(separator);
    QStringList result;
    
    for (const QString& part : parts)
    {
        result.append(trim(part));
    }
    
    return result;
}

// Find file in multiple locations
QString FileManager::findFile(const QString& filename) const
{
    // List of possible locations to search
    QStringList searchPaths = {
        filename,                                    // Current directory
        QString("../") + filename,                   // Parent directory
        QString("../../") + filename,                // Two levels up
        QString("../../../") + filename,             // Three levels up
        QString("../../../../") + filename,          // Four levels up
        QString("data/") + filename,                 // data subdirectory
        QString("../data/") + filename,              // data in parent
        QString("../../data/") + filename            // data two levels up
    };
    
    // Search in all possible paths
    for (const QString& path : searchPaths)
    {
        QFile file(path);
        if (file.exists())
        {
            qDebug() << "Archivo encontrado en:" << path;
            return path;
        }
    }
    
    qDebug() << "Archivo" << filename << "no encontrado en ninguna ubicacion.";
    return QString(); // Return empty string if not found
}

// Validate station line format
bool FileManager::validateStationLine(const QStringList& parts) const
{
    if (parts.size() != 4)
    {
        return false;
    }
    
    // Check if ID is a valid integer
    bool okId;
    parts[0].toInt(&okId);
    if (!okId)
    {
        return false;
    }
    
    // Check if x and y are valid doubles
    bool okX, okY;
    parts[2].toDouble(&okX);
    parts[3].toDouble(&okY);
    
    return okX && okY;
}

// Validate route line format
bool FileManager::validateRouteLine(const QStringList& parts) const
{
    if (parts.size() != 3)
    {
        return false;
    }
    
    // Check if all values are valid
    bool ok1, ok2, ok3;
    parts[0].toInt(&ok1);
    parts[1].toInt(&ok2);
    parts[2].toDouble(&ok3);
    
    return ok1 && ok2 && ok3;
}

// Load stations from file
bool FileManager::loadStations(const QString& filename, StationBST& bst, Graph& graph)
{
    lastError.clear();

    // Find the file in multiple locations
    QString filePath = findFile(filename);
    
    if (filePath.isEmpty())
    {
        lastError = QString("El archivo %1 no existe en ninguna ubicacion conocida.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para lectura.").arg(filePath);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    int stationsLoaded = 0;
    
    qDebug() << "\nCargando estaciones desde" << filePath << "...";
    
    while (!in.atEnd())
    {
        QString line = in.readLine();
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.isEmpty() || line.startsWith("#") || line.startsWith("//"))
        {
            continue;
        }
        
        // Parse station data: id, name, x, y
        QStringList parts = splitLine(line, ",");
        
        if (!validateStationLine(parts))
        {
            qDebug() << "Advertencia: Linea" << lineNumber << "invalida (formato esperado: id, nombre, x, y). Ignorando...";
            continue;
        }
        
        // Extract values
        int id = parts[0].toInt();
        QString name = parts[1];
        double x = parts[2].toDouble();
        double y = parts[3].toDouble();
        
        // Create station
        Station station(id, name, x, y);
        
        // Add to BST and Graph
        bst.insert(station);
        graph.addStation(station);
        
        stationsLoaded++;
    }
    
    file.close();
    
    qDebug() << "Archivo" << filePath << "cargado correctamente. (" << stationsLoaded << "estaciones)";
    
    if (stationsLoaded == 0)
    {
        qDebug() << "Advertencia: El archivo de estaciones esta vacio.";
    }

    return true;
}

// Load routes from file
bool FileManager::loadRoutes(const QString& filename, Graph& graph)
{
    lastError.clear();

    // Find the file in multiple locations
    QString filePath = findFile(filename);
    
    if (filePath.isEmpty())
    {
        lastError = QString("El archivo %1 no existe en ninguna ubicacion conocida.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para lectura.").arg(filePath);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    int routesLoaded = 0;
    
    qDebug() << "\nCargando rutas desde" << filePath << "...";
    
    while (!in.atEnd())
    {
        QString line = in.readLine();
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.isEmpty() || line.startsWith("#") || line.startsWith("//"))
        {
            continue;
        }
        
        // Parse route data: origin, destination, weight
        QStringList parts = splitLine(line, ",");
        
        if (!validateRouteLine(parts))
        {
            qDebug() << "Advertencia: Linea" << lineNumber << "invalida (formato esperado: origen, destino, peso). Ignorando...";
            continue;
        }
        
        // Extract values
        int origin = parts[0].toInt();
        int destination = parts[1].toInt();
        double weight = parts[2].toDouble();
        
        // Check if stations exist
        if (!graph.containsStation(origin))
        {
            qDebug() << "Advertencia: Estacion origen" << origin << "no existe. Ignorando ruta...";
            continue;
        }
        
        if (!graph.containsStation(destination))
        {
            qDebug() << "Advertencia: Estacion destino" << destination << "no existe. Ignorando ruta...";
            continue;
        }
        
        // Add edge to graph
        graph.addEdge(origin, destination, weight);
        routesLoaded++;
    }
    
    file.close();
    
    qDebug() << "Archivo" << filePath << "cargado correctamente. (" << routesLoaded << "rutas)";
    
    if (routesLoaded == 0)
    {
        qDebug() << "Advertencia: El archivo de rutas esta vacio.";
    }

    return true;
}

// Load closures from file
bool FileManager::loadClosures(const QString& filename, Graph& graph)
{
    lastError.clear();

    // Find the file in multiple locations
    QString filePath = findFile(filename);
    
    if (filePath.isEmpty())
    {
        qDebug() << "Advertencia: El archivo" << filename << "no existe. No se aplicaran cierres.";
        return false;
    }
    
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para lectura.").arg(filePath);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    int closuresApplied = 0;
    
    qDebug() << "\nCargando cierres desde" << filePath << "...";
    
    // Clear previous closures before loading new ones
    graph.clearClosures();
    
    while (!in.atEnd())
    {
        QString line = in.readLine();
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.isEmpty() || line.startsWith("#") || line.startsWith("//"))
        {
            continue;
        }
        
        // Parse closure data: TIPO,id1[,id2]
        QStringList parts = splitLine(line, ",");
        
        if (parts.size() >= 2 && parts[0].toUpper() == "ESTACION")
        {
            // Station closure: ESTACION,id
            int stationId = parts[1].toInt();
            
            if (graph.containsStation(stationId))
            {
                graph.closeStation(stationId);
                qDebug() << "  Estacion" << stationId << "bloqueada (cerrada).";
                closuresApplied++;
            }
            else
            {
                qDebug() << "Advertencia: Estacion" << stationId << "no existe. Ignorando cierre...";
            }
        }
        else if (parts.size() >= 3 && parts[0].toUpper() == "RUTA")
        {
            // Route closure: RUTA,origen,destino
            int origin = parts[1].toInt();
            int destination = parts[2].toInt();
            
            if (graph.containsStation(origin) && graph.containsStation(destination))
            {
                graph.closeRoute(origin, destination);
                qDebug() << "  Ruta" << origin << "<->" << destination << "bloqueada (cerrada).";
                closuresApplied++;
            }
            else
            {
                qDebug() << "Advertencia: Ruta" << origin << "->" << destination << "(estaciones no existen). Ignorando cierre...";
            }
        }
        else
        {
            qDebug() << "Advertencia: Linea" << lineNumber << "invalida. Formato esperado: ESTACION,id o RUTA,origen,destino";
        }
    }
    
    file.close();
    
    qDebug() << "Archivo" << filePath << "procesado. (" << closuresApplied << "cierres aplicados)";
    return closuresApplied > 0;
}

// Save stations to file
bool FileManager::saveStations(const QString& filename, const StationBST& bst)
{
    lastError.clear();

    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para escritura.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "# Archivo de estaciones - UrbanPath\n";
    out << "# Formato: id, nombre, coordenada_x, coordenada_y\n";
    out << "# Generado: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n\n";
    
    // Get stations in order (InOrder traversal)
    QList<Station> stations = bst.inOrder();
    
    for (const Station& station : stations)
    {
        out << station.getId() << ", "
            << station.getName() << ", "
            << station.getX() << ", "
            << station.getY() << "\n";
    }
    
    file.close();
    
    qDebug() << "Estaciones guardadas en" << filename << "(" << stations.size() << "estaciones)";
    return true;
}

// Save routes to file
bool FileManager::saveRoutes(const QString& filename, const Graph& graph)
{
    lastError.clear();

    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para escritura.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "# Archivo de rutas - UrbanPath\n";
    out << "# Formato: origen, destino, peso\n";
    out << "# Generado: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n\n";
    
    // Get all stations
    QList<Station> stations = graph.getAllStations();
    int routeCount = 0;
    
    // Iterate through all stations and their neighbors
    for (const Station& station : stations)
    {
        int stationId = station.getId();
        QList<QPair<int, double>> neighbors = graph.getNeighbors(stationId);
        
        for (const auto& neighbor : neighbors)
        {
            int neighborId = neighbor.first;
            double weight = neighbor.second;
            
            // For undirected graphs, only write each edge once (origin < destination)
            if (stationId < neighborId)
            {
                out << stationId << ", "
                    << neighborId << ", "
                    << weight << "\n";
                routeCount++;
            }
        }
    }
    
    file.close();
    
    qDebug() << "Rutas guardadas en" << filename << "(" << routeCount << "rutas)";
    return true;
}

// Save closures to file
bool FileManager::saveClosures(const QString& filename, const Graph& graph)
{
    lastError.clear();

    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para escritura.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "# Archivo de cierres - UrbanPath\n";
    out << "# Formato: TIPO,identificador(es)\n";
    out << "# \n";
    out << "# ESTACION,id          -> Cierra una estacion (aparece en GRIS)\n";
    out << "# RUTA,origen,destino  -> Cierra una ruta (aparece en ROJO)\n";
    out << "#\n";
    out << "# Generado: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n\n";
    
    // Get closed stations and routes
    QSet<int> closedStations = graph.getClosedStations();
    QList<QPair<int, int>> closedRoutes = graph.getClosedRoutes();
    
    int closureCount = 0;
    
    // Write closed stations
    if (!closedStations.isEmpty())
    {
        out << "# Estaciones cerradas\n";
        for (int stationId : closedStations)
        {
            out << "ESTACION," << stationId << "\n";
            closureCount++;
        }
        out << "\n";
    }
    
    // Write closed routes
    if (!closedRoutes.isEmpty())
    {
        out << "# Rutas cerradas\n";
        for (const auto& route : closedRoutes)
        {
            out << "RUTA," << route.first << "," << route.second << "\n";
            closureCount++;
        }
    }
    
    file.close();
    
    qDebug() << "Cierres guardados en" << filename << "(" << closureCount << "cierres)";
    return true;
}

// Export report to file
bool FileManager::exportReport(const QString& filename, const QString& content)
{
    lastError.clear();

    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo crear el archivo de reporte %1.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header with timestamp
    out << "========================================\n";
    out << "  REPORTE DEL SISTEMA URBANPATH\n";
    out << "========================================\n";
    out << "Fecha: " << QDateTime::currentDateTime().toString("yyyy-MM-dd") << "\n";
    out << "Hora: " << QDateTime::currentDateTime().toString("HH:mm:ss") << "\n";
    out << "========================================\n\n";
    
    // Write content
    out << content;
    
    // Write footer
    out << "\n========================================\n";
    out << "  Fin del reporte\n";
    out << "========================================\n";
    
    file.close();
    
    qDebug() << "Reporte exportado exitosamente a:" << filename;
    return true;
}

// Load accidents from file
bool FileManager::loadAccidents(Graph& graph, const QString& filename)
{
    lastError.clear();
    
    // Try to find file in multiple locations
    QString filePath = findFile(filename);
    
    if (filePath.isEmpty())
    {
        // File not found, but this is not necessarily an error
        qDebug() << "[INFO] No se encontró archivo de accidentes:" << filename;
        return false;
    }
    
    qDebug() << "[INFO] Cargando accidentes desde:" << filePath;
    
    // Delegate to Graph's loadAccidents method
    bool result = graph.loadAccidents(filePath);
    
    if (!result)
    {
        lastError = QString("No se pudieron cargar accidentes desde %1").arg(filename);
    }
    
    return result;
}

// Save accidents to file
bool FileManager::saveAccidents(const QString& filename, const Graph& graph)
{
    lastError.clear();

    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para escritura.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "# Archivo de accidentes - UrbanPath\n";
    out << "# Formato: origen,destino,incremento_porcentual\n";
    out << "#\n";
    out << "# Cada linea representa un accidente que aumenta el peso de una ruta\n";
    out << "# origen,destino: IDs de las estaciones conectadas\n";
    out << "# incremento: porcentaje de aumento del peso (ej: 30 = +30%)\n";
    out << "#\n";
    out << "# IMPORTANTE: Los accidentes NO eliminan rutas, solo las hacen mas costosas\n";
    out << "# Para eliminar completamente una ruta, use el sistema de cierres\n";
    out << "#\n";
    out << "# Generado: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n\n";
    
    // Get affected routes
    QSet<QPair<int, int>> affectedRoutes = graph.getAffectedRoutes();
    
    if (affectedRoutes.isEmpty())
    {
        out << "# No hay accidentes activos\n";
        file.close();
        qDebug() << "Archivo de accidentes guardado (sin accidentes activos):" << filename;
        return true;
    }
    
    out << "# Accidentes activos\n";
    
    int accidentCount = 0;
    QSet<QPair<int, int>> processedRoutes;
    
    // Write accidents (avoid duplicates for undirected graphs)
    for (const auto& route : affectedRoutes)
    {
        int origin = route.first;
        int dest = route.second;
        
        // Avoid duplicate entries for undirected graphs
        QPair<int, int> reverseRoute(dest, origin);
        if (processedRoutes.contains(route) || processedRoutes.contains(reverseRoute))
        {
            continue;
        }
        
        processedRoutes.insert(route);
        
        // Calculate increment percentage from current and original weights
        double currentWeight = graph.getEdgeWeight(origin, dest);
        // Note: We can't retrieve the exact percentage used, so we calculate it
        // This is a limitation - in a future version, we could store the percentage
        // For now, we'll note this route has an accident but use a placeholder
        
        out << origin << "," << dest << ",30\n"; // Default 30% - user should adjust
        accidentCount++;
    }
    
    file.close();
    
    qDebug() << "Accidentes guardados en" << filename << "(" << accidentCount << "accidentes)";
    qDebug() << "NOTA: Los porcentajes se guardaron como 30% por defecto. Ajuste manualmente si es necesario.";
    
    return true;
}
