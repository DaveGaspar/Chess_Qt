#include "boardviewmodel.h"
#include "boardview.h"
#include "boardfield.h"
#include "kingpawnmodel.h"
#include "queenpawnmodel.h"
#include "rookpawnmodel.h"
#include "bishoppawnmodel.h"
#include "knightpawnmodel.h"
#include "pawnpawnmodel.h"
#include <math.h>

BoardViewModel::BoardViewModel(bool isGameModeEtudes) {
    activePawn = nullptr;
    whosTurn = PlayerType::white;
    isEnPassantAvailable = true;
    pawnViewModel = PawnViewModel();
    winner = nullptr;

    if (isGameModeEtudes) {
        initializePawnsEtudes();
    }
    else {
        initializePawns();
    }
}

QList<BasePawnModel*>& BoardViewModel::getBlackPawnsRef() {
    return blackPawns;
}

QList<BasePawnModel*>& BoardViewModel::getWhitePawnsRef() {
    return whitePawns;
}

QList<BasePawnModel*> BoardViewModel::getBlackPawns() {
    return blackPawns;
}

QList<BasePawnModel*> BoardViewModel::getWhitePawns() {
    return whitePawns;
}

BasePawnModel* BoardViewModel::getActivePawn() {
    return activePawn;
}

PlayerType BoardViewModel::getWhosTurn() {
    return whosTurn;
}

PlayerType* BoardViewModel::getWinner() {
    return winner;
}

void BoardViewModel::setActivePawnForField(PawnField *pawn) {
    BasePawnModel* pawnModel = getPawnOnBoardPosition(pawn->getPosition());
    if (constIsGameModeEtudes) {
        activePawn = pawnModel;
        pawn->setZValue(1);
        return;
    }

    if (pawnModel && pawnModel->owner == whosTurn) {
        activePawn = pawnModel;
        pawn->setZValue(1);
    }
}

void BoardViewModel::setNewPositionForActivePawn(BoardPosition position) {
    activePawn->didTakeFirstMove = true;
    activePawn->position = position;
}

void BoardViewModel::discardActivePawn() {
    activePawn = nullptr;
}

BasePawnModel* BoardViewModel::getPawnOnBoardPosition(BoardPosition baordPosition) {
    for (int i = 0; i < blackPawns.length(); i++) {
        BasePawnModel *pawnModel = blackPawns[i];
        if (baordPosition.x == pawnModel->position.x &&
                baordPosition.y == pawnModel->position.y) {
            return pawnModel;
        }
    }

    for (int i = 0; i < whitePawns.length(); i++) {
        BasePawnModel *pawnModel = whitePawns[i];
        if (baordPosition.x == pawnModel->position.x &&
                baordPosition.y == pawnModel->position.y) {
            return pawnModel;
        }
    }

    return nullptr;
}

bool BoardViewModel::validatePawnPalcementForMousePosition(QPoint point) {
    if (point.x() > BoardView::startXPosition &&
            point.x() < (BoardView::startXPosition + BoardField::defaultWidthHeight*BoardView::numberOfRowsColumns) &&
            point.y() > BoardView::startYPosition &&
            point.y() < (BoardView::startYPosition + BoardField::defaultWidthHeight*BoardView::numberOfRowsColumns)) {
        return true;
    }
    return false;
}

bool BoardViewModel::validatePawnMove(BoardPosition positionToMove,
                                      BasePawnModel *pawn,
                                      BoardPosition *requestedActivePawnPosition) {
    BasePawnModel *pawnToValidate;
    if (pawn) {
        pawnToValidate = pawn;
    } else {
        pawnToValidate = activePawn;
    }

    BasePawnModel *pawnOnPositionToMove = getPawnOnBoardPosition(positionToMove);
    bool isMoveValid = pawnToValidate->validateMove(positionToMove, pawnOnPositionToMove, requestedActivePawnPosition);

    switch (pawnToValidate->type) {
    case PawnType::king:
    case PawnType::queen:
    case PawnType::rook:
    case PawnType::bishop:
    case PawnType::pawn:
        return isMoveValid && validateAnotherPawnIntersection(positionToMove, pawnToValidate, requestedActivePawnPosition);
    case PawnType::knight:
        return isMoveValid;
    }
}

