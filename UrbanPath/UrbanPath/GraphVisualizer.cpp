#include "GraphVisualizer.h"
#include <QDebug>
#include <QFont>

// Constructor
GraphVisualizer::GraphVisualizer(QGraphicsScene* scene, QGraphicsView* view, Graph* graph)
    : QObject(nullptr), scene(scene), view(view), graph(graph), backgroundItem(nullptr)
{
    // Initialize default visual settings
    nodeRadius = 8.0;
    normalNodeColor = QColor(70, 130, 180);      // Steel blue
    highlightNodeColor = QColor(255, 69, 0);     // Red-orange
    normalEdgeColor = QColor(150, 150, 150);     // Gray
    optimalEdgeColor = QColor(255, 0, 0);        // Red
    normalEdgeWidth = 2.0;
    optimalEdgeWidth = 4.0;
    
    // Configure view settings
    if (view)
    {
        view->setRenderHint(QPainter::Antialiasing);
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }
    
    qDebug() << "GraphVisualizer inicializado correctamente.";
}

// Destructor
GraphVisualizer::~GraphVisualizer()
{
    // Scene owns the items, so they will be deleted automatically
    // We just need to clear our references
    nodeItems.clear();
    edgeItems.clear();
    weightLabels.clear();
}

// Set click callback function
void GraphVisualizer::setClickCallback(function<void(double, double)> callback)
{
    clickCallback = callback;
    qDebug() << "Callback de clic en mapa configurado.";
}

// Install event filter to capture mouse clicks
void GraphVisualizer::installClickEvent()
{
    if (view && view->viewport())
    {
        view->viewport()->installEventFilter(this);
        qDebug() << "Event filter instalado en el viewport.";
    }
    else
    {
        qDebug() << "Error: No se pudo instalar event filter.";
    }
}

// Event filter to capture mouse clicks on the map
bool GraphVisualizer::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress && obj == view->viewport())
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        
        // Only handle left clicks
        if (mouseEvent->button() == Qt::LeftButton)
        {
            // Convert viewport coordinates to scene coordinates
            QPointF scenePos = view->mapToScene(mouseEvent->pos());
            
            qDebug() << "Clic en mapa detectado en:" << scenePos;
            
            // Call the callback function if set
            if (clickCallback)
            {
                clickCallback(scenePos.x(), scenePos.y());
            }
            
            return true;  // Event handled
        }
    }
    
    return QObject::eventFilter(obj, event);  // Pass event to parent
}

// Load background image
void GraphVisualizer::loadBackground(const QString& imagePath)
{
    if (!scene)
    {
        qDebug() << "Error: Escena no inicializada.";
        return;
    }
    
    QPixmap background(imagePath);
    
    if (background.isNull())
    {
        qDebug() << "Advertencia: No se pudo cargar la imagen de fondo:" << imagePath;
        qDebug() << "El grafo se dibujara sin fondo.";
        return;
    }
    
    // Remove old background if exists
    if (backgroundItem)
    {
        scene->removeItem(backgroundItem);
        delete backgroundItem;
    }
    
    // Add new background
    backgroundItem = scene->addPixmap(background);
    backgroundItem->setZValue(-1);  // Keep background behind everything
    
    // Adjust scene rect to background size
    scene->setSceneRect(background.rect());
    
    qDebug() << "Fondo cargado correctamente:" << imagePath;
    qDebug() << "Dimensiones:" << background.width() << "x" << background.height();
}

// Clear the entire scene
void GraphVisualizer::clearScene()
{
    if (!scene)
    {
        return;
    }
    
    // Clear all stored items
    nodeItems.clear();
    edgeItems.clear();
    weightLabels.clear();
    
    // Clear scene (keeps background if setZValue was used correctly)
    QPixmap savedBackground;
    if (backgroundItem)
    {
        savedBackground = backgroundItem->pixmap();
    }
    
    scene->clear();
    
    // Restore background
    if (!savedBackground.isNull())
    {
        backgroundItem = scene->addPixmap(savedBackground);
        backgroundItem->setZValue(-1);
    }
    else
    {
        backgroundItem = nullptr;
    }
}

