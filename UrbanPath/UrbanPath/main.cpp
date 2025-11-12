#include "MainWindow.h"
#include "Station.h"
#include "StationBST.h"
#include <QApplication>
#include <QDebug>

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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Run Station tests
    testStation();
    
    // Run StationBST tests
    testStationBST();
    
    MainWindow window;
    window.show();
    return app.exec();
}
