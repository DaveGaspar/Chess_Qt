#ifndef BOARDPOSITION_H
#define BOARDPOSITION_H

#include <QMultiHash>

struct BoardPosition {
    int x;
    int y;

    inline bool operator ==(const BoardPosition& oth) const{
        return (this->x == oth.x && this->y == oth.y);
    }

    bool operator !=(const BoardPosition& oth){
        return !((*this)==oth);
    }
};

// inline bool operator==(const BoardPosition &e1, const BoardPosition &e2)
// {
//     return e1.x == e2.x
//            && e1.y == e2.y;
// }

inline size_t qHash(const BoardPosition &key, size_t seed)
{
    return qHashMulti(seed, key.x, key.y);
}

#endif // BOARDPOSITION_H
