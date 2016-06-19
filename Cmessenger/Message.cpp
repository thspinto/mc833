#include "Message.h"
#include "easylogging++.h"

std::map<std::string, Message::Action> Message::actionMap = {
        {"CONN", Action::CONN}, {"SEND", Action::SEND},
        {"CREATEG", Action::CREATEG}, {"JOING", Action::JOING},
        {"SENDG", Action::SENDG}, {"WHO", Action::WHO}
};

Message::Action Message::parseAction() {
    int size;
    std::string command = " ";

    std::istringstream stream (Message::buf.data());
    stream >> size;
    stream >> command;

    stream.seekg(1, std::ios_base::cur); //pula um espaço em branco
    Message::size -= stream.tellg();

    Message::buf.resize(size);
    stream.read(&Message::buf[0], size);

    return Message::actionMap[command];
}

std::string Message::parseCommandParameter() {
    std::string command = " ";

    std::istringstream stream (Message::buf.data());
    stream >> command;

    stream.seekg(1, std::ios_base::cur); //pula um espaço em branco
    Message::size -= stream.tellg();

    Message::buf.resize(size);
    stream.read(&Message::buf[0], size);

    return command;
}

void Message::setBuffer(const char* buffer, int size) {
    buf.resize(size);
    buf.insert(buf.begin(), &buffer[0], &buffer[size]);
    Message::size = size;
}

bool Message::sendMessage() {
    if(origin != NULL){
        std::string user = origin->user;
        user.append(" ");
        send(dest->socketfd, user.data(), user.length(), 0);
    }
    send(dest->socketfd, &buf[0], size, 0);
}