#pragma once

#include <QDebug>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QFont>
#include <QIcon>
#include <QFile>
#include <QPushButton>
#include <QSpacerItem>
#include <LayerShellQt/window.h>
#include "Clock.h"
#include "Battery.h"
#include "HyprListener.h"
#include "Settings.h"

using namespace LayerShellQt;

const auto barStyleSheet = R"(color: white; background-color: transparent;)"; // #303030


class BarManager : public QWidget {
public:
    explicit BarManager(QWidget* parent = nullptr);

private:
    void initConnections();
    void initLayout();

    QWidget* m_leftArea, *m_centerArea, *m_rightArea;
    QHBoxLayout *m_leftLayout, *m_centerLayout, *m_rightLayout;

    HyprListener* m_listener = nullptr;

    QLabel* m_currentWorkspace = nullptr;
    QLabel* m_numActiveWorkspaces = nullptr; 
};
