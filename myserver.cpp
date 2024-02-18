#include "myserver.h"
#include "ui_myserver.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTime>
#include <QLabel>

MyServer::MyServer(int port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyServer),
    m_nextBlockSize(0)
{
    ui->setupUi(this);
    m_tcpServer = new QTcpServer(this);
    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:" + m_tcpServer->errorString());
        m_tcpServer->close();
        return;
    }
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    m_txt = new QTextEdit;
    m_txt->setReadOnly(true);

    //Layout setup
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("<H1>Server</H1>"));
    layout->addWidget(m_txt);
    setLayout(layout);
}

MyServer::~MyServer()
{
    delete ui;
}

void MyServer::sendToClient(QTcpSocket *pSocket, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

/*virtual*/ void MyServer::slotNewConnection()
{
    QTcpSocket *pClientSocket = m_tcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), SLOT(slotReadClient()));

    sendToClient(pClientSocket, "Server Response: Connected!");
}

void MyServer::slotReadClient()
{
    QTcpSocket *clientSocket = (QTcpSocket*) sender();
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_4_6);
    for (;;) {
        if (!m_nextBlockSize) {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nextBlockSize;
        }
        if (clientSocket->bytesAvailable() < m_nextBlockSize) {
            break;
        }
        QTime time;
        QString str;
        in >> time >> str;

        QString strMessage = time.toString() + " " + "Client has sent - " + str;
        m_txt->append(strMessage);
        m_nextBlockSize = 0;
        sendToClient(clientSocket, "Server Response: Received \"" + str + "\"");
    }
}
