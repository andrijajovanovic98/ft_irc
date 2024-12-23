#pragma once

#include <iostream>
#include "Client.hpp"
#include "Server.hpp"
#include <map>
#include "TicTacToeBot.hpp"
#include "Dcc.hpp"


class Server;
class Client;

class CommandHandler {
private:

    std::map<std::string, int> commandBook;

public: 

	CommandHandler();

	void processCommand(const std::string& command, Client* client, Server* server);
	int adminCommand(const std::string& command, Client* client, Server* server);
	int upperCaseCommand(const std::string& command, Client* client, Server* server);

	bool isCommandCorrect(const std::string& command, Client* client);
	std::string errorMessage(const std::string& command) const;

	// commands
	
	int inviteCommand(const std::string& command, Client* client, Server* server);
	int modeCommand(const std::string& command, Client* client, Server* server);
	int privmsgCommand(const std::string& command, Client* client, Server* server);
	int topicCommand(const std::string& command, Client* client, Server* server);
	int nickCommand(const std::string& command, Client* client, Server *server);
	int joinCommand(const std::string& command, Client* client, Server *server);
	int partCommand(const std::string &command, Client* client, Server *server);
	int kickCommand(std::string command, Client* client, Server *server);
	int dccCommand(const std::string& command, Client* client, Server* server);
	int quitCommand(const std::string& command, Client* client, Server* server);
	int userCommand(const std::string& command, Client* client, Server* server);

};

bool isAlphanumeric(const std::string &str);
