#ifndef REDISCLIENT_H
#define REDISCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include "resparser.h"

/**
 * TCP client that connects to the mini-redis C++ server
 * and communicates via RESP protocol.
 */
class RedisClient : public QObject
{
    Q_OBJECT

public:
    explicit RedisClient(QObject *parent = nullptr);
    ~RedisClient();

    enum State { Disconnected, Connecting, Connected };

    State state() const { return m_state; }
    QString host() const { return m_host; }
    int port() const { return m_port; }

public slots:
    void connectToServer(const QString &host, int port);
    void disconnectFromServer();
    void sendCommand(const QStringList &parts);
    void sendRaw(const QString &commandLine);

signals:
    void connected(const QString &host, int port);
    void disconnected(const QString &reason);
    void connectionError(const QString &message);
    void responseReceived(const QVariant &parsed, const QString &formatted);
    void stateChanged(RedisClient::State state);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    QString m_host;
    int m_port;
    State m_state;
    QTimer *m_pingTimer;
};

#endif // REDISCLIENT_H
