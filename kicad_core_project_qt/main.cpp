/*
 * Qt-based main entry point for qt_pcb_project
 * This creates a minimal Qt application
 */

#include <QApplication>
#include <QMainWindow>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow window;
    window.setWindowTitle("Qt PCB Project");
    window.resize(800, 600);
    
    QLabel* label = new QLabel("Qt PCB Project - Build Successful!", &window);
    label->setAlignment(Qt::AlignCenter);
    window.setCentralWidget(label);
    
    window.show();
    
    return app.exec();
}
