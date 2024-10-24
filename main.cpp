#include <QApplication>
#include <gameview.h>

GameView *game;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    game = new GameView();

    QRect screenrect = a.primaryScreen()->geometry();
    game->move(screenrect.top(), screenrect.left());
    game->show();
    game->displayMainMenu();

    return a.exec();
}
