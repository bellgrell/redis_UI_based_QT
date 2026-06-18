#include <QApplication>
#include <QPalette>
#include <QColor>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("mini-redis Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("mini-redis");

    // Global dark palette
    QPalette p;
    p.setColor(QPalette::Window, QColor("#1e1e2e"));
    p.setColor(QPalette::WindowText, QColor("#cdd6f4"));
    p.setColor(QPalette::Base, QColor("#11111b"));
    p.setColor(QPalette::AlternateBase, QColor("#1e1e2e"));
    p.setColor(QPalette::ToolTipBase, QColor("#313244"));
    p.setColor(QPalette::ToolTipText, QColor("#cdd6f4"));
    p.setColor(QPalette::Text, QColor("#cdd6f4"));
    p.setColor(QPalette::Button, QColor("#313244"));
    p.setColor(QPalette::ButtonText, QColor("#cdd6f4"));
    p.setColor(QPalette::BrightText, QColor("#f38ba8"));
    p.setColor(QPalette::Link, QColor("#89b4fa"));
    p.setColor(QPalette::Highlight, QColor("#89b4fa"));
    p.setColor(QPalette::HighlightedText, QColor("#1e1e2e"));
    app.setPalette(p);

    MainWindow window;
    window.show();
    return app.exec();
}
