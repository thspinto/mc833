#include "easylogging++.h"
#include "Server.h"


INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {
    int port = 4444;
    LOG(INFO) << "Welcome to cMessenger";

    if(argc > 1) {
        std::istringstream iss(argv[1]);
        if (!(iss >> port)) {
            LOG(WARNING) << "Invalid port " << argv[1] << '\n';
            std::cout << "usage: server <port>";
        }
    }
    Server server = Server();
    server.run(4444);

    return 0;
}