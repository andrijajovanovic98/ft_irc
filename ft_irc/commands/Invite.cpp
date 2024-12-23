#include "../CommandHandler.hpp"

int CommandHandler::inviteCommand(const std::string& command, Client* client, Server* server) {
	
    std::string theInvited;
    
    std::string channelName = getArgument(command, 3);
    Channel* targetChannel = server->getChannelByName(getArgument(command, 3));
     if (!targetChannel) {
        client->sendMessage("Channel not found: " + channelName + "\n");
        return (1);
    }

    if (getNumOfArgument(command) == 3)
    {
         if (!targetChannel->getIsClientAdminOnServer(client->getFd())) {
        std::string noAdminRight = ":" + client->getNickname() + 
                           "!" + client->getUserName() + 
                           "@localhost 482 " + 
                           channelName + 
                           " :You're not channel operator\r\n";        
    		client->sendMessage(noAdminRight);
        return (1);
    }
          
    std::string theInvited = getArgument(command,2);
    for (std::map<int, Client>::iterator it = server->_clients.begin(); it != server->_clients.end(); ++it) {
    if (ft_trim(it->second.getNickname()) == theInvited) {
        std::string theChannel = getArgument(command, 3);
        if (theChannel == "") {
            client->sendMessage("INVITE <clientname> <channelname>\n");    
            return (1);
        }
        std::string channelName = getArgument(command, 3);
        Channel* targetChannel = server->getChannelByName(channelName);
        targetChannel->addClientsToInvitedList(&it->second);
        it->second.sendMessage("You are invited to join to the channel: " + theChannel + "\n");
        return (1);
        }
    }
    }

    std::string error = ":" + client->getNickname() + 
                            "!" + client->getUserName() + 
                            "@localhost 401 " + theInvited + 
                            " :No such nick\r\n";
    client->sendMessage(error);
    return (1);
}
