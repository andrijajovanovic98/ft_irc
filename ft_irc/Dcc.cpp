#include "Dcc.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int Dcc::dccCommand(const std::string& command, Client* client, Server* server) {
    std::string subCommand = getArgument(command, 2);
    if (subCommand == "send") {
        return dccSendCommand(command, client, server);
    } else if (subCommand == "get") {
        return dccGetCommand(command, client, server);
    } else {
        client->sendMessage("Unknown DCC command.\n");
        return 1;
    }
}

int Dcc::dccSendCommand(const std::string& command, Client* client, Server* server) {
    std::string recipientName = getArgument(command, 3);
    std::string fileName = getArgument(command, 4);

    Client* recipient = server->getClientByNickname(recipientName);
    if (!recipient) {
        client->sendMessage("No such user: " + recipientName + "\n");
        return 1;
    }

    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        client->sendMessage("Failed to open file: " + fileName + "\n");
        return 1;
    }

    std::streamsize fileSize = file.tellg();
    file.close();

    std::ostringstream oss;
    oss << "DCC SEND " << fileName << " " << client->getIpAddress() << " " << client->getDccPort() << " " << fileSize << "\n";
    recipient->sendMessage(oss.str());

    client->sendMessage("DCC SEND request sent to " + recipientName + "\n");
    return 0;
}

int Dcc::dccGetCommand(const std::string& command, Client* client, Server* server) {
    (void)server;
    std::string dccId = getArgument(command, 3);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        client->sendMessage("Failed to create socket.\n");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(client->getDccPort());
    inet_pton(AF_INET, client->getIpAddress().c_str(), &serverAddr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        client->sendMessage("Failed to connect to sender.\n");
        close(sockfd);
        return 1;
    }

    char buffer[1024];
    std::ofstream outFile(("received_" + dccId).c_str(), std::ios::binary);
    if (!outFile.is_open()) {
        client->sendMessage("Failed to open file for writing.\n");
        close(sockfd);
        return 1;
    }

    int bytesRead;
    while ((bytesRead = read(sockfd, buffer, sizeof(buffer))) > 0) {
        outFile.write(buffer, bytesRead);
    }

    outFile.close();
    close(sockfd);

    client->sendMessage("DCC GET request processed for ID: " + dccId + "\n");
    return 0;
}
