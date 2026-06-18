#include "redisclient.h"
#include <QDebug>

RedisClient::RedisClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_host("127.0.0.1")
    , m_port(6379)
    , m_state(Disconnected)
    , m_pingTimer(new QTimer(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &RedisClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &RedisClient::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &RedisClient::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &RedisClient::onReadyRead);

    // Ping every 30 seconds to keep connection alive
    connect(m_pingTimer, &QTimer::timeout, this, [this]() {
        if (m_state == Connected) {
            m_socket->write(RespParser::encodeCommand({"PING"}));
        }
    });
}

RedisClient::~RedisClient()
{
    disconnectFromServer();
}

void RedisClient::connectToServer(const QString &host, int port)
{
    m_host = host;
    m_port = port;
    m_state = Connecting;
    m_buffer.clear();
    emit stateChanged(m_state);
    m_socket->connectToHost(host, port);
}

void RedisClient::disconnectFromServer()
{
    m_pingTimer->stop();
    if (m_socket->state() != QTcpSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QTcpSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
    }
    m_state = Disconnected;
    emit stateChanged(m_state);
}

void RedisClient::sendCommand(const QStringList &parts)
{
    if (m_state != Connected) {
        QVariantMap errMap;
        errMap["error"] = "Not connected to Redis server";
        emit responseReceived(errMap, "(error) Not connected to Redis server");
        return;
    }
    QByteArray data = RespParser::encodeCommand(parts);
    m_socket->write(data);
}

void RedisClient::sendRaw(const QString &commandLine)
{
    QStringList parts = RespParser::parseCommandLine(commandLine);
    if (parts.isEmpty()) return;
    sendCommand(parts);
}

void RedisClient::onConnected()
{
    m_state = Connected;
    m_buffer.clear();
    emit connected(m_host, m_port);
    emit stateChanged(m_state);
    m_pingTimer->start(30000);
}

void RedisClient::onDisconnected()
{
    m_pingTimer->stop();
    m_state = Disconnected;
    emit disconnected("Connection closed");
    emit stateChanged(m_state);
}

void RedisClient::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    m_state = Disconnected;
    QString errMsg = m_socket->errorString();
    emit connectionError(errMsg);
    emit stateChanged(m_state);
}

void RedisClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    int consumed = 0;
    QList<QVariant> results = RespParser::parseResponse(m_buffer, consumed);

    if (consumed > 0) {
        m_buffer = m_buffer.mid(consumed);
        for (const QVariant &result : results) {
            QString formatted = RespParser::formatValue(result);
            emit responseReceived(result, formatted);
        }
    }
}