bool BoardViewModel::didRemoveEnemyOnBoardPosition(BoardPosition boardPosition) {
    BasePawnModel *pawn = getPawnOnBoardPosition(boardPosition);
    if (constIsGameModeEtudes){
        if (pawn) {
            // switch (whosTurn) {
            switch (pawn->owner) {
            case PlayerType::white: {
                int index = whitePawns.indexOf(pawn);
                whitePawns.removeAt(index);
            }
            break;
            case PlayerType::black: {
                int index = blackPawns.indexOf(pawn);
                blackPawns.removeAt(index);
            }
            break;
            }
            delete pawn;
            return true;
        }
        return false;
    }

    if (pawn && pawn->owner == whosTurn) {
        return false;
    }
    if (pawn) {
        switch (whosTurn) {
        case PlayerType::black: {
            int index = whitePawns.indexOf(pawn);
            whitePawns.removeAt(index);
        }
        break;
        case PlayerType::white: {
            int index = blackPawns.indexOf(pawn);
            blackPawns.removeAt(index);
        }
        break;
        }

        if (pawn->type == PawnType::king) {
            winner = &whosTurn;
        }
        delete pawn;
        return true;
    }
    return false;
}

bool BoardViewModel::isKingInCheck(PlayerType owner,
                                   bool isCheckingActivePlayer,
                                   BoardPosition positionToMoveActivePlayer) {
    BasePawnModel *king = nullptr;

    if (isCheckingActivePlayer && activePawn->type == PawnType::king) {
        king = activePawn;
    } else {
        switch (owner) {
        case PlayerType::black:
            for (int i = 0; i < blackPawns.length(); i++) {
                BasePawnModel *pawn = blackPawns[i];
                if (pawn->type == PawnType::king) {
                    king = pawn;
                }
            }
            break;
        case PlayerType::white:
            for (int i = 0; i < whitePawns.length(); i++) {
                BasePawnModel *pawn = whitePawns[i];
                if (pawn->type == PawnType::king) {
                    king = pawn;
                }
            }
            break;
        }
    }

    if (king) {
        bool isInCheck = false;

        switch (owner) {
        case PlayerType::black:
            isInCheck = validateKingsCheckForPawns(whitePawns, isCheckingActivePlayer, king, positionToMoveActivePlayer);
            break;
        case PlayerType::white:
            isInCheck = validateKingsCheckForPawns(blackPawns, isCheckingActivePlayer, king, positionToMoveActivePlayer);
        }

        return isInCheck;
    }

    return false;
}

bool BoardViewModel::didPromoteActivePawn() {
    if (!activePawn) {
        return false;
    }

    if (activePawn->type != PawnType::pawn) {
        return false;
    }

    switch (activePawn->owner) {
    case PlayerType::black:
        if (activePawn->position.y == 7) {
            BoardPosition boardPosition = activePawn->position;
            PawnType type = pawnViewModel.getTypeForInitialPosition(boardPosition);
            QString imagePath = pawnViewModel.getImagePath(type, PlayerType::black);
            auto temp = activePawn;
            activePawn = new QueenPawnModel(boardPosition, PlayerType::black, type, imagePath);
            // delete temp;
            blackPawns.removeIf([&](BasePawnModel* pawn){
                return pawn == temp;
            });
            blackPawns.push_back(activePawn);
            activePawn->type = PawnType::queen;
            return true;
        }
        break;
    case PlayerType::white:
        if (activePawn->position.y == 0) {
            BoardPosition boardPosition = activePawn->position;
            PawnType type = pawnViewModel.getTypeForInitialPosition(boardPosition);
            QString imagePath = pawnViewModel.getImagePath(type, PlayerType::white);
            auto temp = activePawn;
            activePawn = new QueenPawnModel(boardPosition, PlayerType::white, type, imagePath);
            // delete temp;
            whitePawns.removeIf([&](BasePawnModel* pawn){
                return pawn == temp;
            });
            whitePawns.push_back(activePawn);
            activePawn->type = PawnType::queen;
            return true;
        }
        break;
    }

    return false;
}

void BoardViewModel::switchRound() {
    switch (whosTurn) {
    case PlayerType::black:
        whosTurn = PlayerType::white;
        break;
    case PlayerType::white:
        whosTurn = PlayerType::black;
        break;
    }
}

