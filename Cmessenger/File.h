//
// Created by guilherme on 29/06/16.
//

#ifndef CMESSENGERCLIENT_FILE_H
#define CMESSENGERCLIENT_FILE_H


#include <string>
#include <unistd.h>
#include <iostream>

class File {
    char *file_path;
    int socketfd;

    File(int socketfd, char *name):socketfd(socketfd), file_path(name){}

    int sendFile();

    int recvFile();
};


#endif //CMESSENGERCLIENT_FILE_H
