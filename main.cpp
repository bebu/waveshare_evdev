#include <thread>
#include <chrono>
#include <mutex>
#include <libudev.h>
#include <signal.h>
#include <string>
#include <fstream>
#include <sstream>

#include "message.h"
#include "touchscreen.h"

using namespace std;

mutex m;
bool loop = true;



struct ci_char_traits : public std::char_traits<char> {
    static char to_upper(char ch) {
        return std::toupper((unsigned char) ch);
    }
    static bool eq(char c1, char c2) {
        return to_upper(c1) == to_upper(c2);
    }
    static bool lt(char c1, char c2) {
        return to_upper(c1) <  to_upper(c2);
    }
    static int compare(const char* s1, const char* s2, size_t n) {
        while ( n-- != 0 ) {
            if ( to_upper(*s1) < to_upper(*s2) ) return -1;
            if ( to_upper(*s1) > to_upper(*s2) ) return 1;
            ++s1; ++s2;
        }
        return 0;
    }
    static const char* find(const char* s, int n, char a) {
        auto const ua (to_upper(a));
        while ( n-- != 0 )
        {
            if (to_upper(*s) == ua)
                return s;
            s++;
        }
        return nullptr;
    }
};

typedef basic_string<char, ci_char_traits> String;

const String wsVid("0EEF");
const String wsPid("0005");

void kill_loop(int sig){
    lock_guard<mutex> lock(m);
    switch (sig) {
    case SIGINT:
    case SIGABRT:
    case SIGTERM:
        loop = false;
        break;
    default:
        break;
    }
}

bool shouldExit(){
    lock_guard<mutex> lock(m);
    return !loop;
}

void input_loop(const string& devicePath){


    Message message;
#ifdef DRY_RUN
    unsigned char dirty[] = {0xaa, 0x01, 0x00, 0xe4 , 0x01, 0x39 , 0xbb, 0x01
                             , 0x01, 0xe0 , 0x03, 0x20 , 0x01, 0xe0 , 0x03, 0x20
                             , 0x01, 0xe0 , 0x03, 0x20 , 0x01, 0xe0 , 0x03, 0x20
                             , 0xcc};
    unsigned char clean[] = {
        0xaa, 0x00 , 0x00, 0x00 , 0x00, 0x00 , 0xbb, 0x00
        , 0x00, 0x00 , 0x00, 0x00 , 0x00, 0x00 , 0x00, 0x00
        , 0x00, 0x00 , 0x00, 0x00 , 0x00, 0x00 , 0x00, 0x00
        , 0x00
    };
    stringstream waveshareHid;
    for(int i = 0; i < 10; i++){

        waveshareHid.write((char*)dirty, 25);
        waveshareHid.write((char*)clean, 25);
    }
    waveshareHid.seekg(0);
#else
    ifstream waveshareHid(devicePath, ifstream::in | ifstream::binary);
    if(!waveshareHid){
        cerr << "failed to open " << devicePath << endl;
        return;
    }
#endif
    Touchscreen touchscreen(800, 480);
    while (!shouldExit()){
        if(!message.readFrom(waveshareHid)){
            cout << "read failed" << endl;
            break;
        }
        touchscreen.handleMessage(message);
    }
}

int main(int , char **)
{
    signal(SIGINT, kill_loop);
    signal(SIGTERM, kill_loop);
    signal(SIGABRT, kill_loop);

#ifdef DRY_RUN
    input_loop("");
#else
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    udev = udev_new();
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    string devicePath;
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        path = udev_device_get_devnode(dev);

        dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
        String vid = udev_device_get_sysattr_value(dev,"idVendor");
        String pid = udev_device_get_sysattr_value(dev, "idProduct");


        udev_device_unref(dev);
        dev = nullptr;
        if(wsVid == vid && wsPid == pid ){
            devicePath = path;
            break;
        }
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    if(devicePath.empty()){
        cerr << "did not fine waveshare device" << endl;
        return -1;
    }
    thread loop_thread(input_loop, devicePath);
    loop_thread.join();
#endif
    return 0;
}
