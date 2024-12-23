#pragma once

#include <iostream>
#include "Client.hpp"
#include <vector>
#include <algorithm>

class Client;

class Channel {

private:

	std::string channelName;
	std::vector<Client *> _clients;
	std::string topic;
	bool isTopicRestricted;
	bool _inviteOnly;
	std::string _password;
	std::vector<Client *> AdminOnChannel;
	std::vector<Client*> _invitedClients;
	int userLimit;
	int currentUsersCount;

public:

	bool isFirstJoiner;
	Channel();
	Channel(const std::string& name);

	void addClient(Client *client);
	void addClientsToInvitedList(Client *client);
	void addAdmin(Client* client);
	
	void setTopic(const std::string &newTopic);
	void setTopicRestricted(bool value);
	void setInviteOnly(bool value);
	void setPassword(const std::string& password);
	void setUserLimit(int limit);

	std::string getChannelname() const;
	std::string getTopic() const;
	std::string getPassword() const;
	bool getIsClientAdminOnServer(int clientFd) const;
	bool getIsTopicRestricted() const;
	int getCurrentUsersCount() const;

	bool isUserInvited(Client* client) const;
	bool isInviteOnly() const;
	bool isClientAdminOnServer(Client* client) const;
	bool isEmpty() const;
	bool isMember(Client *client);

	void removeAdmin(Client* client);
	void removeClient(Client *client);
    void removeUserLimit();

	bool canJoin() const;
	void clientCounter(char sign);
	void broadcastMessage(const std::string& message, Client* sender);

};
