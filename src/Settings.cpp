#include "Settings.h"

Settings::Settings(QWidget *parent)
    : QSvgWidget{parent}
{
    load(QString(":resources/settings-gear.svg"));
    setFixedSize(24, 24);
}