// Draw the complete graph
void GraphVisualizer::drawGraph()
{
    if (!scene || !graph)
    {
        qDebug() << "Error: Escena o grafo no inicializados.";
        return;
    }
    
    clearScene();
    
    qDebug() << "\nDibujando grafo...";
    
    // Get all stations
    QList<Station> stations = graph->getAllStations();
    
    if (stations.isEmpty())
    {
        qDebug() << "Advertencia: No hay estaciones para dibujar.";
        return;
    }
    
    // First, draw all edges (behind nodes)
    int edgeCount = 0;
    QSet<QPair<int, int>> drawnEdges;
    
    for (const Station& station : stations)
    {
        int stationId = station.getId();
        QList<QPair<int, double>> neighbors = graph->getNeighbors(stationId);
        
        for (const auto& neighbor : neighbors)
        {
            int neighborId = neighbor.first;
            double weight = neighbor.second;
            
            // For undirected graphs, avoid drawing same edge twice
            QPair<int, int> edgeKey = makeEdgeKey(stationId, neighborId);
            
            if (!drawnEdges.contains(edgeKey))
            {
                drawEdge(stationId, neighborId, weight);
                drawnEdges.insert(edgeKey);
                edgeCount++;
            }
        }
    }
    
    // Then, draw all nodes (on top of edges)
    for (const Station& station : stations)
    {
        drawStationNode(station);
    }
    
    qDebug() << "Grafo dibujado:";
    qDebug() << "  - Estaciones:" << stations.size();
    qDebug() << "  - Rutas:" << edgeCount;
    
    // Fit graph in view
    fitInView();
}

// Draw a single station node
void GraphVisualizer::drawStationNode(const Station& station)
{
    if (!scene)
    {
        return;
    }
    
    double x = station.getX();
    double y = station.getY();
    int id = station.getId();
    
    // Create node circle
    QGraphicsEllipseItem* node = scene->addEllipse(
        x - nodeRadius, 
        y - nodeRadius, 
        nodeRadius * 2, 
        nodeRadius * 2,
        QPen(Qt::black, 1),
        QBrush(normalNodeColor)
    );
    
    node->setZValue(2);  // Above edges
    node->setFlag(QGraphicsItem::ItemIsSelectable);
    
    // Store node reference
    nodeItems[id] = node;
    
    // Create label with station name and ID
    QString label = QString("%1\n%2").arg(id).arg(station.getName());
    QGraphicsTextItem* text = scene->addText(label);
    
    QFont font = text->font();
    font.setPointSize(8);
    font.setBold(true);
    text->setFont(font);
    text->setDefaultTextColor(Qt::black);
    
    // Position text next to node
    text->setPos(x + nodeRadius + 2, y - nodeRadius);
    text->setZValue(3);  // Above everything
}

// Draw an edge between two stations
void GraphVisualizer::drawEdge(int fromId, int toId, double weight)
{
    QPen pen(normalEdgeColor, normalEdgeWidth);
    drawEdgeWithStyle(fromId, toId, weight, pen);
}

// Draw edge with custom style
void GraphVisualizer::drawEdgeWithStyle(int fromId, int toId, double weight, const QPen& pen)
{
    if (!scene || !graph)
    {
        return;
    }
    
    Station* fromStation = graph->getStation(fromId);
    Station* toStation = graph->getStation(toId);
    
    if (!fromStation || !toStation)
    {
        return;
    }
    
    double x1 = fromStation->getX();
    double y1 = fromStation->getY();
    double x2 = toStation->getX();
    double y2 = toStation->getY();
    
    // Create line
    QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, pen);
    line->setZValue(1);  // Below nodes, above background
    
    // Store edge reference
    QPair<int, int> edgeKey = makeEdgeKey(fromId, toId);
    edgeItems[edgeKey] = line;
    
    // Create weight label
    QString weightText = QString::number(weight, 'f', 1);
    QGraphicsTextItem* label = scene->addText(weightText);
    
    QFont font = label->font();
    font.setPointSize(7);
    label->setFont(font);
    label->setDefaultTextColor(Qt::darkBlue);
    
    // Position label at midpoint of edge
    double midX = (x1 + x2) / 2.0;
    double midY = (y1 + y2) / 2.0;
    label->setPos(midX, midY);
    label->setZValue(2);
    
    // Store label reference
    weightLabels[edgeKey] = label;
}

