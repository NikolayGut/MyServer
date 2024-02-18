#include "myclient.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>

MyClient::MyClient(const QString &strHost, int nPort, QWidget *parent) : QWidget(parent),
    m_nNextBlockSize(0)
{
    m_pTcpSocket = new QTcpSocket(this);

    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    m_ptxtInfo = new QTextEdit;
    m_ptxtInput = new QLineEdit;

    m_ptxtInfo->setReadOnly(true);

    QPushButton *pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendtoServer()));
    connect(m_ptxtInput, SIGNAL(returnPressed()), this, SLOT(slotSendtoServer()));

    //layout setup
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("<H1>Client</H1>"));
    layout->addWidget(m_ptxtInfo);
    layout->addWidget(m_ptxtInput);
    layout->addWidget(pcmd);
    this->setLayout(layout);
}

void MyClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_6);
    for(;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }
        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime time;
        QString str;
        in >> time >> str;

        m_ptxtInfo->append(time.toString() + " " + str);
    }
}

void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError = "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                    "The host was not found." :
                                    err == QAbstractSocket::ConnectionRefusedError ?
                                    "The connection was refused." :
                                    QString(m_pTcpSocket->errorString())
                                    );
    m_ptxtInfo->append(strError);
}

void MyClient::slotSendtoServer()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << quint16(0) << QTime::currentTime() << m_ptxtInput->text();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    m_ptxtInput->setText("");
}

void MyClient::slotConnected()
{
    m_ptxtInfo->append("Received the connected() signal");
}
