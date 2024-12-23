#include "../Server.hpp"

int channelLimitation(const std::string& command, Client* client, Server *server)
{
    if (server->getChannelByName(getArgument(command,2)) == NULL && client->getCurrentChannel() == "") {
        if (server->getNumOfChannels() == MAX_CHANNELS) {
                client->sendMessage(":" + std::string(SERVER_NAME) + " 403 " + client->getNickname() +
                   " " + getArgument(command, 2) + " :No such channel: maximum number of channels reached\r\n");
        return (-1);
        }
    }
    else if (server->getChannelByName(getArgument(command,2)) == NULL && client->getCurrentChannel() != "") {
    if (server->getNumOfChannels() == MAX_CHANNELS) {
        
         client->sendMessage(":" + std::string(SERVER_NAME) + " 461 " + client->getNickname() +
                        " " + client->getCurrentChannel() + " MODE :Password is needed for +k mod\r\n");
                return -1;
        return (-1);
        }
    }
    return (0);
}

int CommandHandler::joinCommand(const std::string& command, Client* client, Server *server) {

    std::string channelName = getArgument(command,2);

    if (channelLimitation(command, client, server) == -1) {
        return (-1); }


     if (client->getCurrentChannel() == "" && channelName.length() > 40) {
        client->sendMessage(":" + std::string(SERVER_NAME) + " 479 " + client->getNickname() +
                        " " + channelName + " :Erroneous channel name: too long\r\n");
        return -1;
    }

    if (client->getCurrentChannel() != "" && channelName.length() > 40) {
		std::string error = ":" + client->getNickname() + 
                           "!" + client->getUserName() + 
                           "@localhost 482 " + 
                           channelName + 
                           " Erroneous channel name: too long\r\n";        
    			client->sendMessage(error);
        return -1;
    }
		
    if (server->joinChannel(command, channelName, client) == -1)
        return (1);
    client->setCurrentChannel(channelName);
    client->sendMessage("You joined to to channel: " + channelName + "\n");
    
	std::string joinMessage = ":" + client->getNickname() + "!" + client->getUserName() + "@localhost JOIN :" + channelName + "\r\n";
    server->broadcastToChannel(channelName, joinMessage, client);
    return (0);
}

int Server::joinChannel(const std::string& command, const std::string& channelName, Client* client) {

    if (_channels.find(channelName) == _channels.end()) {
        _channels.insert(std::make_pair(channelName, Channel(channelName)));
        channelCounterf('+');
    }

    if (_channels[channelName].isInviteOnly() == true)
    {
        if (_channels[channelName].isUserInvited(client) == false) {
            client->sendMessage(":" + std::string(SERVER_NAME) + " 473 " + client->getNickname() +
                        " " + channelName + " :Cannot join channel (+i) - you must be invited\r\n");
            return -1;
        }
    }

    if (_channels.find(channelName) != _channels.end()) {
        if (_channels[channelName].getPassword() != "") {
            int argcheck = getNumOfArgument(command);
            if (argcheck != 3) {
                
                           client->sendMessage(":" + std::string(SERVER_NAME) + " 461 " + client->getNickname() +
                        " " + channelName + " MODE :Password is needed for +k mod\r\n");
                return -1;

            }
            else {
                if (getArgument(command,3) != _channels[channelName].getPassword()) {
                     client->sendMessage(":" + std::string(SERVER_NAME) + " 461 " + client->getNickname() +
                        " " + channelName + " INNCORRECT PASSWRD\r\n");
                    return -1;
                }
            }
        }

    }
	
    if (_channels[channelName].canJoin() == false)
    {
       client->sendMessage(":" + std::string(SERVER_NAME) + " 471 " + client->getNickname() +
                        " " + channelName +
                        " :Cannot join channel (+l) - channel is full\r\n");
        return (-1);
    }

    _channels[channelName].clientCounter('+');
    client->AddChannelToList(channelName); // new
    std::cout << _channels[channelName].getCurrentUsersCount() << std::endl;
    _channels[channelName].addClient(client);
    if (_channels[channelName].isFirstJoiner == false) {
        _channels[channelName].addAdmin(client);
        client->sendMessage("You are the admin of the channel: " + channelName + "\n");
        _channels[channelName].isFirstJoiner = true;
    }
    return (0);
}
