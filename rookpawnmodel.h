#ifndef ROOKPAWNMODEL_H
#define ROOKPAWNMODEL_H

#include "basepawnmodel.h"

class RookPawnModel: public BasePawnModel {

public:
    RookPawnModel(BoardPosition position, PlayerType owner, PawnType type, QString imagePath);

    bool validateMove(BoardPosition positionToMove, BasePawnModel *pawnOnPositionToMove, BoardPosition *requestedActivePawnPosition) override;

    void move(QList<BasePawnModel*>) override;
};

#endif // ROOKPAWNMODEL_H
