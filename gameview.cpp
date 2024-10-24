#include "gameview.h"
#include <QGraphicsTextItem>
#include <QColor>
#include <QBrush>
#include "actionbutton.h"
#include "congratulationsview.h"
#include "constants.h"
#include "utils.h"
#include <QGraphicsProxyWidget>
#include <QVector>
#include "kingpawnmodel.h"
#include "queenpawnmodel.h"
#include "bishoppawnmodel.h"
#include "knightpawnmodel.h"
#include "rookpawnmodel.h"
#include "pawnpawnmodel.h"
#include <QSlider>

int viewWidth = 1200;
int viewHeight= 768;

GameView::GameView() {

    setWindowTitle(QString("Chess Game"));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(viewWidth, viewHeight);

    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, viewWidth, viewHeight);
    setScene(scene);

    pawnViewModel = PawnViewModel();

    QBrush brush;
    brush.setStyle((Qt::SolidPattern));
    QColor color = QColor(44, 41, 51);
    brush.setColor(color);
    scene->setBackgroundBrush(brush);

    gameStarted = false;

    //setting up background music
    musicMediaPlayer = new QMediaPlayer();
    musicMediaPlayer->setSource(QUrl("qrc:/sounds/background.mp3"));
    musicAudioOutput = new QAudioOutput();
    musicAudioOutput->setVolume(music);
    musicMediaPlayer->setAudioOutput(musicAudioOutput);
    musicMediaPlayer->setLoops(9999);
    musicMediaPlayer->play();
}

GameView::~GameView() {
    delete musicAudioOutput;
    delete musicMediaPlayer;
    delete resultLabel;
    delete boardViewModel;
    delete blackPlayerView;
    delete whitePlayerView;
    delete board;
    delete scene;
}

void GameView::displayMainMenu() {
    // clear the scene
    scene->clear();

    //create the logo
    QPixmap logoPixmap(":/Images/logo.png");
    QGraphicsPixmapItem* pixmapItem = scene->addPixmap(logoPixmap.scaled(100,100,Qt::KeepAspectRatio));

    // create the title text
    QGraphicsTextItem* titleText = new QGraphicsTextItem(QString("Chess Games"));
    titleText->setFont(QFont("Arial",50));
    titleText->setDefaultTextColor(Qt::white);
    int txPos = this->width()/2 - titleText->boundingRect().width()/2 + pixmapItem->boundingRect().width()/2;
    int tyPos = 200;
    titleText->setPos(txPos,tyPos);
    scene->addItem(titleText);
    pixmapItem->setPos(this->width()/2 - pixmapItem->boundingRect().width()/2 - titleText->boundingRect().width()/2 - 10,185);

    // create start button
    ActionButton *startButton = new ActionButton("Play");
    double buttonXPosition = this->width()/2 - startButton->boundingRect().width()/2;
    double buttonYPosition = 425;//275;
    startButton->setPos(buttonXPosition, buttonYPosition);

    connect(startButton, SIGNAL(buttonPressed()), this, SLOT(chooseGameMode()));
    scene->addItem(startButton);

    // create the settings button
    ActionButton* settingsButton = new ActionButton(QString("Settings"));
    int sxPos = this->width()/2 - settingsButton->boundingRect().width()/2;//width/2 - settingsButton->boundingRect().width()*1.35;
    int syPos = 500;

    settingsButton->setPos(sxPos, syPos);
    connect(settingsButton, SIGNAL(buttonPressed()), this, SLOT(displaySettings()));
    scene->addItem(settingsButton);

    // create quit button
    ActionButton *quitButton = new ActionButton("Quit");
    double quitXPosition = this->width()/2 - quitButton->boundingRect().width()/2;
    double quitYPosition = 575;//350;
    quitButton->setPos(quitXPosition, quitYPosition);

    connect(quitButton, SIGNAL(buttonPressed()), this, SLOT(quitGame()));
    scene->addItem(quitButton);
}

void GameView::startGameEtudes() {
    isGameModeEtudes = true;
    constIsGameModeEtudes = isGameModeEtudes;
    scene->clear();

    boardViewModel = new BoardViewModel(true);

    drawBoard();
    drawSettingsPanel();
    drawUserPanel();
    int titleYPosition = Constants::defaultMargin;
    drawTitle(titleYPosition, 40);
    gameStarted = true;
}

void GameView::startGame() {
    isGameModeEtudes = false;
    constIsGameModeEtudes = isGameModeEtudes;
    scene->clear();

    boardViewModel = new BoardViewModel(false);

    drawBoard();
    drawSettingsPanel();
    drawUserPanel();
    int titleYPosition = Constants::defaultMargin;
    drawTitle(titleYPosition, 40);
    gameStarted = true;
}

void GameView::quitGame() {
    close();
}

void GameView::resetGame() {
    gameStarted = false;
    scene->clear();
    if (constIsGameModeEtudes) {
        startGameEtudes();
        return;
    }
    startGame();
}

void GameView::chooseGameMode() {
    scene->clear();

    // create title label
    double titleYPosition = 25;//150;
    drawGameTitle(titleYPosition, 50, "Choose Game Mode");

    // create etudes button
    ActionButton *etudesButton = new ActionButton("Etudes");
    // double buttonXPosition = this->width()/2 - etudesButton->boundingRect().width()/2;
    // double buttonYPosition = 275;
    etudesButton->setPos(250, 600);

    QPixmap etudesPixmap(":/Images/EtudesBoard.png");
    QGraphicsPixmapItem* etudesPixmapItem = scene->addPixmap(etudesPixmap.scaled(400,400,Qt::KeepAspectRatio));
    etudesPixmapItem->setPos(170,150);//this->width()/2 - pixmapItem->boundingRect().width()/2 - titleText->boundingRect().width()/2 - 10,185);

    connect(etudesButton, SIGNAL(buttonPressed()), this, SLOT(startGameEtudes()));
    scene->addItem(etudesButton);

    // create duos button
    ActionButton *duosButton = new ActionButton("Duos");
    // double duosXPosition = this->width()/2 - duosButton->boundingRect().width()/2;
    // double duosYPosition = 350;
    duosButton->setPos(750, 600);

    QPixmap duosPixmap(":/Images/DuosBoard.png");
    QGraphicsPixmapItem* duosPixmapItem = scene->addPixmap(duosPixmap.scaled(400,400,Qt::KeepAspectRatio));
    duosPixmapItem->setPos(650,150);//this->width()/2 - pixmapItem->boundingRect().width()/2 - titleText->boundingRect().width()/2 - 10,185);

    connect(duosButton, SIGNAL(buttonPressed()), this, SLOT(startGame()));
    scene->addItem(duosButton);
}

