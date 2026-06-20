#include <QApplication>
#include <QScreen>
#include <QFont>
#include "BarManager.h"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QFont appFont = QFont("Inter", 14);
    appFont.setBold(true);
    app.setFont(appFont);

    // Bar widget setup
    BarManager bar;
    bar.createWinId();

    // LayerShell setup
    QWindow* window = bar.windowHandle();
    Window* lsWindow = Window::get(window);

    using Anchor = Window::Anchor;
    using Anchors = Window::Anchors;
    Anchors anchors = Anchors(Anchor::AnchorTop) | Anchor::AnchorLeft | Anchor::AnchorRight;

    lsWindow->setAnchors(anchors);
    lsWindow->setExclusiveZone(30);
    lsWindow->setLayer(Window::LayerTop);
    lsWindow->setKeyboardInteractivity(Window::KeyboardInteractivityNone);

    bar.show();
    return app.exec();
}
