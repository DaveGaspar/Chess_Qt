#include "pawnpawnmodel.h"

PawnPawnModel::PawnPawnModel(BoardPosition position, PlayerType owner, PawnType type, QString imagePath): BasePawnModel (position, owner, type, imagePath) {}

/*
    Pawns cannot move backwards.
    Normally a pawn moves by advancing a single square, but the first time a pawn moves, it has the option of advancing two squares
    Pawns may not use the initial two-square advance to jump over an occupied square, or to capture
    Any piece immediately in front of a pawn, friend or foe, blocks its advance
*/
bool PawnPawnModel::validateMove(BoardPosition positionToMove, BasePawnModel *pawnOnPositionToMove, BoardPosition *requestedActivePawnPosition) {
    if (pawnOnPositionToMove && pawnOnPositionToMove->owner == this->owner) {
        return false;
    }

    int xDiference = positionToMove.x - this->position.x;
    int yDiference = positionToMove.y - this->position.y;
    int numbeOfFieldsToMove = std::max(abs(xDiference), abs(yDiference));

    bool wantsToMoveByOneField = (numbeOfFieldsToMove == 1);

    if (abs(xDiference) > 1 || abs(yDiference) > 2) {
        return false;
    }

    if (pawnOnPositionToMove && abs(xDiference) == 0) {
        return false;
    }

    if ( !wantsToMoveByOneField && this->didTakeFirstMove) {
        return false;
    }

    bool wantsToMoveInGoodDirection;

    switch (this->owner) {
    case PlayerType::black:
        wantsToMoveInGoodDirection = yDiference > 0;
        break;
    case PlayerType::white:
        wantsToMoveInGoodDirection = yDiference < 0;
        break;
    }

    if (wantsToMoveByOneField) {
        if (requestedActivePawnPosition && xDiference == 0)  {
            return (wantsToMoveInGoodDirection &&
                    requestedActivePawnPosition->x != positionToMove.x &&
                    requestedActivePawnPosition->y != positionToMove.y);
        } else if (xDiference == 0) {
            return (wantsToMoveInGoodDirection && !pawnOnPositionToMove);
        } else if (requestedActivePawnPosition) {
            return (wantsToMoveInGoodDirection &&
                    requestedActivePawnPosition->x == positionToMove.x &&
                    requestedActivePawnPosition->y == positionToMove.y) || (wantsToMoveInGoodDirection && pawnOnPositionToMove);
        } else {
            return (wantsToMoveInGoodDirection && pawnOnPositionToMove);
        }
    }

    return (wantsToMoveInGoodDirection &&
            !this->didTakeFirstMove &&
            xDiference == 0);
}

void PawnPawnModel::move(QList<BasePawnModel*> pawns)
{
    posThatCanMove.clear();

    PlayerType pawnOwner {};

    for (auto pawn : pawns) {
        if (position == pawn->position) {
            pawnOwner = pawn->owner;
            break;
        }
    }

    BoardPosition temp;

    switch (pawnOwner) {
    case PlayerType::white:
        temp.x = position.x;
        temp.y = position.y - 1;
        if (checkAvailableSquare(temp)){
            bool isOthPawnOnTemp = false;
            for (auto pawn : pawns) {
                if (temp == pawn->position) {
                    isOthPawnOnTemp = true;
                    break;
                }
            }
            if (!isOthPawnOnTemp) {
                posThatCanMove.push_back(temp);
            }
        }
        break;
    case PlayerType::black:
        temp.x = position.x;
        temp.y = position.y + 1;
        if (checkAvailableSquare(temp)){
            bool isOthPawnOnTemp = false;
            for (auto pawn : pawns) {
                if (temp == pawn->position) {
                    isOthPawnOnTemp = true;
                    break;
                }
            }
            if (!isOthPawnOnTemp) {
                posThatCanMove.push_back(temp);
            }
        }
        break;
    default:
        break;
    }

    if (!this->didTakeFirstMove) {
        switch (pawnOwner) {
        case PlayerType::white:
            temp.x = position.x;
            temp.y = position.y - 2;
            if (checkAvailableSquare(temp)){
                bool isOthPawnOnTemp = false;
                for (auto pawn : pawns) {
                    if (temp == pawn->position) {
                        isOthPawnOnTemp = true;
                        break;
                    }
                }
                if (!isOthPawnOnTemp) {
                    posThatCanMove.push_back(temp);
                }
            }
            break;
        case PlayerType::black:
            temp.x = position.x;
            temp.y = position.y + 2;
            if (checkAvailableSquare(temp)){
                bool isOthPawnOnTemp = false;
                for (auto pawn : pawns) {
                    if (temp == pawn->position) {
                        isOthPawnOnTemp = true;
                        break;
                    }
                }
                if (!isOthPawnOnTemp) {
                    posThatCanMove.push_back(temp);
                }
            }
            break;
        default:
            break;
        }
    }


    switch (pawnOwner) {
    case PlayerType::white:
        temp.x = position.x + 1;
        temp.y = position.y - 1;
        if (checkAvailableSquare(temp)){
            bool isOthPawnOnTemp = false;
            for (auto pawn : pawns) {
                if (temp == pawn->position) {
                    isOthPawnOnTemp = true;
                    break;
                }
            }
            if (isOthPawnOnTemp) {
                posThatCanMove.push_back(temp);
            }
        }
        temp.x = position.x - 1;
        temp.y = position.y - 1;
        if (checkAvailableSquare(temp)){
            bool isOthPawnOnTemp = false;
            for (auto pawn : pawns) {
                if (temp == pawn->position) {
                    isOthPawnOnTemp = true;
                    break;
                }
            }
            if (isOthPawnOnTemp) {
                posThatCanMove.push_back(temp);
            }
        }
        break;
    case PlayerType::black:
        temp.x = position.x - 1;
        temp.y = position.y + 1;
        if (checkAvailableSquare(temp)){
            bool isOthPawnOnTemp = false;
            for (auto pawn : pawns) {
                if (temp == pawn->position) {
                    isOthPawnOnTemp = true;
                    break;
                }
            }
            if (isOthPawnOnTemp) {
                posThatCanMove.push_back(temp);
            }
        }
        temp.x = position.x + 1;
        temp.y = position.y + 1;
        if (checkAvailableSquare(temp)){
            bool isOthPawnOnTemp = false;
            for (auto pawn : pawns) {
                if (temp == pawn->position) {
                    isOthPawnOnTemp = true;
                    break;
                }
            }
            if (isOthPawnOnTemp) {
                posThatCanMove.push_back(temp);
            }
        }
        break;
    default:
        break;
    }


}
