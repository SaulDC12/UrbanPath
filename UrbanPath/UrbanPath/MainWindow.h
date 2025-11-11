#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "Graph.h"
#include "StationBST.h"
#include "FileManager.h"
#include "ReportGenerator.h"
#include "GraphVisualizer.h"

using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindowClass ui;
};

