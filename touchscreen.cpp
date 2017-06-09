#include "touchscreen.h"

Touchscreen::Touchscreen(int width, int heigth)
{
    dev = libevdev_new();
    struct input_absinfo absInfoX = {
        0, 0, width, 0, 0, 0
    }, absInfoY = {
        0, 0, heigth, 0, 0, 0
    };


    libevdev_set_name(dev, "waveshare");
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, BTN_TOUCH, 0);
    libevdev_enable_event_type(dev, EV_ABS);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_POSITION_X, &absInfoX);
    libevdev_enable_event_code(dev, EV_ABS, ABS_MT_POSITION_Y, &absInfoY);

    libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uinput);
}

Touchscreen::~Touchscreen()
{
    libevdev_uinput_destroy(uinput);
    libevdev_free(dev);
}

void Touchscreen::handleMessage(const Message &message)
{
    if(message.pressed()){
        auto points = message.points();
        libevdev_uinput_write_event(uinput, EV_KEY, BTN_TOUCH, 1);
        for(auto& p : points){
            libevdev_uinput_write_event(uinput, EV_ABS, ABS_MT_POSITION_X, p.x);
            libevdev_uinput_write_event(uinput, EV_ABS, ABS_MT_POSITION_Y, p.y);
            libevdev_uinput_write_event(uinput, EV_SYN, SYN_MT_REPORT, 0);
        }
        libevdev_uinput_write_event(uinput, EV_SYN, SYN_REPORT, 0);
    }else{
        libevdev_uinput_write_event(uinput, EV_KEY, BTN_TOUCH, 0);
        libevdev_uinput_write_event(uinput, EV_SYN, SYN_MT_REPORT, 0);
        libevdev_uinput_write_event(uinput, EV_SYN, SYN_REPORT, 0);
    }
}