GameView::BoardStatus GameView::checkBoard(QList<BasePawnModel*> whitePawns, QList<BasePawnModel*> blackPawns, QString& resultParam) {
    auto pawns = whitePawns;
    for (auto in : blackPawns) {
        pawns.push_back(in);
    }

    BasePawnModel* blackKing = nullptr;
    for (auto pawn : blackPawns) {
        if (pawn->type == PawnType::king) {
            blackKing = pawn;
            break;
        }
    }

    if (blackKing == nullptr) {
        boardStatus = BoardStatus::Error;
        resultParam = "Invalid input position, Black King is missing!";
        return boardStatus;
    }

    BasePawnModel* whiteKing = nullptr;
    for (auto pawn : whitePawns) {
        if (pawn->type == PawnType::king) {
            whiteKing = pawn;
            break;
        }
    }

    if (whiteKing == nullptr) {
        boardStatus = BoardStatus::Error;
        resultParam = "Invalid input position, White King is missing!";
        return boardStatus;
    }

    for (auto pawn : pawns) {
        pawn->move(pawns);
    }

    for (auto pos : blackKing->posThatCanMove) {
        if (whiteKing->position == pos) {
            boardStatus = BoardStatus::Error;
            resultParam = "Invalid input position, Kings can't be close to each other!";
            return boardStatus;
        }
    }

    for (auto pos : whiteKing->posThatCanMove) {
        if (blackKing->position == pos) {
            boardStatus = BoardStatus::Error;
            resultParam = "Invalid input position, Kings can't be close to each other!";
            return boardStatus;
        }
    }

    if (whitePawns.size() == 1) {
        boardStatus = BoardStatus::Error;
        resultParam = "The entered position is draw, there is not enough pawns to deliver a checkmate!";
        return boardStatus;
    }

    if (whitePawns.size() == 2) {
        for (auto pawn : whitePawns) {
            switch (pawn->type) {
            case PawnType::bishop:
            case PawnType::knight:
                boardStatus = BoardStatus::Error;
                resultParam = "The entered position is draw, there is not enough pawns to deliver a checkmate!";
                return boardStatus;
                break;
            case PawnType::queen:
            case PawnType::rook:
                break;
            case PawnType::king:
                continue;
                break;
            default:
                break;
            }
        }
    }

    bool isBlackKingUnderCheck = false;
    bool hasBlackKingEscapePos = true;

    auto blackKingEscapePoss = blackKing->posThatCanMove;

    //Checking if black king has escape squares or not
    for (auto pawn : whitePawns){
        for (auto posWhitePawn : pawn->posThatCanMove){
            for (auto posBlackKing : blackKingEscapePoss){
                if (posBlackKing.x == posWhitePawn.x && posBlackKing.y == posWhitePawn.y){
                    // blackKingEscapePoss.removeOne(posBlackKing);
                    auto i = std::remove(blackKingEscapePoss.begin(), blackKingEscapePoss.end(), posBlackKing);
                    blackKingEscapePoss.erase(i,blackKingEscapePoss.end());
                }
            }
        }
        if (blackKingEscapePoss.empty()){
            hasBlackKingEscapePos = false;
            break;
        }
    }

    //Checking if black king is under check or not
    for (auto pawn : whitePawns){
        if (isBlackKingUnderCheck == false){
            for (auto posWhitePawn : pawn->posThatCanMove){
                if (blackKing->position.x == posWhitePawn.x && blackKing->position.y == posWhitePawn.y){
                    isBlackKingUnderCheck = true;
                    break;
                }
            }
        }
    }
    if (isBlackKingUnderCheck == false && hasBlackKingEscapePos == false){
        if (!isGameModeEtudes) {
            if (blackPawns.size() > 1) {
                for (auto pawn : blackPawns) {
                    if (pawn == blackKing) {
                        continue;
                    }
                    else {
                        if (!pawn->posThatCanMove.empty()){
                            boardStatus = BoardStatus::Nothing;
                            return BoardStatus::Nothing;
                        }
                    }
                }
            }
        }
        boardStatus = BoardStatus::Stalemate;
        resultParam = "The entered position is stalemate, the Black King has no legal moves!";
    }
    else if (isBlackKingUnderCheck == true && hasBlackKingEscapePos == true){
        boardStatus = BoardStatus::Check;
        resultParam = "The entered position is check, the Black King is under check, but he can escape!";
    }
    else if (isBlackKingUnderCheck == true && hasBlackKingEscapePos == false){
        boardStatus = BoardStatus::Checkmate;
        resultParam = "The entered position is checkmate, the Black King is under check and has no legal moves!";
    }
    //if (isBlackKingUnderCheck == false && hasBlackKingEscapePos == true){
    else{
        boardStatus = BoardStatus::Nothing;
        resultParam = "The entered position is not check, checkmate or stalemate";
    }
    return boardStatus;
}

