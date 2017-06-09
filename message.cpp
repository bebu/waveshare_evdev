#include "message.h"

Message::Message()
{

}

Message::~Message()
{

}

bool Message::readFrom(std::istream &stream){
    stream.read((char*)messageBuffer.data(), messageBuffer.size());
    return stream.good();
}

bool Message::pressed() const {
    return messageBuffer[1];
}

int Message::pressedButtons() const {
    return messageBuffer[7];
}

std::list<Message::Point> Message::points() const{
    std::list<Point> l;
    int buttons = pressedButtons();
    for(auto index : indices){
        if( buttons & index.first ){
            l.push_back(readPoint(index.second));
        }
    }
    return l;
}

unsigned short Message::readCoord(int pos) const{
    return messageBuffer[pos] << 8 | messageBuffer[pos + 1];
}

Message::Point Message::readPoint(int offset) const {
    int xOffset = offset == 2 ? offset : offset  + 2;
    int yOffset = offset == 2 ? offset + 2: offset;
    Point p = {
        readCoord(xOffset), readCoord(yOffset)
    };
    return p;
}
