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
	void removeClient(Client *client);
	void setTopic(const std::string &newTopic);
	void setTopicRestricted(bool value);
	void setInviteOnly(bool value);
	void setPassword(const std::string& password);
	void setUserLimitation(bool value);

	std::string getChannelname() const;
	std::string getClientList() const;
	std::string getTopic() const;
	bool getIsClientAdminOnServer(int clientFd) const;
	std::string getPassword() const;

	bool isUserInvited(Client* client) const;
	bool isInviteOnly() const;

	bool getIsTopicRestricted() const;

	void broadcastMessage(const std::string& message, Client* sender);
	bool isEmpty() const;

	void addAdmin(Client* client);
	void removeAdmin(Client* client);

	bool isClientAdminOnServer(Client* client) const;

	//userlimitation

	void setUserLimit(int limit);
    void removeUserLimit();
    int getUserLimit() const;
	int getCurrentUsersCount() const;
	bool canJoin() const;
	void clientCounter(char sign);
	bool isMember(Client *client);

};