// std::pair<PawnType, BoardPosition> GameView::lookForMateIn1(QList<BasePawnModel*> whitePawns, QList<BasePawnModel*> blackPawns)
void GameView::lookForMateIn1(QList<BasePawnModel*> whitePawnsList, QList<BasePawnModel*> blackPawnsList) {

    for (int indexWhitePawn = 0; indexWhitePawn < whitePawnsList.size(); ++indexWhitePawn) {
        for (auto pos : whitePawnsList[indexWhitePawn]->posThatCanMove) {
            bool temp = false;
            QVector<BasePawnModel*> tempWhitePawns;
            // QVector<BasePawnModel*> tempBlackPawns;
            for (auto whitePawn : whitePawnsList) {
                if (pos == whitePawn->position) {
                    temp = true;
                    break;
                }
                BoardPosition boardPosition = whitePawn->position;
                QString imagePath;
                BasePawnModel* pawn;
                if (whitePawnsList[indexWhitePawn]->type == whitePawn->type) {
                    boardPosition = pos;
                }
                switch (whitePawn->type) {
                case PawnType::king:
                    imagePath = pawnViewModel.getImagePath(PawnType::king, PlayerType::white);
                    pawn = new KingPawnModel(boardPosition, PlayerType::white, PawnType::king, imagePath);
                    tempWhitePawns.push_back(pawn);
                    break;
                case PawnType::queen:
                    imagePath = pawnViewModel.getImagePath(PawnType::queen, PlayerType::white);
                    pawn = new QueenPawnModel(boardPosition, PlayerType::white, PawnType::queen, imagePath);
                    tempWhitePawns.push_back(pawn);
                    break;
                case PawnType::bishop:
                    imagePath = pawnViewModel.getImagePath(PawnType::bishop, PlayerType::white);
                    pawn = new BishopPawnModel(boardPosition, PlayerType::white, PawnType::bishop, imagePath);
                    tempWhitePawns.push_back(pawn);
                    break;
                case PawnType::knight:
                    imagePath = pawnViewModel.getImagePath(PawnType::knight, PlayerType::white);
                    pawn = new KnightPawnModel(boardPosition, PlayerType::white, PawnType::knight, imagePath);
                    tempWhitePawns.push_back(pawn);
                    break;
                case PawnType::rook:
                    imagePath = pawnViewModel.getImagePath(PawnType::rook, PlayerType::white);
                    pawn = new RookPawnModel(boardPosition, PlayerType::white, PawnType::rook, imagePath);
                    tempWhitePawns.push_back(pawn);
                    break;
                default:
                    break;
                }
            }
            if (temp) {
                continue;
            }
            tempWhitePawns[indexWhitePawn]->position = pos;
            QString str;
            switch (checkBoard(QList<BasePawnModel*>::fromVector(tempWhitePawns), blackPawnsList/*QList<BasePawnModel*>::fromVector(tempBlackPawns)*/,str)) {
            case Nothing:
            case Stalemate:
            case Check:
            case Error:
                break;
            case Checkmate:
                result.append("\nThere is mate in one: ");
                switch (tempWhitePawns[indexWhitePawn]->type) {
                case PawnType::king:
                    result.append("White King ");
                    break;
                case PawnType::queen:
                    result.append("White Queen ");
                    break;
                case PawnType::knight:
                    result.append("White Knight ");
                    break;
                case PawnType::bishop:
                    result.append("White Bishop ");
                    break;
                case PawnType::rook:
                    result.append("White Rook ");
                    break;
                default:
                    break;
                }
                result.append((char)(97+pos.x));
                result.append(QString::number(8-pos.y));
                return;
                break;
            default:
                break;
            }
        }
    }

    // BasePawnModel* whiteQueen;
    // BasePawnModel* whiteKing;
    // BasePawnModel* blackKing;

    // for (auto pawn : boardViewModel->getWhitePawns()) {
    //     if (pawn->owner == PlayerType::white && pawn->type == PawnType::queen) {
    //         whiteQueen = pawn;
    //     }
    //     else if(pawn->owner == PlayerType::white && pawn->type == PawnType::king) {
    //         whiteKing = pawn;
    //     }
    // }

    // for (auto pawn : boardViewModel->getBlackPawns()) {
    //     if (pawn->owner == PlayerType::black && pawn->type == PawnType::king) {
    //         blackKing = pawn;
    //     }
    // }

    // for (auto whiteKingPos : whiteKing->posThatCanMove) {
    //     for (auto whiteQueenPos : whiteQueen->posThatCanMove) {
    //         for (auto blackKingPos : blackKing->posThatCanMove) {
    //             if (whiteKingPos == whiteQueenPos && whiteKingPos == blackKingPos && whiteQueenPos == blackKingPos && !boardViewModel->getPawnOnBoardPosition(whiteKingPos)) {
    //                 if (blackKing->position.x == whiteQueenPos.x || blackKing->position.y == whiteQueenPos.y) {
    //                     result.append("\nThere is mate in one: ");
    //                     result.append("White Queen ");
    //                     result.append((char)(97+whiteQueenPos.x));
    //                     result.append(QString::number(8-whiteQueenPos.y));
    //                     return;
    //                 }
    //             }
    //         }
    //     }
    // }
}

void GameView::displayResult()
{
    resultLabel->setText(result);
}

void GameView::callCheckBoardSlot()
{
    boardStatus = checkBoard(boardViewModel->getWhitePawns(), boardViewModel->getBlackPawns(), result);
    if (boardStatus == BoardStatus::Nothing || boardStatus == BoardStatus::Stalemate){
        lookForMateIn1(boardViewModel->getWhitePawns(), boardViewModel->getBlackPawns());
    }
    displayResult();
}

void GameView::displayMainMenuSlot()
{
    gameStarted = false;
    scene->clear();
    displayMainMenu();
}

