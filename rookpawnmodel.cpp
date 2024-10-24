#include "rookpawnmodel.h"

RookPawnModel::RookPawnModel(BoardPosition position, PlayerType owner, PawnType type, QString imagePath): BasePawnModel (position, owner, type, imagePath) {}

bool RookPawnModel::validateMove(BoardPosition positionToMove, BasePawnModel *pawnOnPositionToMove, BoardPosition *requestedActivePawnPosition) {
    if (pawnOnPositionToMove && pawnOnPositionToMove->owner == this->owner) {
        return false;
    }

    return validateVerticalOrHorizontalMove(positionToMove, pawnOnPositionToMove);
}

void RookPawnModel::move(QList<BasePawnModel*> pawns)
{
    posThatCanMove.clear();

    BoardPosition temp;
    bool flag;

    //horizontal and vertical moves

    for (int i = 1; i < 8; i++){
        flag = false;
        temp.x = position.x;
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
        temp.x = position.x + i;
        temp.y = position.y;
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
        temp.x = position.x;
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
        temp.x = position.x - i;
        temp.y = position.y;
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
