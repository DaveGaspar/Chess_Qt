#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QMediaPlayer>
#include <QAudioOutput>

class ActionButton: public QObject, public QGraphicsRectItem  {
    Q_OBJECT
public:
    ActionButton(QString title);
    ~ActionButton();

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    QMediaPlayer* hoverSfxMediaPlayer;
    QMediaPlayer* clickSfxMediaPlayer;
    QAudioOutput* hoverAudioOutput;
    QAudioOutput* clickAudioOutput;

signals:
    void buttonPressed();

private:
    void setBackgroundColor(Qt::GlobalColor color);
};

#endif // ACTIONBUTTON_H
