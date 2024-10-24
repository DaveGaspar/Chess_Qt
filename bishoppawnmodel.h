#ifndef BISHOPPAWNMODEL_H
#define BISHOPPAWNMODEL_H

#include "basepawnmodel.h"

class BishopPawnModel: public BasePawnModel {

public:
    BishopPawnModel(BoardPosition position, PlayerType owner, PawnType type, QString imagePath);

    bool validateMove(BoardPosition positionToMove, BasePawnModel *pawnOnPositionToMove, BoardPosition *requestedActivePawnPosition) override;

    void move(QList<BasePawnModel*>) override;
};

#endif // BISHOPPAWNMODEL_H
