#include "MainWindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application information
    app.setApplicationName("UrbanPath");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Estructura de Datos");
    
    qDebug() << "=== Iniciando Sistema UrbanPath ===";
    qDebug() << "Version: 1.0";
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "";
    
    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    qDebug() << "Ventana principal cargada exitosamente.";
    qDebug() << "Sistema listo para operar.\n";
    
    return app.exec();
}
