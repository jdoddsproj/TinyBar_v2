#include "HyprListener.h"

// ===== Constructor / Destructor =====
HyprListener::HyprListener(QObject* parent)
    : QObject(parent)
    , eventSocket(new QLocalSocket(this))
{
    sendControlCommand(HyprCommand::GetActiveWorkspace); 
    sendControlCommand(HyprCommand::GetWorkspaces);
    connectToEventSocket();
}

// ===== Public Functions =====

// ===== Private Functions =====
void HyprListener::sendControlCommand(HyprCommand cmd) {
    QString uid = QString::number(getuid());
    QString sig = QString::fromUtf8(qgetenv("HYPRLAND_INSTANCE_SIGNATURE")); 
    QString socketPath = QString("/run/user/%1/hypr/%2/.socket.sock").arg(uid).arg(sig);

    QLocalSocket* controlSocket = new QLocalSocket(this);
    controlSocket->setProperty("cmd", QVariant::fromValue(cmd));

    connect(controlSocket, &QLocalSocket::connected, this, [=]() {
        controlSocket->write(cmdToStr(cmd).toUtf8());
        controlSocket->flush();
    });
    connect(controlSocket, &QLocalSocket::readyRead, this, &HyprListener::readCommandResponse);
    connect(controlSocket, &QLocalSocket::disconnected, controlSocket, &QLocalSocket::deleteLater);
    connect(controlSocket, &QLocalSocket::errorOccurred, this, &HyprListener::onError);  

    controlSocket->connectToServer(socketPath);
}


void HyprListener::connectToEventSocket() {
    QString uid = QString::number(getuid());
    QString sig = QString::fromUtf8(qgetenv("HYPRLAND_INSTANCE_SIGNATURE")); 
    QString socketPath = QString("/run/user/%1/hypr/%2/.socket2.sock").arg(uid).arg(sig);

    connect(eventSocket, &QLocalSocket::readyRead, this, &HyprListener::readEventData);
    connect(eventSocket, &QLocalSocket::errorOccurred, this, &HyprListener::onError);

    eventSocket->connectToServer(socketPath); 
}


QString HyprListener::cmdToStr(HyprCommand cmd) {
    switch(cmd) {
        case HyprCommand::GetClients: return "j/clients";
        case HyprCommand::GetWorkspaces: return "j/workspaces";
        case HyprCommand::GetMonitors: return "j/monitors";
        case HyprCommand::GetActiveWorkspace: return "j/activeworkspace";
        default: return "";
    }
}


HyprListener::HyprEvent HyprListener::identifyEvent(const QString& event) {
    if (event.startsWith("workspace>>")) return HyprEvent::Workspace;
    else if (event.startsWith("focusedmon>>")) return HyprEvent::FocusedMon;
    else if (event.startsWith("createworkspace>>")) return HyprEvent::WorkspaceCreated;
    else if (event.startsWith("destroyworkspace>>")) return HyprEvent::WorkspaceDestroyed;
    else return HyprEvent::Unknown;
}


// ===== Private Slots =====
// socket2.sock (events)
void HyprListener::readEventData() {
    QByteArray raw = eventSocket->readAll();
    QString data = QString::fromUtf8(raw);
    QStringList events = data.split("\n", Qt::SkipEmptyParts); 

    for (const QString& event : events) {
        switch(identifyEvent(event)) {
            case HyprEvent::Workspace:
                emit workspaceChanged(event.split(">>").last());
                break;
            case HyprEvent::FocusedMon:
                emit focusedWorkspaceChanged(event.split(",").last());
                break;
            case HyprEvent::WorkspaceCreated:
                sendControlCommand(HyprCommand::GetWorkspaces);
                break;
            case HyprEvent::WorkspaceDestroyed:
                sendControlCommand(HyprCommand::GetWorkspaces);
                break;
            default: 
                emit unhandledEvent(event);
                //qDebug() << "From event socket (unhandled event):" << event;
        }
    }
}

// socket.sock (command/response)
void HyprListener::readCommandResponse() {
    QLocalSocket* controlSocket = qobject_cast<QLocalSocket*>(sender());
    QByteArray raw = controlSocket->readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    HyprCommand cmd = controlSocket->property("cmd").value<HyprCommand>();

    if (err.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse control respone:" << err.errorString();
        return;
    }
    if (cmd == HyprCommand::GetWorkspaces) {
        QJsonArray arr = doc.array();
        emit activeWorkspacesChanged(QString::number(arr.size()));
        //qDebug() << arr.size();
    }
    else if (cmd == HyprCommand::GetActiveWorkspace) {
        emit workspaceChanged(doc.object()["name"].toString());
    } 
} 


void HyprListener::onError(QLocalSocket::LocalSocketError error) {
    // Ignoring PeerClosedError because it is triggered when Hyprland closes a socket
    if (error != QLocalSocket::PeerClosedError) {
        qWarning() << "Hyprland socket error:" << error;
    }
}

