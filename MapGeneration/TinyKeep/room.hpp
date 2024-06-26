#ifndef TINYKEEP_ROOM
#define TINYKEEP_ROOM

#include<raylib.h>

namespace TinyKeep {

  struct Room {
    float x;
    float y;
    float width;
    float height;
    bool mainRoom;
    double middlex;
    double middley;
    long id;

    bool isCollidingWidth(Room& other);
    void drawRoom(int tileSize);
  };
}
#endif
