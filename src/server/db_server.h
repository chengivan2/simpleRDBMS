#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <memory>
#include "../core/query_executor.h"
#include "../core/table_manager.h"

class DatabaseServer : public QObject {
    Q_OBJECT

public:
    explicit DatabaseServer(std::shared_ptr<TableManager> tableManager, QObject* parent = nullptr);
    bool start(quint16 port = 8080);
    void stop();

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    void handleRequest(QTcpSocket* socket, const QString& method, const QString& path, const QByteArray& body);
    void sendResponse(QTcpSocket* socket, int statusCode, const QByteArray& contentType, const QByteArray& body);
    void sendCorsHeaders(QTcpSocket* socket);

    QTcpServer* tcpServer;
    std::unique_ptr<QueryExecutor> queryExecutor;
    std::shared_ptr<TableManager> tableManager;
};
