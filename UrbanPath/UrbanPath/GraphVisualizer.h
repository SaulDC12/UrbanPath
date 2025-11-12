#pragma once

#include "Graph.h"
#include "Station.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QPen>
#include <QBrush>
#include <QMap>
#include <QList>
#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <functional>

using namespace std;

class GraphVisualizer : public QObject
{
private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    Graph* graph;
    
    // Store visual elements for updates
    QMap<int, QGraphicsEllipseItem*> nodeItems;
    QMap<QPair<int, int>, QGraphicsLineItem*> edgeItems;
    QMap<QPair<int, int>, QGraphicsTextItem*> weightLabels;
    QGraphicsPixmapItem* backgroundItem;
    
    // Visual settings
    double nodeRadius;
    QColor normalNodeColor;
    QColor highlightNodeColor;
    QColor normalEdgeColor;
    QColor optimalEdgeColor;
    double normalEdgeWidth;
    double optimalEdgeWidth;
    
    // Callback function for map clicks
    function<void(double, double)> clickCallback;
    
public:
    // Constructor
    explicit GraphVisualizer(QGraphicsScene* scene, QGraphicsView* view, Graph* graph);
    
    // Destructor
    ~GraphVisualizer();
    
    // Main visualization methods
    void loadBackground(const QString& imagePath);
    void drawGraph();
    void clearScene();
    
    // Route highlighting
    void drawOptimalRoute(const QList<int>& route);
    void clearOptimalRoute();
    
    // Individual drawing methods
    void highlightStation(int stationId);
    void unhighlightStation(int stationId);
    
    // Visual settings
    void setNodeRadius(double radius);
    void setNodeColor(const QColor& normal, const QColor& highlight);
    void setEdgeColor(const QColor& normal, const QColor& optimal);
    void setEdgeWidth(double normal, double optimal);
    
    // Utility
    void fitInView();
    void resetZoom();
    
    // Map click callback setup
    void setClickCallback(function<void(double, double)> callback);
    void installClickEvent();

protected:
    // Event filter for capturing mouse clicks
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    // Helper methods for drawing
    void drawStationNode(const Station& station);
    void drawEdge(int fromId, int toId, double weight);
    void drawEdgeWithStyle(int fromId, int toId, double weight, const QPen& pen);
    
    // Coordinate conversion
    QPointF getStationPosition(int stationId) const;
    
    // Edge key generation
    QPair<int, int> makeEdgeKey(int from, int to) const;
};

