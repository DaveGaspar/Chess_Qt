#include "kingpawnmodel.h"
#include "constants.h"

KingPawnModel::KingPawnModel(BoardPosition position, PlayerType owner, PawnType type, QString imagePath): BasePawnModel(position, owner, type, imagePath) {}

bool KingPawnModel::validateMove(BoardPosition positionToMove, BasePawnModel *pawnOnPositionToMove, BoardPosition *requestedActivePawnPosition) {
    if (pawnOnPositionToMove && pawnOnPositionToMove->owner == this->owner) {
        return false;
    }

    return pawnWantsToMoveByOneField(positionToMove);
}

void KingPawnModel::move(QList<BasePawnModel*> pawns)
{
    posThatCanMove.clear();

    BoardPosition temp;
    temp.x = position.x - 1;
    temp.y = position.y + 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x - 1;
    temp.y = position.y;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x - 1;
    temp.y = position.y - 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x;
    temp.y = position.y - 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x + 1;
    temp.y = position.y - 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x + 1;
    temp.y = position.y;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x + 1;
    temp.y = position.y + 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }
    temp.x = position.x;
    temp.y = position.y + 1;
    if (checkAvailableSquare(temp)){
        posThatCanMove.push_back(temp);
    }

    PlayerType kingOwner {};

    for (auto pawn : pawns) {
        if (pawn->position == position) {
            kingOwner = pawn->owner;
        }
    }

    if (constIsGameModeEtudes) {
        return;
    }

    switch (kingOwner) {
    case PlayerType::white:
        for (auto pawn : pawns) {
            if (pawn->position != position && pawn->owner == PlayerType::white) {
                posThatCanMove.removeAll(pawn->position);
            }
        }
        break;
    case PlayerType::black:
        for (auto pawn : pawns) {
            if (pawn->position != position && pawn->owner == PlayerType::black) {
                posThatCanMove.removeAll(pawn->position);
            }
        }
        break;
    default:
        break;
    }
}