// Highlight optimal route
void GraphVisualizer::drawOptimalRoute(const QList<int>& route)
{
    if (route.size() < 2)
    {
        qDebug() << "Advertencia: La ruta debe tener al menos 2 estaciones.";
        return;
    }
    
    qDebug() << "\nResaltando ruta optima...";
    
    QPen optimalPen(optimalEdgeColor, optimalEdgeWidth);
    
    // Highlight edges in the route
    for (int i = 0; i < route.size() - 1; i++)
    {
        int from = route[i];
        int to = route[i + 1];
        
        // Remove old edge if exists
        QPair<int, int> edgeKey = makeEdgeKey(from, to);
        if (edgeItems.contains(edgeKey))
        {
            scene->removeItem(edgeItems[edgeKey]);
            delete edgeItems[edgeKey];
        }
        
        // Redraw with optimal style
        Station* fromStation = graph->getStation(from);
        Station* toStation = graph->getStation(to);
        
        if (fromStation && toStation)
        {
            double x1 = fromStation->getX();
            double y1 = fromStation->getY();
            double x2 = toStation->getX();
            double y2 = toStation->getY();
            
            QGraphicsLineItem* line = scene->addLine(x1, y1, x2, y2, optimalPen);
            line->setZValue(1);
            edgeItems[edgeKey] = line;
        }
        
        // Highlight nodes in route
        highlightStation(from);
    }
    
    // Highlight last node
    if (!route.isEmpty())
    {
        highlightStation(route.last());
    }
    
    qDebug() << "Ruta optima resaltada:" << route;
}

// Clear optimal route highlighting
void GraphVisualizer::clearOptimalRoute()
{
    // Redraw the entire graph to reset highlighting
    drawGraph();
}

// Highlight a specific station
void GraphVisualizer::highlightStation(int stationId)
{
    if (nodeItems.contains(stationId))
    {
        nodeItems[stationId]->setBrush(QBrush(highlightNodeColor));
    }
}

// Unhighlight a specific station
void GraphVisualizer::unhighlightStation(int stationId)
{
    if (nodeItems.contains(stationId))
    {
        nodeItems[stationId]->setBrush(QBrush(normalNodeColor));
    }
}

// Get station position
QPointF GraphVisualizer::getStationPosition(int stationId) const
{
    if (graph)
    {
        Station* station = graph->getStation(stationId);
        if (station)
        {
            return QPointF(station->getX(), station->getY());
        }
    }
    return QPointF(0, 0);
}

// Create edge key (always smaller ID first for undirected graphs)
QPair<int, int> GraphVisualizer::makeEdgeKey(int from, int to) const
{
    if (from < to)
    {
        return QPair<int, int>(from, to);
    }
    else
    {
        return QPair<int, int>(to, from);
    }
}

// Fit graph in view
void GraphVisualizer::fitInView()
{
    if (view && scene)
    {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        view->scale(0.9, 0.9);  // Slight zoom out for margins
    }
}

// Reset zoom to 1:1
void GraphVisualizer::resetZoom()
{
    if (view)
    {
        view->resetTransform();
    }
}

// Visual settings setters
void GraphVisualizer::setNodeRadius(double radius)
{
    nodeRadius = radius;
}

void GraphVisualizer::setNodeColor(const QColor& normal, const QColor& highlight)
{
    normalNodeColor = normal;
    highlightNodeColor = highlight;
}

void GraphVisualizer::setEdgeColor(const QColor& normal, const QColor& optimal)
{
    normalEdgeColor = normal;
    optimalEdgeColor = optimal;
}

void GraphVisualizer::setEdgeWidth(double normal, double optimal)
{
    normalEdgeWidth = normal;
    optimalEdgeWidth = optimal;
}
