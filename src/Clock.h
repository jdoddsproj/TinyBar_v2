#ifndef CLOCK_H
#define CLOCK_H

#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QString>

class TinyClock : public QLabel {
    Q_OBJECT
public:
    static inline const QString DEFAULT = "h:mm AP"; // ex. 3:00 PM

    explicit TinyClock(QWidget* parent = nullptr, const QString &format = DEFAULT);
};
#endif //CLOCK_H
