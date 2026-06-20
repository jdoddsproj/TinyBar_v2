#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSvgWidget>
#include <QString>

class Settings : public QSvgWidget {
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);

signals:
};

#endif // SETTINGS_H
