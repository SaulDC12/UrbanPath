#pragma once

#include <QString>
#include <QList>

using namespace std;

class Station
{
private:
    int id;
    QString name;
    double x;  // Coordinate X for map positioning
    double y;  // Coordinate Y for map positioning

public:
    // Constructors
    Station();
    Station(int id, const QString& name, double x, double y);
    
    // Copy constructor
    Station(const Station& other);
    
    // Getters
    int getId() const;
    QString getName() const;
    double getX() const;
    double getY() const;
    
    // Setters
    void setId(int id);
    void setName(const QString& name);
    void setX(double x);
    void setY(double y);
    
    // Utility methods
    QString toString() const;
    
    // Operators
    bool operator==(const Station& other) const;  // Compare by id
    bool operator<(const Station& other) const;   // Compare by id for ordering
    bool operator>(const Station& other) const;   // Compare by id for ordering
    
    // Assignment operator
    Station& operator=(const Station& other);
};

