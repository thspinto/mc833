#include "easylogging++.h"
#include "Server.h"

INITIALIZE_EASYLOGGINGPP

int main() {
    LOG(INFO) << "Welcome to cMessenger";

    Server server = Server();
    server.run(4444);

    return 0;
}