void GameView::displaySettings()
{
    // clear the scene
    scene->clear();

    // create the title text
    QGraphicsTextItem* titleText = new QGraphicsTextItem(QString("Settings"));
    titleText->setFont(QFont("Arial",45));
    titleText->setDefaultTextColor(Qt::white);
    int tTxPos = this->width()/2 - titleText->boundingRect().width()/2;
    int tTyPos = 65;
    titleText->setPos(tTxPos,tTyPos);
    scene->addItem(titleText);

    // create the sliders

    // sfx slider

    // create the title text for sfx
    QGraphicsTextItem* sfxText = new QGraphicsTextItem(QString("SFX"));
    sfxText->setFont(QFont("Arial",20));
    sfxText->setDefaultTextColor(Qt::white);
    int sTxPos = 225;//this->width()/2 - sfxText->boundingRect().width()/2;
    int sTyPos = 230;
    sfxText->setPos(sTxPos,sTyPos);
    scene->addItem(sfxText);

    QSlider* sfxSlider = new QSlider(Qt::Horizontal);
    sfxSlider->setStyleSheet("QSlider::groove:horizontal {"
                             "border: 1px solid rgb(44, 41, 51);"
                             "height: 100px;"
                             "border-radius: 10px;"
                             "}"
                             ""
                             "QSlider::sub-page:horizontal {"
                             "border: 1px solid rgb(44, 41, 51);"
                             "background-color: rgb(157, 128, 101);"
                             "height: 100px;"
                             "border-radius: 1px;"
                             "}"
                             ""
                             "QSlider::add-page:horizontal {"
                             "background: rgb(196, 178, 140);"
                             "border: 1px solid rgb(44, 41, 51);"
                             "height: 100px;"
                             "border-radius: 10px;"
                             "}"
                             ""
                             "QSlider::handle:horizontal {"
                             "background: bisque;"
                             "border: 1px solid rgb(44, 41, 51);"
                             "width: 12 px;"
                             "margin-top: -2px;"
                             "margin-bottom: -2px;"
                             "border-radius: 10px;"
                             "}"
                             );
    sfxSlider->resize(600,25);
    sfxSlider->setSliderPosition(sound);
    sfxSlider->connect(sfxSlider, &QSlider::valueChanged, [this, sfxSlider](){
        sound = sfxSlider->value();
        sfxSlider->setSliderPosition(sound);
    });
    auto* sfxItem = scene->addWidget(sfxSlider);
    sfxItem->setPos(310,235);

    // music slider

    // create the title text for music
    QGraphicsTextItem* musicText = new QGraphicsTextItem(QString("Music"));
    musicText->setFont(QFont("Arial",20));
    musicText->setDefaultTextColor(Qt::white);
    int mTxPos = 225;//this->width()/2 - musicText->boundingRect().width()/2;
    int mTyPos = 280;
    musicText->setPos(mTxPos,mTyPos);
    scene->addItem(musicText);

    QSlider* musicSlider = new QSlider(Qt::Horizontal);
    musicSlider->setStyleSheet("QSlider::groove:horizontal {"
                               "border: 1px solid rgb(44, 41, 51);"
                               "height: 100px;"
                               "border-radius: 10px;"
                               "}"
                               ""
                               "QSlider::sub-page:horizontal {"
                               "border: 1px solid rgb(44, 41, 51);"
                               "background-color: rgb(157, 128, 101);"
                               "height: 100px;"
                               "border-radius: 1px;"
                               "}"
                               ""
                               "QSlider::add-page:horizontal {"
                               "background: rgb(196, 178, 140);"
                               "border: 1px solid rgb(44, 41, 51);"
                               "height: 100px;"
                               "border-radius: 10px;"
                               "}"
                               ""
                               "QSlider::handle:horizontal {"
                               "background: bisque;"
                               "border: 1px solid rgb(44, 41, 51);"
                               "width: 12 px;"
                               "margin-top: -2px;"
                               "margin-bottom: -2px;"
                               "border-radius: 10px;"
                               "}"
                               );
    musicSlider->resize(600,25);
    musicSlider->setSliderPosition(music);
    musicSlider->connect(musicSlider, &QSlider::valueChanged, [this, musicSlider](){
        music = musicSlider->value();
        musicAudioOutput->setVolume(QAudio::convertVolume(music / qreal(100.0),
                                                          QAudio::LogarithmicVolumeScale,
                                                          QAudio::LinearVolumeScale));
        musicSlider->setSliderPosition(music);
    });
    auto* musicItem = scene->addWidget(musicSlider);
    musicItem->setPos(310,285);


    // create the help button
    ActionButton* helpButton = new ActionButton(QString("Help"));
    int hxPos = this->width()/2 - helpButton->boundingRect().width()/2;
    int hyPos = 500;
    helpButton->setPos(hxPos, hyPos);
    connect(helpButton, SIGNAL(buttonPressed()), this, SLOT(displayHelp()));
    scene->addItem(helpButton);

    // create the back button
    ActionButton* backButton = new ActionButton(QString("Back"));
    int bxPos = this->width()/2 - backButton->boundingRect().width()/2;
    int byPos = 575;
    backButton->setPos(bxPos, byPos);
    connect(backButton, SIGNAL(buttonPressed()), this, SLOT(displayMainMenuSlot()));
    scene->addItem(backButton);
}

void GameView::displayHelp()
{
    // clear the scene
    scene->clear();

    // create the title text
    QGraphicsTextItem* titleText = new QGraphicsTextItem(QString("Help"));
    titleText->setFont(QFont("Arial",50));
    titleText->setDefaultTextColor(Qt::white);
    int tTxPos = this->width()/2 - titleText->boundingRect().width()/2;
    int tTyPos = 35;
    titleText->setPos(tTxPos,tTyPos);
    scene->addItem(titleText);

    // create the text
    QString str;
    str.append("There are two games modes in this chess simulator: Chess Etudes and Chess Duos"
               "\n\n");
    str.append("Chess Etudes are captivating puzzles that challenge players to find elegant solutions on the board.\n\n"
               "Two critical functionalities are implemented in this program:\n\n"
               "1. Checkmate Detection: Determine whether the current position on\n"
               "the chessboard is a checkmate or not.\n\n"
               "2. One-Move Checkmate: Identify positions where a player can deliver\n"
               "checkmate in just one move.\n\n");
    str.append("You must place the pieces in chess board and hit run button.\n"
               "\n"
               "And Chess Duos is regular two player chess.\n");


    QGraphicsTextItem* text = new QGraphicsTextItem(str);
    text->setFont(QFont("Times New Roman",16));
    text->setDefaultTextColor(Qt::white);
    int txPos = 225;// */this->width()/2 - text->boundingRect().width()/2;
    int tyPos = 160;
    text->setPos(txPos,tyPos);
    scene->addItem(text);

    // create the back button
    ActionButton* backButton = new ActionButton(QString("Back"));
    int bxPos = this->width()/2 - backButton->boundingRect().width()/2;
    int byPos = 575;
    backButton->setPos(bxPos, byPos);
    connect(backButton, SIGNAL(buttonPressed()), this, SLOT(displaySettings()));
    scene->addItem(backButton);
}

