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
	const std::vector<std::string>& getOnChannels() const;
 	
	Client(int fd, const std::string& nickname);

    std::string getCurrentChannel() const;
	std::string getNickname() const;
	std::string getUserName() const;
	std::string getRealName() const;
	std::string getIpAddress() const;
	std::string getBuffer() const;
	int getFd() const;
    int getDccPort() const;

	void setNickname(const std::string &nickname);
	void setUsername(const std::string &userName);
 	void setCurrentChannel(const std::string &channel);
	void setClientAuthenticated(bool value);
	std::string generateUsername(int fd);

	bool isClientAuthenticated() const;

	void AddChannelToList(const std::string& channelName);

	void RemoveGetOnChannels(const std::string &channelName);
	bool FindGetOnChannels(const std::string &channelName);

	std::string extractCommand();
	void clearBuffer();
	void appendToBuffer(const std::string& data);
	bool hasCompleteCommand() const;

	bool sendMessage(const std::string& message) const; 
	void broadcastNickChange(const std::string& oldNick, const std::string& newNick, Server* server);
};

int stringToInt(const std::string& str);
std::string intToString(int value);
