#ifndef TWOKILLONE_EVENTS_H
#define TWOKILLONE_EVENTS_H

enum UserEvent {
    StartGame = 1,
    SetBlack,
    SetWhite
};

// Messages send to JS by C++
enum Msg {
    GameOver = 1
};

#endif //TWOKILLONE_EVENTS_H
