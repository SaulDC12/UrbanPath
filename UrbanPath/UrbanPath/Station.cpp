#include "Station.h"

// Default constructor
Station::Station() : id(-1), name(""), x(0.0), y(0.0)
{
}

// Parameterized constructor
Station::Station(int id, const QString& name, double x, double y)
    : id(id), name(name), x(x), y(y)
{
}

// Copy constructor
Station::Station(const Station& other)
    : id(other.id), name(other.name), x(other.x), y(other.y)
{
}

// Getters
int Station::getId() const
{
    return id;
}

QString Station::getName() const
{
    return name;
}

double Station::getX() const
{
    return x;
}

double Station::getY() const
{
    return y;
}

// Setters
void Station::setId(int id)
{
    this->id = id;
}

void Station::setName(const QString& name)
{
    this->name = name;
}

void Station::setX(double x)
{
    this->x = x;
}

void Station::setY(double y)
{
    this->y = y;
}

// Utility methods
QString Station::toString() const
{
    return QString("Station[ID=%1, Name=%2, X=%3, Y=%4]")
        .arg(id)
        .arg(name)
        .arg(x)
        .arg(y);
}

// Operators
bool Station::operator==(const Station& other) const
{
    return this->id == other.id;
}

bool Station::operator<(const Station& other) const
{
    return this->id < other.id;
}

bool Station::operator>(const Station& other) const
{
    return this->id > other.id;
}

// Assignment operator
Station& Station::operator=(const Station& other)
{
    if (this != &other)
    {
        this->id = other.id;
        this->name = other.name;
        this->x = other.x;
        this->y = other.y;
    }
    return *this;
}
