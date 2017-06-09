#ifndef MESSAGE_H
#define MESSAGE_H

#include <array>
#include <map>
#include <list>
#include <iostream>

class Message{
    std::array<unsigned char, 25> messageBuffer;
    const std::map<int, int> indices = {
        {1, 2},
        {2, 8},
        {4, 12},
        {8, 16},
        {0x10, 20}
    };

public:
    struct Point{
        int x;
        int y;
    };

    Message();
    virtual ~Message();

    bool readFrom(std::istream& stream);

    bool pressed() const;

    int pressedButtons() const;

    std::list<Point> points() const;

    unsigned short readCoord(int pos) const;

    Point readPoint(int offset) const;

};


#endif // MESSAGE_H