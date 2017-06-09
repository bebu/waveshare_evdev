#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <libevdev/libevdev-uinput.h>
#include "message.h"

class Touchscreen
{
private:
    struct libevdev* dev;
    struct libevdev_uinput* uinput;
public:
    Touchscreen(int width, int heigth);
    virtual ~Touchscreen();

    void handleMessage(const Message& message);
};

#endif // TOUCHSCREEN_H