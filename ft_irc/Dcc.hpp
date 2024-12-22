#pragma once

#include "Client.hpp"
#include "Server.hpp"
#include <string>

class Server;

class Dcc {
public:
    static int dccCommand(const std::string& command, Client* client, Server* server);
    static int dccSendCommand(const std::string& command, Client* client, Server* server);
    static int dccGetCommand(const std::string& command, Client* client, Server* server);
};
