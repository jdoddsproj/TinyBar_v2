#pragma once

#include <QCoreApplication>
#include <QLocalSocket>
#include <QProcess>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QLocalSocket>
#include <unistd.h>

using namespace std;

class HyprListener : public QObject {
    Q_OBJECT
public:
    explicit HyprListener(QObject* parent = nullptr); 

signals:
    void workspaceChanged(const QString& workspace);
    void focusedWorkspaceChanged(const QString& workspace);
    void activeWorkspacesChanged(const QString& numActiveWorkspaces);
    void unhandledEvent(const QString& event);

private slots:
    void readCommandResponse();
    void readEventData();
    void onError(QLocalSocket::LocalSocketError error);

private:
    enum class HyprCommand { 
        GetClients,
        GetWorkspaces,
        GetMonitors,
        GetActiveWorkspace
    };
    Q_ENUM(HyprCommand)
    static QString cmdToStr(HyprCommand cmd); 

    enum class HyprEvent {
        Workspace,
        FocusedMon,
        WorkspaceCreated,
        WorkspaceDestroyed,
        MonitorAdded, // Add signal and implement in .cpp
        MonitorDestroyed, // Add signal and implement in .cpp
        Unknown,
    };
    static HyprEvent identifyEvent(const QString& event); 

    void sendControlCommand(HyprCommand cmd);
    void connectToEventSocket();

    QLocalSocket* eventSocket  = nullptr; // Hyprland .socket2.sock 
};
