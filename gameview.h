#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include "boardview.h"
#include "boardviewmodel.h"
#include "pawnfield.h"
#include "playerview.h"
#include <QLabel>
#include <QAudioOutput>
#include <QMediaPlayer>

class GameView : public QGraphicsView {
    Q_OBJECT

public:
    enum BoardStatus{
        Error,
        Nothing,
        Check,
        Checkmate,
        Stalemate
    };
    Q_ENUM(BoardStatus)

    GameView();
    ~GameView();

    QGraphicsScene *scene;
    QLabel* resultLabel;
    QString result;
    BoardStatus boardStatus;
    PawnViewModel pawnViewModel;
    bool isGameModeEtudes;

    QAudioOutput* musicAudioOutput;
    QMediaPlayer* musicMediaPlayer;

    void displayMainMenu();
    // std::pair<PawnType, BoardPosition> lookForMateIn1(QList<BasePawnModel*>, QList<BasePawnModel*>);
    void lookForMateIn1(QList<BasePawnModel*>, QList<BasePawnModel*>);
    BoardStatus checkBoard(QList<BasePawnModel*>, QList<BasePawnModel*>, QString&);
    void displayResult();

public slots:
    void startGame();
    void startGameEtudes();
    void quitGame();
    void resetGame();
    void chooseGameMode();
    void callCheckBoardSlot();
    void displayMainMenuSlot();
    void displaySettings();
    void displayHelp();

private:
    BoardViewModel* boardViewModel;
    bool gameStarted;
    BoardView *board;
    PlayerView *blackPlayerView;
    PlayerView *whitePlayerView;

    void drawBoard();
    void drawSettingsPanel();
    void drawUserPanel();
    PlayerView* drawViewForUser(PlayerType player);
    void drawTitle(double yPosition, int fontSize);
    void drawGameTitle(double yPosition, int fontSize, QString);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void selectPawn(PawnField *pawn);
    void handleSelectingPointForActivePawnByMouse(QPoint point);
    void setCheckStateOnPlayerView(PlayerType player, bool isInCheck);
    void moveActivePawnToSelectedPoint(QPoint point);
    void releaseActivePawn();
    void showCongratulationsScreen(PlayerType winner);
    void showStalemateScreen();

    bool checkIsKingUnderCheck(PlayerType);
    bool checkIsKingCheckmated(PlayerType);
    bool checkIsStalemate(PlayerType);
};

#endif // GAME_H
