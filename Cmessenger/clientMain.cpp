#include "easylogging++.h"
#include "Client.h"


INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {
    int port = 4444;

    if(argc > 1) {
        std::istringstream streamHost(argv[1]);
        if(streamHost.str().length() < 8){
            LOG(WARNING) << "Invalid host " << argv[1] << '\n';
            std::cout << "usage hostname, eg: 143.134.65.23";
        }
        std::istringstream streamPort(argv[2]);
        streamPort >> port;

        std::istringstream streamClient(argv[3]);
        if (!streamClient){
            LOG(WARNING) << "Invalid port " << argv[1] << '\n';
            std::cout << "usage: your name";
        }
    }
    Client client(argv[1], port, argv[3]);
    client.run();

    return 0;
}