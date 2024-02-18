#include "myserver.h"
#include "myclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyServer server(2323);
    server.show();
    MyClient client("localhost", 2323);
    client.show();

    return a.exec();
}
