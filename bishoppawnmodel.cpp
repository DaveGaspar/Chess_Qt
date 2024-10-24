#include "bishoppawnmodel.h"

BishopPawnModel::BishopPawnModel(BoardPosition position, PlayerType owner, PawnType type, QString imagePath): BasePawnModel (position, owner, type, imagePath) {}

bool BishopPawnModel::validateMove(BoardPosition positionToMove, BasePawnModel *pawnOnPositionToMove, BoardPosition *requestedActivePawnPosition) {
    if (pawnOnPositionToMove && pawnOnPositionToMove->owner == this->owner) {
        return false;
    }

    return validateDiagonalMove(positionToMove, pawnOnPositionToMove);
}

void BishopPawnModel::move(QList<BasePawnModel *> pawns)
{
    posThatCanMove.clear();

    BoardPosition temp;
    bool flag;
    for (int i = 1; i < 8; i++){
        flag = false;
        temp.x = position.x - i;
        temp.y = position.y + i;
        if (checkAvailableSquare(temp)){
            for (auto pawn : pawns){
                if (pawn->type == PawnType::king && pawn->owner == PlayerType::black){
                    continue;
                }
                else {
                    if(pawn->position == temp){
                        flag = true;
                        posThatCanMove.push_back(temp);
                        break;
                    }
                }
            }
            if (!flag){
                posThatCanMove.push_back(temp);
            }
        }
        if (flag){
            break;
        }
    }
    for (int i = 1; i < 8; i++){
        flag = false;
        temp.x = position.x - i;
        temp.y = position.y - i;
        if (checkAvailableSquare(temp)){
            for (auto pawn : pawns){
                if (pawn->type == PawnType::king && pawn->owner == PlayerType::black){
                    continue;
                }
                else {
                    if(pawn->position == temp){
                        flag = true;
                        posThatCanMove.push_back(temp);
                        break;
                    }
                }
            }
            if (!flag){
                posThatCanMove.push_back(temp);
            }
        }
        if (flag){
            break;
        }
    }
    for (int i = 1; i < 8; i++){
        flag = false;
        temp.x = position.x + i;
        temp.y = position.y - i;
        if (checkAvailableSquare(temp)){
            for (auto pawn : pawns){
                if (pawn->type == PawnType::king && pawn->owner == PlayerType::black){
                    continue;
                }
                else {
                    if(pawn->position == temp){
                        flag = true;
                        posThatCanMove.push_back(temp);
                        break;
                    }
                }
            }
            if (!flag){
                posThatCanMove.push_back(temp);
            }
        }
        if (flag){
            break;
        }
    }
    for (int i = 1; i < 8; i++){
        flag = false;
        temp.x = position.x + i;
        temp.y = position.y + i;
        if (checkAvailableSquare(temp)){
            for (auto pawn : pawns){
                if (pawn->type == PawnType::king && pawn->owner == PlayerType::black){
                    continue;
                }
                else {
                    if(pawn->position == temp){
                        flag = true;
                        posThatCanMove.push_back(temp);
                        break;
                    }
                }
            }
            if (!flag){
                posThatCanMove.push_back(temp);
            }
        }
        if (flag){
            break;
        }
    }
}
