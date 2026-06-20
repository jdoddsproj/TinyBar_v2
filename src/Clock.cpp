#include "Clock.h"

TinyClock::TinyClock(QWidget* parent, const QString &format)
    : QLabel(parent)
{
    QTimer* timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [this, &format]() {
        this->setText(QDateTime::currentDateTime().toString(format));
    });

    timer->start(1000);
    setText(QDateTime::currentDateTime().toString(format));
}