void GameView::drawBoard() {
    board = new BoardView();
    board->draw();
    board->initializePawnFields(boardViewModel->getBlackPawns());
    board->initializePawnFields(boardViewModel->getWhitePawns());
}

void GameView::drawSettingsPanel() {
    // create reset button
    ActionButton *resetButton = new ActionButton("Reset game");
    double resetXPosition = 690 + resetButton->boundingRect().width()/2;
    double resetYPosition;

    // create quit button
    ActionButton *quitButton = new ActionButton("Main Menu");
    double quitXPosition = 690 + quitButton->boundingRect().width()/2;
    double quitYPosition;

    if (constIsGameModeEtudes) {
        // create check button
        ActionButton *checkButton = new ActionButton("Check");
        double checkXPosition = 690 + checkButton->boundingRect().width()/2;
        double checkYPosition = 440;
        checkButton->setPos(checkXPosition, checkYPosition);

        connect(checkButton, SIGNAL(buttonPressed()), this, SLOT(callCheckBoardSlot()));
        scene->addItem(checkButton);

        resetYPosition = 510;

        quitYPosition = 580;
    }
    else {
        resetYPosition = 420;
        quitYPosition = 490;
    }

    resetButton->setPos(resetXPosition, resetYPosition);
    connect(resetButton, SIGNAL(buttonPressed()), this, SLOT(resetGame()));
    scene->addItem(resetButton);

    quitButton->setPos(quitXPosition, quitYPosition);
    connect(quitButton, SIGNAL(buttonPressed()), this, SLOT(displayMainMenuSlot()));
    scene->addItem(quitButton);
}

void GameView::drawUserPanel() {
    blackPlayerView = drawViewForUser(PlayerType::black);
    whitePlayerView = drawViewForUser(PlayerType::white);

    whitePlayerView->setActive(true);
    // blackPlayerView->setActive(true);

    if (!constIsGameModeEtudes) {
        return;
    }
    resultLabel = new QLabel("Hit Check to Start");
    resultLabel->setWordWrap(true);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setFixedSize(500,300);

    // QColor(157, 128, 101)
    // QColor(196, 178, 140)
    // QColor(44, 41, 51)

    resultLabel->setStyleSheet("QLabel {"
                               "color: rgb(44, 41, 51);"
                               "background-color: rgb(196, 178, 140);"
                               "border: solid 1px black;"
                               "text-align: center;"
                               "font-size: 25px;"
                               "padding: 10px;"
                               "}");

    auto* resultLabelProxyWidget = scene->addWidget(resultLabel);
    resultLabelProxyWidget->setPos(650,122);
}

PlayerView* GameView::drawViewForUser(PlayerType player) {
    PlayerView *playerView = new PlayerView();

    int xPosition = 80;
    int yPosition = BoardView::startYPosition;

    switch (player) {
    case PlayerType::black:
        xPosition = 680;
        break;
    case PlayerType::white:
        xPosition = 680 + PlayerView::defaultWidthHeight + 20;
        break;
    }

    if (!constIsGameModeEtudes) {
        scene->addItem(playerView);
    }
    playerView->setRect(xPosition, yPosition, PlayerView::defaultWidthHeight, PlayerView::defaultWidthHeight);
    playerView->setPlayer(player);

    return playerView;
}

void GameView::drawTitle(double yPosition, int fontSize) {
    QString titleStr = "Chess Duos";
    if (constIsGameModeEtudes) {
        titleStr = "Chess Etudes";
    }

    QGraphicsTextItem *title = Utils::createTextItem(titleStr, fontSize, Qt::white);
    double xPosition = this->width()/2 - title->boundingRect().width()/2;
    title->setPos(xPosition, yPosition);
    scene->addItem(title);
}

void GameView::drawGameTitle(double yPosition, int fontSize, QString titleString) {
    QGraphicsTextItem *title = Utils::createTextItem(titleString, fontSize, Qt::white);
    double xPosition = this->width()/2 - title->boundingRect().width()/2;
    title->setPos(xPosition, yPosition);
    scene->addItem(title);
}

