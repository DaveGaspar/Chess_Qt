#include "knightpawnmodel.h"

KnightPawnModel::KnightPawnModel(BoardPosition position, PlayerType owner, PawnType type, QString imagePath): BasePawnModel (position, owner, type, imagePath) {}

bool KnightPawnModel::validateMove(BoardPosition positionToMove, BasePawnModel *pawnOnPositionToMove, BoardPosition *requestedActivePawnPosition) {
    if (pawnOnPositionToMove && pawnOnPositionToMove->owner == this->owner) {
        return false;
    }

    int xDiference = abs(positionToMove.x - this->position.x);
    int yDiference = abs(positionToMove.y - this->position.y);

    if (xDiference == 2 && yDiference == 1) {
        return true;
    }

    if (xDiference == 1 && yDiference == 2) {
        return true;
    }

    return false;
}

void KnightPawnModel::move(QList<BasePawnModel*>)
{
    posThatCanMove.clear();

    BoardPosition temp;
    temp.x = position.x - 1;
    temp.y = position.y + 2;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x - 2;
    temp.y = position.y + 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x - 2;
    temp.y = position.y - 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x - 1;
    temp.y = position.y - 2;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x + 1;
    temp.y = position.y - 2;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x + 2;
    temp.y = position.y - 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x + 2;
    temp.y = position.y + 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x + 1;
    temp.y = position.y + 2;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
}

