#ifndef KNIGHTPAWNMODEL_H
#define KNIGHTPAWNMODEL_H

#include "basepawnmodel.h"

class KnightPawnModel: public BasePawnModel {

public:
    KnightPawnModel(BoardPosition position, PlayerType owner, PawnType type, QString imagePath);

    bool validateMove(BoardPosition positionToMove, BasePawnModel *pawnOnPositionToMove, BoardPosition *requestedActivePawnPosition) override;

    void move(QList<BasePawnModel*>) override;
};

#endif // KNIGHTPAWNMODEL_H
