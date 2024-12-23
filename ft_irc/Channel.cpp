#include "Channel.hpp"

Channel::Channel() : channelName("") {}

Channel::Channel(const std::string& name) : channelName(name) {
	setTopic("Topic is default!");
	setInviteOnly(false);
	setPassword("");
	userLimit = 0;
	currentUsersCount = 0;
	isTopicRestricted = false;
	isFirstJoiner = false;
}

void Channel::addClient(Client *client) {
	if (std::find(_clients.begin(), _clients.end(), client) == _clients.end())
	{
		_clients.push_back(client);	
	}
}

void Channel::removeClient(Client* client) {
    _clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
}

std::string Channel::getChannelname() const {
	return (this->channelName);
}

void Channel::broadcastMessage(const std::string& message, Client* sender) {
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
    	if (*it != sender) {
  	      (*it)->sendMessage(message);
  		}
	}
}

bool Channel::isEmpty() const {
    return _clients.empty();
}

std::string Channel::getClientList() const {
    std::ostringstream oss;
    for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it != _clients.begin()) {
            oss << ", "; 
        }
        oss << (*it)->getNickname();
    }
    return oss.str();
}


std::string Channel::getTopic() const {
	return (this->topic);
}


void Channel::setTopic(const std::string &newTopic) {
	this->topic = newTopic;
}


bool Channel::getIsTopicRestricted() const {
	return (this->isTopicRestricted);
}

void	Channel::setTopicRestricted(bool value) {
	this->isTopicRestricted = value;
}


bool	Channel::getIsClientAdminOnServer(int clientFd) const {

	for (std::vector<Client*>::const_iterator it = AdminOnChannel.begin(); it != AdminOnChannel.end(); ++it)
	{
		if ((*it)->getFd() == clientFd) {
				return true;
			
		}
	}
	return (false);
}

bool Channel::isClientAdminOnServer(Client* client) const {
    return std::find(AdminOnChannel.begin(), AdminOnChannel.end(), client) != AdminOnChannel.end();
}

void Channel::setInviteOnly(bool value) {
    this->_inviteOnly = value;
}

bool Channel::isInviteOnly() const {
    return this->_inviteOnly;
}

void Channel::setPassword(const std::string& password) {
    this->_password = password;
}

std::string Channel::getPassword() const {
    return this->_password;
}

void Channel::addAdmin(Client* client) {
	if (std::find(AdminOnChannel.begin(), AdminOnChannel.end(), client) == AdminOnChannel.end())
	{
		AdminOnChannel.push_back(client);
	}
}

void Channel::removeAdmin(Client* client) {
    std::vector<Client *>::iterator it = std::find(AdminOnChannel.begin(), AdminOnChannel.end(), client);
    if (it != AdminOnChannel.end()) {
        AdminOnChannel.erase(it);
	}
}

void Channel::addClientsToInvitedList(Client *client) {
	if (std::find(_invitedClients.begin(), _invitedClients.end(), client) == _invitedClients.end())
	{
		_invitedClients.push_back(client);
	}
}


bool Channel::isUserInvited(Client* client) const {
    return std::find(_invitedClients.begin(), _invitedClients.end(), client) != _invitedClients.end();
}

void Channel::setUserLimit(int limit)  {
	 userLimit = limit; 
}

void Channel::removeUserLimit() { 
	userLimit = 0; 
}

int Channel::getUserLimit() const { 
	return userLimit;
}

bool Channel::canJoin() const {
    return userLimit == 0 || getCurrentUsersCount() < userLimit;
}

int Channel::getCurrentUsersCount() const {
	return (this->currentUsersCount);
}

void Channel::clientCounter(char sign)
{
	if (sign == '-')
		currentUsersCount--;
	else
		currentUsersCount++;
}
bool Channel::isMember(Client *client) {
    return std::find(_clients.begin(), _clients.end(), client) != _clients.end();

}
