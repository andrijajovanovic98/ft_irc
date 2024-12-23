#pragma once
#include <iostream>
#include <vector>
#include <unistd.h>
#include "Server.hpp"
#include <cstdlib>
#include <ctime>
#include <sstream>

class Server;

class Client {
private:

	int client_fd;
	std::string nickname;
	std::string userName;
	std::string realName;
	std::vector<char> messageBuffer;
	std::vector<std::string> _onChannels;
	std::string currentChannel;
	bool isAdmin;
	std::string ipAddress;
    int dccPort;
	std::string generateRandomNickname();
	bool authenticated;
	std::string buffer;

public:

	int kick;
 	Client(int fd, const std::string& nickname);

    std::string getCurrentChannel() const;
	std::string getNickname() const;
	std::string getUserName() const;
	std::string getRealName() const;
	int getFd() const;
	std::string getIpAddress() const;
    int getDccPort() const;

	void setNickname(const std::string &nickname);
	void setUsername(const std::string &userName);
	std::string generateUsername(int fd);

 	void setCurrentChannel(const std::string &channel);

	bool sendMessage(const std::string& message) const; 

	bool isClientAuthenticated() const;
	void setClientAuthenticated(bool value);

	void AddChannelToList(const std::string& channelName);
	void broadcastNickChange(const std::string& oldNick, const std::string& newNick, Server* server);

	const std::vector<std::string>& getOnChannels() const;
	void RemoveGetOnChannels(const std::string &channelName);
	bool FindGetOnChannels(const std::string &channelName);

	// Client buffer

	void appendToBuffer(const std::string& data);
	std::string getBuffer() const;
	bool hasCompleteCommand() const;
	std::string extractCommand();
	void clearBuffer();
};

int stringToInt(const std::string& str);
std::string intToString(int value);
