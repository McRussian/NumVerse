#include <QApplication>
#include "ui/main_window.h"
#include "ui/theme.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet(Theme::defaultStyleSheet());

    MainWindow window;
    window.show();

    return app.exec();
}
