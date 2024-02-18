#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>

class MyClient : public QWidget
{
    Q_OBJECT
public:
    explicit MyClient(const QString &strHost, int nPort, QWidget *parent = 0);

private:
    QTcpSocket *m_pTcpSocket;
    QTextEdit *m_ptxtInfo;
    QLineEdit *m_ptxtInput;
    quint16 m_nNextBlockSize;

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError err);
    void slotSendtoServer();
    void slotConnected();

};

#endif // MYCLIENT_H
