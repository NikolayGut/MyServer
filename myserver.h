#ifndef MYSERVER_H
#define MYSERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTextEdit>
#include <QTcpSocket>

namespace Ui {
class MyServer;
}
// =============Класс сервера============
class MyServer : public QWidget
{
    Q_OBJECT

public:
    explicit MyServer(int port, QWidget *parent = 0);
    ~MyServer();

private:
    Ui::MyServer *ui;
    QTcpServer *m_tcpServer;
    QTextEdit *m_txt;
    quint16 m_nextBlockSize;

    void sendToClient(QTcpSocket *pSocket, const QString &str);

public slots:
    virtual void slotNewConnection();
            void slotReadClient();
};

#endif // MYSERVER_H