void GameView::mousePressEvent(QMouseEvent *event) {
        if (!gameStarted) {
            QGraphicsView::mousePressEvent(event);
            return;
        } else if (event->button() == Qt::RightButton) {
            releaseActivePawn();
        } else if (boardViewModel->getActivePawn()) {
            handleSelectingPointForActivePawnByMouse(event->pos());
        } else {
            PawnField *pawn = board->getPawnAtMousePosition(event->pos());
            selectPawn(pawn);
        }
    QGraphicsView::mousePressEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent *event) {
    // if there is a pawn selected, then make it follow the mouse
    if (gameStarted && boardViewModel->getActivePawn()) {
        board->moveActivePawnToMousePosition(event->pos(), boardViewModel->getActivePawn());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GameView::selectPawn(PawnField *pawn) {
    if (pawn == nullptr) {
        return;
    }

    boardViewModel->setActivePawnForField(pawn);
}

void GameView::handleSelectingPointForActivePawnByMouse(QPoint point) {
    if (boardViewModel->getActivePawn() == nullptr) {
        return;
    }

    // check if mouse selected place on board
    if (!boardViewModel->validatePawnPalcementForMousePosition(point)) {
        return;
    }

    BoardPosition boardPosition = boardViewModel->getBoardPositionForMousePosition(point);

    if (boardViewModel->getActivePawn()->position == boardPosition) {
        return;
    }

    if (constIsGameModeEtudes) {
        // check if field was taken by opposite player and remove it from the board
        // if (boardViewModel->didRemoveEnemyOnBoardPositionEtudes(boardPosition)) {
        if (boardViewModel->didRemoveEnemyOnBoardPosition(boardPosition)) {
            board->removePawnAtBoardPosition(boardPosition);
        }

        // move active pawn to new position
        moveActivePawnToSelectedPoint(point);

        // check if pawn can be promoted
        if (boardViewModel->didPromoteActivePawn()) {
            board->promotePawnAtBoardPosition(boardPosition);
        }

        // check if game is over
        if (boardViewModel->getWinner()) {
            showCongratulationsScreen(*boardViewModel->getWinner());
            return;
        }

        // change round owner to opposite player
        boardViewModel->discardActivePawn();
        // boardViewModel->switchRound();
        blackPlayerView->setActive(boardViewModel->getWhosTurn() == PlayerType::black);
        whitePlayerView->setActive(boardViewModel->getWhosTurn() == PlayerType::white);
        return;
    }



    // first validate Move
    if (!boardViewModel->validatePawnMove(boardPosition)) {
        return;
    }

    // Players cannot make any move that places their own king in check
    bool isKingInCheck = boardViewModel->isKingInCheck(boardViewModel->getActivePawn()->owner, true, boardPosition);

    if (boardViewModel->getActivePawn()->type == PawnType::king && boardViewModel->getPawnOnBoardPosition(boardPosition)) {
        auto newBoardViewModel = boardViewModel;
        auto oldPawn = newBoardViewModel->getPawnOnBoardPosition(boardPosition);
        BasePawnModel* pawn;
        switch (oldPawn->type) {
        case PawnType::king:
            pawn = new KingPawnModel(oldPawn->position, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::queen:
            pawn = new QueenPawnModel(oldPawn->position, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::bishop:
            pawn = new BishopPawnModel(oldPawn->position, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::knight:
            pawn = new KnightPawnModel(oldPawn->position, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::rook:
            pawn = new RookPawnModel(oldPawn->position, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::pawn:
            pawn = new PawnPawnModel(oldPawn->position, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        default:
            break;
        }
        if (newBoardViewModel->didRemoveEnemyOnBoardPosition(boardPosition)) {
            isKingInCheck = newBoardViewModel->isKingInCheck(newBoardViewModel->getActivePawn()->owner, true, boardPosition);
        }
        switch (pawn->owner) {
        case PlayerType::white:
            newBoardViewModel->getWhitePawnsRef().push_back(pawn);
            break;
        case PlayerType::black:
            newBoardViewModel->getBlackPawnsRef().push_back(pawn);
            break;
        default:
            break;
        }
    }

    if (boardViewModel->getActivePawn()->type == PawnType::king && !boardViewModel->getPawnOnBoardPosition(boardPosition)) {
        auto newBoardViewModel = boardViewModel;
        newBoardViewModel->getBlackPawnsRef() = boardViewModel->getBlackPawns();
        newBoardViewModel->getWhitePawnsRef() = boardViewModel->getWhitePawns();
        auto oldKing = newBoardViewModel->getPawnOnBoardPosition(boardViewModel->getActivePawn()->position);
        BasePawnModel* pawn = new KingPawnModel(boardPosition, oldKing->owner, oldKing->type, oldKing->imagePath);
        switch (newBoardViewModel->getActivePawn()->owner) {
        case PlayerType::white: {
            newBoardViewModel->getWhitePawnsRef().removeOne(oldKing);
            newBoardViewModel->getWhitePawnsRef().push_back(pawn);
            isKingInCheck = newBoardViewModel->isKingInCheck(PlayerType::white, true, boardPosition);
            newBoardViewModel->getWhitePawnsRef().removeOne(pawn);
            newBoardViewModel->getWhitePawnsRef().push_back(oldKing);
            delete pawn;
            break;
        }
        case PlayerType::black:{
            newBoardViewModel->getBlackPawnsRef().removeOne(oldKing);
            newBoardViewModel->getBlackPawnsRef().push_back(pawn);
            isKingInCheck = newBoardViewModel->isKingInCheck(PlayerType::black, true, boardPosition);
            newBoardViewModel->getBlackPawnsRef().removeOne(pawn);
            newBoardViewModel->getBlackPawnsRef().push_back(oldKing);
            delete pawn;
            break;
        }
        default:
            break;
        }
    } else if (boardViewModel->getActivePawn()->type != PawnType::king){// && !boardViewModel->getPawnOnBoardPosition(boardPosition)) {
        auto newBoardViewModel = boardViewModel;
        newBoardViewModel->getBlackPawnsRef() = boardViewModel->getBlackPawns();
        newBoardViewModel->getWhitePawnsRef() = boardViewModel->getWhitePawns();

        auto oldPawn = newBoardViewModel->getPawnOnBoardPosition(boardViewModel->getActivePawn()->position);
        BasePawnModel* pawn;// = new KingPawnModel(boardPosition, oldPawn->owner, oldPawn->type, oldPawn->imagePath);

        switch (oldPawn->type) {
        case PawnType::queen:
            pawn = new QueenPawnModel(boardPosition, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::bishop:
            pawn = new BishopPawnModel(boardPosition, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::knight:
            pawn = new KnightPawnModel(boardPosition, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::rook:
            pawn = new RookPawnModel(boardPosition, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        case PawnType::pawn:
            pawn = new PawnPawnModel(boardPosition, oldPawn->owner, oldPawn->type, oldPawn->imagePath);
            break;
        default:
            break;
        }
        switch (newBoardViewModel->getActivePawn()->owner) {
        case PlayerType::white: {
            newBoardViewModel->getWhitePawnsRef().removeOne(oldPawn);
            newBoardViewModel->getWhitePawnsRef().push_back(pawn);
            isKingInCheck = newBoardViewModel->isKingInCheck(PlayerType::white, true, boardPosition);
            newBoardViewModel->getWhitePawnsRef().removeOne(pawn);
            newBoardViewModel->getWhitePawnsRef().push_back(oldPawn);
            delete pawn;
            break;
        }
        case PlayerType::black:{
            newBoardViewModel->getBlackPawnsRef().removeOne(oldPawn);
            newBoardViewModel->getBlackPawnsRef().push_back(pawn);
            isKingInCheck = newBoardViewModel->isKingInCheck(PlayerType::black, true, boardPosition);
            newBoardViewModel->getBlackPawnsRef().removeOne(pawn);
            newBoardViewModel->getBlackPawnsRef().push_back(oldPawn);
            delete pawn;
            break;
        }
        default:
            break;
        }
    }

    board->setPawnMoveCheckWarning(isKingInCheck);
    if (isKingInCheck) {
        return;
    }

    // check if field was taken by opposite player and remove it from the board
    if (boardViewModel->didRemoveEnemyOnBoardPosition(boardPosition)) {
        board->removePawnAtBoardPosition(boardPosition);
    }

    // move active pawn to new position
    moveActivePawnToSelectedPoint(point);

    // check if pawn can be promoted
    if (boardViewModel->didPromoteActivePawn()) {
        board->promotePawnAtBoardPosition(boardPosition);
    }

    // check for opposite player king's check
    switch (boardViewModel->getActivePawn()->owner) {
    case PlayerType::black:
        setCheckStateOnPlayerView(PlayerType::white, boardViewModel->isKingInCheck(PlayerType::white, false, boardPosition));//checkIsKingUnderCheck(PlayerType::black));
        break;
    case PlayerType::white:
        setCheckStateOnPlayerView(PlayerType::black, boardViewModel->isKingInCheck(PlayerType::black, false, boardPosition));//checkIsKingUnderCheck(PlayerType::white));
        break;
    }

    // update active player check state
    setCheckStateOnPlayerView(boardViewModel->getActivePawn()->owner, isKingInCheck);

    // check if game is over
    if (boardViewModel->getWinner()) {
        showCongratulationsScreen(*boardViewModel->getWinner());
        return;
    }
    QString str;
    if (checkIsKingCheckmated(boardViewModel->getActivePawn()->owner)) {
        // _sleep(100);
        showCongratulationsScreen(boardViewModel->getActivePawn()->owner);
        return;
    }
    else {
        switch (boardViewModel->getActivePawn()->owner) {
        case PlayerType::white:
            if (checkBoard(boardViewModel->getWhitePawns(), boardViewModel->getBlackPawns(), str) == BoardStatus::Stalemate) {
                showStalemateScreen();
                return;
            }
            break;
        case PlayerType::black:
            if (checkBoard(boardViewModel->getBlackPawns(), boardViewModel->getWhitePawns(), str) == BoardStatus::Stalemate) {
                showStalemateScreen();
                return;
            }
            break;
        default:
            break;
        }
    }

    // change round owner to opposite player
    boardViewModel->discardActivePawn();
    boardViewModel->switchRound();
    blackPlayerView->setActive(boardViewModel->getWhosTurn() == PlayerType::black);
    whitePlayerView->setActive(boardViewModel->getWhosTurn() == PlayerType::white);
}

void GameView::setCheckStateOnPlayerView(PlayerType player, bool isInCheck) {
    switch (player) {
    case PlayerType::black:
        blackPlayerView->setIsInCheck(isInCheck);
        break;
    case PlayerType::white:
        whitePlayerView->setIsInCheck(isInCheck);
        break;
    }
}

// update pawn field position and pawn model position
void GameView::moveActivePawnToSelectedPoint(QPoint point) {
    BoardPosition boardPosition = boardViewModel->getBoardPositionForMousePosition(point);
    board->placeActivePawnAtBoardPosition(boardViewModel->getActivePawn(), boardPosition);
    boardViewModel->setNewPositionForActivePawn(boardPosition);
}

void GameView::releaseActivePawn() {
    if (boardViewModel->getActivePawn() == nullptr) {
        return;
    }

    BasePawnModel *activePawn = boardViewModel->getActivePawn();
    board->placeActivePawnAtBoardPosition(activePawn, activePawn->position);
    board->setPawnMoveCheckWarning(false);
    boardViewModel->discardActivePawn();
}

void GameView::showCongratulationsScreen(PlayerType winner) {
    gameStarted = false;

    scene->clear();

    CongratulationsView *congratulationsView = new CongratulationsView(winner);
    congratulationsView->setRect(0, 0, viewWidth, viewHeight);
}

void GameView::showStalemateScreen() {
    gameStarted = false;

    scene->clear();

    CongratulationsView *congratulationsView = new CongratulationsView();
    congratulationsView->setRect(0, 0, viewWidth, viewHeight);
}

bool GameView::checkIsKingUnderCheck(PlayerType playerType)
{
    QList<BasePawnModel*> blackPawns;// = boardViewModel->getBlackPawns();
    QList<BasePawnModel*> whitePawns;// = boardViewModel->getWhitePawns();
    if (playerType == PlayerType::white) {
        blackPawns = boardViewModel->getBlackPawns();
        whitePawns = boardViewModel->getWhitePawns();
    }
    else {
        whitePawns = boardViewModel->getBlackPawns();
        blackPawns = boardViewModel->getWhitePawns();
    }

    auto pawns = whitePawns + blackPawns;
    BasePawnModel* blackKing {};

    for (auto blackPawn : blackPawns) {
        if (blackPawn->type == PawnType::king) {
            blackKing = blackPawn;
            break;
        }
    }

    for (auto pawn : pawns) {
        pawn->move(pawns);
    }

    for (auto whitePawn : whitePawns) {
        for (auto possiblePos : whitePawn->posThatCanMove) {
            if (blackKing->position == possiblePos) {
                return true;
            }
        }
    }
    return false;
}

struct TriplePos {
    PawnType type;
    BoardPosition oldPos;
    BoardPosition newPos;
    TriplePos(PawnType type, BoardPosition oldPos, BoardPosition newPos) :  type(type), oldPos(oldPos), newPos(newPos) { }
    bool operator==(const TriplePos& oth) const{
        return (type == oth.type && oldPos == oth.oldPos && newPos == oth.newPos);
    }
};

bool GameView::checkIsKingCheckmated(PlayerType playerType) {

    bool isKingUnderCheckInitially = checkIsKingUnderCheck(playerType);
    if (!isKingUnderCheckInitially) {
        return false;
    }

    QVector<TriplePos> allLegalMoves;

    QList<BasePawnModel*> blackPawns = boardViewModel->getBlackPawns();
    QList<BasePawnModel*> whitePawns = boardViewModel->getWhitePawns();


    for (auto pawn : boardViewModel->getBlackPawnsRef()) {
        pawn->move(boardViewModel->getBlackPawnsRef()+boardViewModel->getWhitePawnsRef());
    }

    for (auto pawn : boardViewModel->getWhitePawnsRef()) {
        pawn->move(boardViewModel->getBlackPawnsRef()+boardViewModel->getWhitePawnsRef());
    }

    switch (playerType) {
    case PlayerType::white:
        for (auto blackPawn : boardViewModel->getBlackPawnsRef()) {
            for (auto pos : blackPawn->posThatCanMove) {
                allLegalMoves.push_back(TriplePos(blackPawn->type,blackPawn->position, pos));
            }
        }
        break;
    case PlayerType::black:
        for (auto blackPawn : boardViewModel->getWhitePawnsRef()) {
            for (auto pos : blackPawn->posThatCanMove) {
                allLegalMoves.push_back(TriplePos(blackPawn->type,blackPawn->position, pos));
            }
        }
        break;
    default:
        break;
    }

    switch (playerType) {
    case PlayerType::white:
        for (auto move : allLegalMoves) {
            for (auto blackPawn : boardViewModel->getBlackPawnsRef()) {
                if (blackPawn->type == move.type && blackPawn->position == move.oldPos) {
                    if (boardViewModel->getPawnOnBoardPosition(move.newPos)) {
                        auto oldWhitePawn = boardViewModel->getPawnOnBoardPosition(move.newPos);
                        boardViewModel->getWhitePawnsRef().removeOne(oldWhitePawn);
                        blackPawn->position = move.newPos;
                        if (!checkIsKingUnderCheck(playerType)) {
                            blackPawn->position = move.oldPos;
                            boardViewModel->getWhitePawnsRef().push_back(oldWhitePawn);
                            allLegalMoves.removeOne(move);

                            boardViewModel->getBlackPawnsRef() = blackPawns;
                            boardViewModel->getWhitePawnsRef() = whitePawns;

                            return false;
                        }
                        blackPawn->position = move.oldPos;
                        boardViewModel->getWhitePawnsRef().push_back(oldWhitePawn);
                        allLegalMoves.removeOne(move);

                        boardViewModel->getBlackPawnsRef() = blackPawns;
                        boardViewModel->getWhitePawnsRef() = whitePawns;
                    }
                    else {
                        blackPawn->position = move.newPos;
                        if (!checkIsKingUnderCheck(playerType)) {
                            blackPawn->position = move.oldPos;
                            allLegalMoves.removeOne(move);

                            boardViewModel->getBlackPawnsRef() = blackPawns;
                            boardViewModel->getWhitePawnsRef() = whitePawns;

                            return false;
                        }
                        blackPawn->position = move.oldPos;
                        allLegalMoves.removeOne(move);

                        boardViewModel->getBlackPawnsRef() = blackPawns;
                        boardViewModel->getWhitePawnsRef() = whitePawns;
                    }
                }
            }
        }
        break;
    case PlayerType::black:
        for (auto move : allLegalMoves) {
            for (auto blackPawn : boardViewModel->getWhitePawnsRef()) {
                if (blackPawn->type == move.type && blackPawn->position == move.oldPos) {
                    if (boardViewModel->getPawnOnBoardPosition(move.newPos)) {
                        auto oldWhitePawn = boardViewModel->getPawnOnBoardPosition(move.newPos);
                        boardViewModel->getBlackPawnsRef().removeOne(oldWhitePawn);
                        blackPawn->position = move.newPos;
                        if (!checkIsKingUnderCheck(playerType)) {
                            blackPawn->position = move.oldPos;
                            boardViewModel->getBlackPawnsRef().push_back(oldWhitePawn);
                            allLegalMoves.removeOne(move);

                            boardViewModel->getBlackPawnsRef() = blackPawns;
                            boardViewModel->getWhitePawnsRef() = whitePawns;

                            return false;
                        }
                        blackPawn->position = move.oldPos;
                        boardViewModel->getBlackPawnsRef().push_back(oldWhitePawn);
                        allLegalMoves.removeOne(move);

                        boardViewModel->getBlackPawnsRef() = blackPawns;
                        boardViewModel->getWhitePawnsRef() = whitePawns;
                    }
                    else {
                        blackPawn->position = move.newPos;
                        if (!checkIsKingUnderCheck(playerType)) {
                            blackPawn->position = move.oldPos;
                            allLegalMoves.removeOne(move);

                            boardViewModel->getBlackPawnsRef() = blackPawns;
                            boardViewModel->getWhitePawnsRef() = whitePawns;

                            return false;
                        }
                        blackPawn->position = move.oldPos;
                        allLegalMoves.removeOne(move);

                        boardViewModel->getBlackPawnsRef() = blackPawns;
                        boardViewModel->getWhitePawnsRef() = whitePawns;
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    if (isKingUnderCheckInitially && allLegalMoves.empty()) {
        return true;
    }

    return false;
}
