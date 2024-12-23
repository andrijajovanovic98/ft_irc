#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "konstantVariables.hpp"
#include <string.h>
#include <sys/epoll.h>
#include "Client.hpp"
#include <map>
#include <cstdio>
#include "CommandHandler.hpp"
#include "Channel.hpp"
#include <csignal>
#include <sys/ioctl.h>


#include "TicTacToeBot.hpp"


class CommandHandler;
class Client;
class Channel;
class TicTacToeBot;

class Server;

extern Server* global_server;


class Server {
private:

	int server_fd;
	int epoll_fd;
	void setupServerSocket(int port);
	void setupEpoll();
	CommandHandler* commandHandler;
	std::map<std::string, Channel> _channels;
	int port;
	std::string password;
	int channelCounter;
	int userCounter;
    TicTacToeBot ticTacToeBot;


public:

    std::map<int, Client> _clients;
		
	Server(int port, const std::string &password);
	~Server();
	void shutdownServer();
	void run();

    void handleNewConnection();
	void handleClientData(int client_fd);
	void disconnectClient(int fd);


	int joinChannel(const std::string& command, const std::string& channelName, Client* client);
	void leaveChannel(const std::string& channelName, Client *client);
	void listChannels(Client* client);
	void listChannelMembers(const std::string& channelName, Client* requester);

	void kickCommand(Client* client);

	Channel* getChannelByName(const std::string& name);
	Client* getClientByNickname(const std::string& nickname);


	void handlingCommands(const std::string& command, Client* client, Server* server);
	bool isAdminCommand(const std::string& command);
	

	void setPort(int port);
	std::string getServerPassword() const;
	bool isValidMessage(const std::string& message);

	std::string commandTranslate(const std::string& command);

	void broadcastToChannel(const std::string& channelName, const std::string& message, Client* sender);


	int irssiAutomaticCommands(Client &client, std::string &message, int &client_fd, std::string password);
	bool isNickNameTaken(std::string nickname);
	
	// user and channel limitation

	void clientCounterServer(char sign);
	int getNumOfClientsServer();
	void channelCounterf(char sign);
	int getNumOfChannels();

	void rejectClient(int client_fd, std::string message);	
	void removeClient(int client_fd);
	void leaveAllChannels(Client* client);

	bool isUserNameTaken(std::string nickname);


};

std::string ft_trim(const std::string& str);
int getNumOfArgument(const std::string& command);
std::string getArgument(const std::string& command, int arg);
void signalHandler(int signal);
