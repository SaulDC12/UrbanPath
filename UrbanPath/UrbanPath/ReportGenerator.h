#pragma once

#include "FileManager.h"
#include <QString>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

using namespace std;

// Forward declarations
class Graph;
class StationBST;

class ReportGenerator
{
public:
    // Constructor
    ReportGenerator() = default;
    
    // Report generation methods
    bool generateRouteReport(const QString& filename, const QList<int>& route, const Graph& graph);
    bool generateTraversalReport(const QString& filename, const StationBST& bst);
    bool generateSystemStats(const QString& filename, const Graph& graph, const StationBST& bst);
    bool generateMSTReport(const QString& filename, const Graph& graph);
    bool generateConnectivityReport(const QString& filename, const Graph& graph);
    bool generateAccidentReport(const QString& filename, const Graph& graph);
    
    // Incremental report (append mode)
    bool appendToReport(const QString& filename, const QString& sectionTitle, const QString& content);
    
    // Get last error
    QString getLastError() const;
    
private:
    QString lastError;
    
    // Helper methods for formatting
    void writeHeader(QTextStream& out, const QString& title);
    void writeFooter(QTextStream& out);
    void writeSectionTitle(QTextStream& out, const QString& title);
    void writeSeparator(QTextStream& out);
    QString formatDateTime() const;
    QString formatStationInfo(int id, const QString& name, double x, double y) const;
};

