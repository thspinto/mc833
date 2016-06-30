#include "Message.h"
#include "easylogging++.h"

std::map<std::string, Message::Action> Message::actionMap = {
        {"CONN", Action::CONN}, {"SEND", Action::SEND},
        {"CREATEG", Action::CREATEG}, {"JOING", Action::JOING},
        {"SENDG", Action::SENDG}, {"WHO", Action::WHO}
};

Message::Action Message::parseAction() {
    int s;
    std::string command = " ";

    Message::buf.push_back('\0');
    std::istringstream stream (Message::buf.data());
    stream >> s;
    stream >> command;

    stream.seekg(1, std::ios_base::cur); //pula um espaço em branco
    Message::size -= stream.tellg();

    Message::buf.resize(size);
    stream.read(&Message::buf[0], s);
    Message::buf.push_back('\0');

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
    Message::buf.push_back('\0');

    return command;
}

void Message::setBuffer(const char* buffer, int size) {
    buf.resize(size);
    buf.insert(buf.begin(), &buffer[0], &buffer[size]);
    Message::size = size;
}

std::string Message::toString() {
    return std::string(buf.begin(), buf.end());
}

bool Message::sendMessage() {
    std::string user = "";
    if(origin != NULL){
        user.append("[");
        user.append(origin->user);
        if(groupHeader.length() > 0) {
            user.append("@");
            user.append(groupHeader);
        }
        user.append(">] ");
    }
    int s = user.size() + size;
    user.append(std::string(buf.begin(), buf.end()));
    std::string m = user.substr(0, s).append("\n");
    send(dest->socketfd, &m[0], s+1, 0);
}