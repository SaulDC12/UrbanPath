#include "FileManager.h"
#include "Graph.h"
#include "StationBST.h"
#include <QDebug>
#include <QDateTime>

// Check if file exists
bool FileManager::fileExists(const QString& path) const
{
    QFile file(path);
    return file.exists();
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
    QFile file(filename);
    
    if (!file.exists())
    {
        lastError = QString("El archivo %1 no existe.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para lectura.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    int stationsLoaded = 0;
    
    qDebug() << "\nCargando estaciones desde" << filename << "...";
    
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
    
    qDebug() << "Archivo" << filename << "cargado correctamente. (" << stationsLoaded << "estaciones)";
    return stationsLoaded > 0;
}

// Load routes from file
bool FileManager::loadRoutes(const QString& filename, Graph& graph)
{
    QFile file(filename);
    
    if (!file.exists())
    {
        lastError = QString("El archivo %1 no existe.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para lectura.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    int routesLoaded = 0;
    
    qDebug() << "\nCargando rutas desde" << filename << "...";
    
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
    
    qDebug() << "Archivo" << filename << "cargado correctamente. (" << routesLoaded << "rutas)";
    return routesLoaded > 0;
}

// Load closures from file
bool FileManager::loadClosures(const QString& filename, Graph& graph)
{
    QFile file(filename);
    
    if (!file.exists())
    {
        qDebug() << "Advertencia: El archivo" << filename << "no existe. No se aplicaran cierres.";
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        lastError = QString("No se pudo abrir el archivo %1 para lectura.").arg(filename);
        qDebug() << "Error:" << lastError;
        return false;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    int closuresApplied = 0;
    
    qDebug() << "\nCargando cierres desde" << filename << "...";
    
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
        
        // Parse closure data
        QStringList parts = splitLine(line, ",");
        
        if (parts.size() == 1)
        {
            // Station closure: stationId
            int stationId = parts[0].toInt();
            
            if (graph.containsStation(stationId))
            {
                graph.removeStation(stationId);
                qDebug() << "  Estacion" << stationId << "cerrada.";
                closuresApplied++;
            }
            else
            {
                qDebug() << "Advertencia: Estacion" << stationId << "no existe. Ignorando cierre...";
            }
        }
        else if (parts.size() == 2)
        {
            // Route closure: origin, destination
            int origin = parts[0].toInt();
            int destination = parts[1].toInt();
            
            if (graph.hasEdge(origin, destination))
            {
                graph.removeEdge(origin, destination);
                qDebug() << "  Ruta" << origin << "->" << destination << "cerrada.";
                closuresApplied++;
            }
            else
            {
                qDebug() << "Advertencia: Ruta" << origin << "->" << destination << "no existe. Ignorando cierre...";
            }
        }
        else
        {
            qDebug() << "Advertencia: Linea" << lineNumber << "invalida. Formato esperado: estacionId o origen,destino";
        }
    }
    
    file.close();
    
    qDebug() << "Archivo" << filename << "procesado. (" << closuresApplied << "cierres aplicados)";
    return closuresApplied > 0;
}

// Save stations to file
bool FileManager::saveStations(const QString& filename, const StationBST& bst)
{
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

// Export report to file
bool FileManager::exportReport(const QString& filename, const QString& content)
{
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