BoardPosition BoardViewModel::getBoardPositionForMousePosition(QPoint point) {
    int xPosition = static_cast<int>(floor((point.x() - BoardView::startXPosition)/BoardField::defaultWidthHeight));
    int yPosition = static_cast<int>(floor((point.y() - BoardView::startYPosition)/BoardField::defaultWidthHeight));

    return BoardPosition { xPosition, yPosition };
}

void BoardViewModel::initializePawnsEtudes() {
    BoardPosition boardPosition = { 5, 5 };
    QString imagePath = pawnViewModel.getImagePath(PawnType::king, PlayerType::white);
    BasePawnModel *pawn = new KingPawnModel(boardPosition, PlayerType::white, PawnType::king, imagePath);

    whitePawns.append(pawn);

    BoardPosition boardPosition1 = { 7, 4 };
    QString imagePath1 = pawnViewModel.getImagePath(PawnType::king, PlayerType::black);
    BasePawnModel *pawn1 = new KingPawnModel(boardPosition1, PlayerType::black, PawnType::king, imagePath1);

    blackPawns.append(pawn1);

    BoardPosition boardPosition2 = { 0, 2 };
    QString imagePath2 = pawnViewModel.getImagePath(PawnType::queen, PlayerType::white);
    BasePawnModel *pawn2 = new QueenPawnModel(boardPosition2, PlayerType::white, PawnType::queen, imagePath2);

    whitePawns.append(pawn2);

    BoardPosition boardPosition3 = { 1, 1 };
    QString imagePath3 = pawnViewModel.getImagePath(PawnType::bishop, PlayerType::white);
    BasePawnModel *pawn3 = new BishopPawnModel(boardPosition3, PlayerType::white, PawnType::bishop, imagePath3);

    whitePawns.append(pawn3);

    BoardPosition boardPosition4 = { 3, 2 };
    QString imagePath4 = pawnViewModel.getImagePath(PawnType::knight, PlayerType::white);
    BasePawnModel *pawn4 = new KnightPawnModel(boardPosition4, PlayerType::white, PawnType::knight, imagePath4);

    whitePawns.append(pawn4);

    BoardPosition boardPosition5 = { 6, 1 };
    QString imagePath5 = pawnViewModel.getImagePath(PawnType::rook, PlayerType::white);
    BasePawnModel *pawn5 = new RookPawnModel(boardPosition5, PlayerType::white, PawnType::rook, imagePath5);

    whitePawns.append(pawn5);

    // BoardPosition boardPosition6 = { 2, 6 };
    // QString imagePath6 = pawnViewModel.getImagePath(PawnType::pawn, PlayerType::white);
    // BasePawnModel *pawn6 = new PawnPawnModel(boardPosition6, PlayerType::white, PawnType::pawn, imagePath6);

    // whitePawns.append(pawn6);

    // initializePawnsForRow(0, PlayerType::black);
    // initializePawnsForRow(1, PlayerType::black);
    // initializePawnsForRow(6, PlayerType::white);
    // initializePawnsForRow(7, PlayerType::white);
}

void BoardViewModel::initializePawns() {
    initializePawnsForRow(0, PlayerType::black);
    initializePawnsForRow(1, PlayerType::black);
    initializePawnsForRow(6, PlayerType::white);
    initializePawnsForRow(7, PlayerType::white);
}

void BoardViewModel::initializePawnsForRow(int rowNumber, PlayerType owner) {
    for (int i = 0; i < BoardView::numberOfRowsColumns; i++) {
        BoardPosition boardPosition = { i, rowNumber };
        PawnType type = pawnViewModel.getTypeForInitialPosition(boardPosition);
        QString imagePath = pawnViewModel.getImagePath(type, owner);
        BasePawnModel *pawn;

        switch (type) {
        case PawnType::king:
            pawn = new KingPawnModel(boardPosition, owner, type, imagePath);
            break;
        case PawnType::queen:
            pawn = new QueenPawnModel(boardPosition, owner, type, imagePath);
            break;
        case PawnType::rook:
            pawn = new RookPawnModel(boardPosition, owner, type, imagePath);
            break;
        case PawnType::bishop:
            pawn = new BishopPawnModel(boardPosition, owner, type, imagePath);
            break;
        case PawnType::knight:
            pawn = new KnightPawnModel(boardPosition, owner, type, imagePath);
            break;
        case PawnType::pawn:
            pawn = new PawnPawnModel(boardPosition, owner, type, imagePath);
            break;
        }

        switch (owner) {
        case PlayerType::black:
            blackPawns.append(pawn);
            break;
        case PlayerType::white:
            whitePawns.append(pawn);
            break;
        }
    }
}

