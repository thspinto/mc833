#include "Message.h"
#include "easylogging++.h"

std::map<std::string, Message::Action> Message::actionMap = {
        {"CONN", Action::CONN}, {"SEND", Action::SEND},
        {"CREATEG", Action::CREATEG}, {"JOING", Action::JOING},
        {"SENDG", Action::SENDG}, {"WHO", Action::WHO}
};

Message::Action Message::parse() {
    int size;
    std::string command = " ";
    std::string tail = " ";

    std::istringstream stream (Message::buf.data());
    stream >> size;
    stream >> command;

    stream.seekg(1, std::ios_base::cur); //pula um espaço em branco
    Message::size -= stream.tellg();

    Message::buf.resize(size);
    stream.read(&Message::buf[0], size);

    return Message::actionMap[command];
}
