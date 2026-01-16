#include "db_server.h"
#include "../utils/logger.h"
#include "../parser/lexer.h"
#include "../parser/parser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

DatabaseServer::DatabaseServer(std::shared_ptr<TableManager> tableManager, QObject* parent)
    : QObject(parent), tableManager(tableManager), queryExecutor(std::make_unique<QueryExecutor>()) {
    queryExecutor->setTableManager(tableManager);
}

bool DatabaseServer::start(quint16 port) {
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &DatabaseServer::onNewConnection);

    if (!tcpServer->listen(QHostAddress::Any, port)) {
        Logger::instance().error(QString("Server failed to start: %1").arg(tcpServer->errorString()));
        return false;
    }

    Logger::instance().info(QString("Server listening on port %1").arg(port));
    return true;
}

void DatabaseServer::stop() {
    if (tcpServer->isListening()) {
        tcpServer->close();
        Logger::instance().info("Server stopped");
    }
}

void DatabaseServer::onNewConnection() {
    QTcpSocket* socket = tcpServer->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &DatabaseServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void DatabaseServer::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    // Logger::instance().info("TRACE: onReadyRead entered"); // Verbose debug

    try {
        // Read all available data
        QByteArray data = socket->readAll();
        if (data.isEmpty()) return; // Don't process empty reads

        QString request = QString::fromUtf8(data);
        
        // Simple HTTP parsing
        QStringList lines = request.split("\r\n");
        if (lines.isEmpty()) return;

        QString requestLine = lines[0];
        QStringList parts = requestLine.split(" ");
        if (parts.size() < 2) return;

        QString method = parts[0];
        QString path = parts[1];

        // Find body (after double CRLF)
        int bodyIndex = request.indexOf("\r\n\r\n");
        QByteArray body;
        if (bodyIndex != -1) {
            body = data.mid(bodyIndex + 4);
        }

        handleRequest(socket, method, path, body);
    } catch (const std::exception& e) {
        Logger::instance().error(QString("Exception in onReadyRead: %1").arg(e.what()));
    } catch (...) {
        Logger::instance().error("Unknown exception in onReadyRead");
    }
}

void DatabaseServer::handleRequest(QTcpSocket* socket, const QString& method, const QString& path, const QByteArray& body) {
    Logger::instance().info(QString("Request: %1 %2").arg(method).arg(path));

    // CORS Preflight
    if (method == "OPTIONS") {
        sendResponse(socket, 204, "text/plain", "");
        return;
    }

    if (path == "/query" && method == "POST") {
        QJsonDocument doc = QJsonDocument::fromJson(body);
        if (!doc.isObject()) {
            sendResponse(socket, 400, "application/json", "{\"error\": \"Invalid JSON\"}");
            return;
        }

        QString sql = doc.object().value("sql").toString();
        if (sql.isEmpty()) {
            sendResponse(socket, 400, "application/json", "{\"error\": \"Missing 'sql' field\"}");
            return;
        }

        Logger::instance().info(QString("Executing SQL: %1").arg(sql));

        try {
            // Lexer & Parser
            Lexer lexer(sql);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);
            auto statement = parser.parse();

            if (!statement) {
                 sendResponse(socket, 400, "application/json", "{\"error\": \"Parse error\"}");
                 return;
            }

            auto result = queryExecutor->execute(statement);

            QJsonObject response;
            response["success"] = result->success;
            
            if (result->success) {
                response["affectedRows"] = (int)result->affectedRows;
                
                QJsonArray columnsArray;
                for (const auto& col : result->columns) columnsArray.append(col);
                response["columns"] = columnsArray;

                QJsonArray rowsArray;
                for (const auto& row : result->rows) {
                    QJsonArray rowArray;
                    for (const auto& val : row) rowArray.append(val);
                    rowsArray.append(rowArray);
                }
                response["rows"] = rowsArray;
            } else {
                response["error"] = result->errorMessage;
            }

            sendResponse(socket, 200, "application/json", QJsonDocument(response).toJson());

        } catch (const std::exception& e) {
            QString error = QString("Exception: %1").arg(e.what());
             Logger::instance().error(error);
             sendResponse(socket, 500, "application/json", QString("{\"error\": \"%1\"}").arg(error).toUtf8());
        } catch (...) {
             QString error = "Unknown exception during request handling";
             Logger::instance().error(error);
             sendResponse(socket, 500, "application/json", QString("{\"error\": \"%1\"}").arg(error).toUtf8());
        }
        return;
    }

    sendResponse(socket, 404, "text/plain", "Not Found");
}

void DatabaseServer::sendResponse(QTcpSocket* socket, int statusCode, const QByteArray& contentType, const QByteArray& body) {
    QString statusText;
    if (statusCode == 200) statusText = "OK";
    else if (statusCode == 204) statusText = "No Content";
    else if (statusCode == 400) statusText = "Bad Request";
    else if (statusCode == 404) statusText = "Not Found";
    else if (statusCode == 500) statusText = "Internal Server Error";

    QByteArray response = QString("HTTP/1.1 %1 %2\r\n").arg(statusCode).arg(statusText).toUtf8();
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    response += "Access-Control-Allow-Headers: Content-Type\r\n";
    response += QString("Content-Type: %1\r\n").arg(QString::fromUtf8(contentType)).toUtf8();
    response += QString("Content-Length: %1\r\n").arg(body.size()).toUtf8();
    response += "Connection: close\r\n\r\n";
    response += body;

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(response);
        socket->disconnectFromHost();
        Logger::instance().info(QString("Response sent: %1").arg(statusCode));
    } else {
        Logger::instance().warning("Socket not connected, cannot send response");
    }
}