bool BoardViewModel::validateAnotherPawnIntersection(BoardPosition positionToMove,
                                                     BasePawnModel *pawnToValidate,
                                                     BoardPosition *requestedActivePawnPosition) {
    int xDiference = positionToMove.x - pawnToValidate->position.x;
    int yDiference = positionToMove.y - pawnToValidate->position.y;
    int numbeOfFieldsToCheck = std::max(abs(xDiference), abs(yDiference));

    if (numbeOfFieldsToCheck == 1) {
        return true;
    }

    for (int i = 0; i < numbeOfFieldsToCheck; i++) {
        BoardPosition positionToCheck;

        if (xDiference < 0) {
            if (yDiference == 0) {
                positionToCheck = { pawnToValidate->position.x + (xDiference + i), pawnToValidate->position.y };
            } else if (yDiference < 0) {
                positionToCheck = { pawnToValidate->position.x + (xDiference + i), pawnToValidate->position.y + (yDiference + i) };
            } else {
                positionToCheck = { pawnToValidate->position.x + (xDiference + i), pawnToValidate->position.y + (yDiference - i) };
            }
        } else if (yDiference < 0) {
            if (xDiference == 0) {
                positionToCheck = { pawnToValidate->position.x, pawnToValidate->position.y + (yDiference + i) };
            } else {
                positionToCheck = { pawnToValidate->position.x + (xDiference - i), pawnToValidate->position.y + (yDiference + i) };
            }
        } else {
            if (xDiference == 0) {
                positionToCheck = { pawnToValidate->position.x, pawnToValidate->position.y + (yDiference - i) };
            } else if (yDiference == 0) {
                positionToCheck = { pawnToValidate->position.x + (xDiference - i), pawnToValidate->position.y };
            } else {
                positionToCheck = { pawnToValidate->position.x + (xDiference - i), pawnToValidate->position.y + (yDiference - i) };
            }
        }

        BasePawnModel *pawnToCheck = getPawnOnBoardPosition(positionToCheck);

        if (requestedActivePawnPosition &&
                positionToCheck.x != positionToMove.x &&
                positionToCheck.y != positionToMove.y &&
                positionToCheck.x == requestedActivePawnPosition->x &&
                positionToCheck.y == requestedActivePawnPosition->y) {
            return false;
        }

        // if we check for pawn that is on the position that the pawn wants to move by we need to check
        // if this position is not position that the pawm wants to move to
        if (pawnToCheck &&
                (pawnToCheck->position.x != positionToMove.x || pawnToCheck->position.y != positionToMove.y)) {
            return false;
        }
    }

    return true;
}

bool BoardViewModel::validateKingsCheckForPawns(QList<BasePawnModel*> pawns,
                                                bool isCheckingActivePlayer,
                                                BasePawnModel *king,
                                                BoardPosition positionToMoveActivePlayer) {
    bool isInCheck = false;

    // check every oppisite players pawn for kings check
    for (int i = 0; i < pawns.length(); i++) {
        BasePawnModel *pawn = pawns[i];
        // when checking for active player check we need to check first if the active player pawn is king,
        // if it's a king then check if position that wants to move the king to
        // is able to be taken in the next move by the opposite player
        if (isCheckingActivePlayer && activePawn->type == PawnType::king) {
            if (validatePawnMove(positionToMoveActivePlayer, pawn, &positionToMoveActivePlayer)) {
                isInCheck = true;
            }
        } else if (isCheckingActivePlayer) {
            // if the active player pawn is not a king then check if active player wants to move to
            // the place of opposite player pawn that could capture active player king in the next move.
            // If he wants to move to other field then check if the field he wants to take will take active
            // player king from the check state

            if ((positionToMoveActivePlayer.x != pawn->position.x || positionToMoveActivePlayer.y != pawn->position.y) &&
                    validatePawnMove(king->position, pawn, &positionToMoveActivePlayer)) {
                isInCheck = true;
            }

        } else if (validatePawnMove(king->position, pawn)) {
            isInCheck = true;
        }
    }

    return isInCheck;
}
