#include "actionbutton.h"
#include <QBrush>
#include <QGraphicsRectItem>
#include <QFont>
#include "constants.h"
#include "utils.h"

ActionButton::ActionButton(QString title) {
    setRect(0, 0, 200, 50);

    QColor backgroundColor = QColor(157, 128, 101);
    Utils::setBackgroundColor(backgroundColor, this);

    QColor textColor = QColor(44, 41, 51);
    QGraphicsTextItem *text = Utils::createTextItem(title, 20, textColor, this);
    double xPosition = rect().width()/2 - text->boundingRect().width()/2;
    double yPosition = rect().height()/2 - text->boundingRect().height()/2;

    text->setPos(xPosition, yPosition);

    // allow responding to hover events
    setAcceptHoverEvents(true);

    // setting up sfx

    hoverSfxMediaPlayer = new QMediaPlayer();
    hoverAudioOutput = new QAudioOutput();

    clickSfxMediaPlayer = new QMediaPlayer();
    clickAudioOutput = new QAudioOutput();

    hoverAudioOutput->setVolume(sound);
    clickAudioOutput->setVolume(sound);

    // hover
    hoverSfxMediaPlayer->setSource(QUrl("qrc:/sounds/buttonHover.mp3"));
    hoverSfxMediaPlayer->setAudioOutput(hoverAudioOutput);

    // click
    clickSfxMediaPlayer->setSource(QUrl("qrc:/sounds/buttonClick.wav"));
    clickSfxMediaPlayer->setAudioOutput(clickAudioOutput);
}

ActionButton::~ActionButton()
{
    delete hoverSfxMediaPlayer;
    delete hoverAudioOutput;
    delete clickSfxMediaPlayer;
    delete clickAudioOutput;
}

void ActionButton::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    clickAudioOutput->setVolume(QAudio::convertVolume(sound / qreal(100.0),
                                                      QAudio::LogarithmicVolumeScale,
                                                      QAudio::LinearVolumeScale));
    clickSfxMediaPlayer->play();
    _sleep(200);
    emit buttonPressed();
}

void ActionButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    QColor backgroundColor = QColor(196, 178, 140);
    Utils::setBackgroundColor(backgroundColor, this);
    hoverAudioOutput->setVolume(QAudio::convertVolume(sound / qreal(100.0),
                                                      QAudio::LogarithmicVolumeScale,
                                                      QAudio::LinearVolumeScale));
    if (hoverSfxMediaPlayer->playbackState() == QMediaPlayer::PlayingState){
        hoverSfxMediaPlayer->setPosition(0);
    }
    else if (hoverSfxMediaPlayer->playbackState() == QMediaPlayer::StoppedState) {
        hoverSfxMediaPlayer->play();
    }
}

void ActionButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    QColor backgroundColor = QColor(157, 128, 101);
    Utils::setBackgroundColor(backgroundColor, this);
}
