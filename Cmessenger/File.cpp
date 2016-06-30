#include "File.h"
#define MAX_LINE 256

// Recebe arquivo
int File::recvFile(){
    std::string file_name = file_name.append("/tmp/").append(file_path);
    ssize_t bytesReceived = 0;
    char recvBuff[MAX_LINE];

    FILE *fp;
    fp = fopen(file_name, "ab");
    if(NULL == fp){
        printf("Error opening file");
        return 1;
    }

    while((bytesReceived = read(socketfd, recvBuff, 256)) > 0) {
        fwrite(recvBuff, 1,bytesReceived,fp);
    }

    if(bytesReceived < 0){
        std::cout << "\n Read Error \n";
    }
}

// Envia arquivo
int File::sendFile(){
    FILE *fp = fopen(file_path,"rb");
    if(fp==NULL){
        std::cout << "File opern error";
        return 1;
    }

    while(1){
        unsigned char buff[256]={0};
        int nread = fread(buff,1,256,fp);

        if(nread > 0){
            write(socketfd, buff, nread);
            std::cout << "msg_id arquivo enviado!";
        }

        if (nread < 256){
            if (feof(fp))
                printf("End of file\n");
            if (ferror(fp))
                printf("Error reading\n");
            break;
        }
    }
}

