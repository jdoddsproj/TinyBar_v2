#ifndef BATTERYOLD_H
#define BATTERYOLD_H

#include <QWidget>
#include <QList>
#include <QPixmap>
#include <QPaintEvent>
#include <QEvent>
#include <QSvgWidget>

class BatteryWidget : public QWidget {
    Q_OBJECT
public:
    explicit BatteryWidget(QWidget* parent = nullptr, const QSize& battery = {24, 24}, const QSize& bolt = {16, 16});

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private slots:
    void onPluggedIn(bool is);
    void onChargeChanged(int charge);
    void handleProperties(const QString& path, const QVariantMap& changed, const QStringList& invalidated);

private:
    enum State {Low, Quarter, Half, ThreeQuarters, Full, PluggedIn};
    State m_state = Low;
    bool m_isPluggedIn = false;
    int m_charge = 0;
    bool m_mouseOnWidget = false;
    QMap<State, QPixmap> m_icons;

    QPixmap svgToPixmap(const QString& path, const QSize& size);
    QPixmap pixmapChangeColor(const QPixmap& pixmap, const QColor& color);
    void loadIcons(const QSize& battery, const QSize& bolt);
};
#endif //BATTERYOLD_H
