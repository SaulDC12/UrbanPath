#pragma once

#include "Station.h"
#include <QString>
#include <QFile>
#include <QTextStream>

using namespace std;

// Forward declarations to avoid circular dependencies
class Graph;
class StationBST;

class FileManager
{
public:
    // Constructor
    FileManager() = default;
    
    // Load data from files
    bool loadStations(const QString& filename, StationBST& bst, Graph& graph);
    bool loadRoutes(const QString& filename, Graph& graph);
    bool loadClosures(const QString& filename, Graph& graph);
    
    // Save data to files
    bool saveStations(const QString& filename, const StationBST& bst);
    bool saveRoutes(const QString& filename, const Graph& graph);
    bool saveClosures(const QString& filename, const Graph& graph);
    bool exportReport(const QString& filename, const QString& content);
    
    // Utility methods
    bool fileExists(const QString& path) const;
    void clearFile(const QString& filename);
    QString getLastError() const;
    
private:
    QString lastError;
    
    // Helper methods for parsing
    QString trim(const QString& str) const;
    QStringList splitLine(const QString& line, const QString& separator) const;
    bool validateStationLine(const QStringList& parts) const;
    bool validateRouteLine(const QStringList& parts) const;
    
    // Find file in multiple locations
    QString findFile(const QString& filename) const;
};

