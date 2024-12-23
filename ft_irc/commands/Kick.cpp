#include "../CommandHandler.hpp"


int CommandHandler::kickCommand(std::string command, Client* client, Server *server) {

	if (getNumOfArgument(command) < 3) {
        client->sendMessage("Not enough parameters given\n");
        return (1);
    }

    std::string channelName = getArgument(command, 2);
    std::string toBeKicked = getArgument(command, 3);
    std::string reason = (getNumOfArgument(command) > 3) ? getArgument(command, 4) : "No reason provided";
    
    if (!reason.empty() && reason[0] == ':') {
    reason = reason.substr(1);
    }

    if (reason.empty()) {
        reason = "No reason provided";
    }

    Channel* targetChannel = server->getChannelByName(channelName);
     if (!targetChannel) {
        client->sendMessage("Channel not found: " + channelName + "\n");
        return (1);
    }
    
    if (!targetChannel->isClientAdminOnServer(client)) {

        std::string noAdminRight = ":" + client->getNickname() + 
                               "!" + client->getUserName() + 
                               "@localhost 482 " + 
                               channelName + 
                               " :You are not channel operator\r\n";        
        client->sendMessage(noAdminRight);
        return (1);
    }

    Client *targetClient = server->getClientByNickname(toBeKicked);
    if (targetClient == NULL) {
        std::string notfound = ":" + client->getNickname() + 
                               "!" + client->getUserName() + 
                               "@localhost 401 " + 
                               channelName + 
                               "\r\n";        
        client->sendMessage(notfound);
        return (1);

    }
    
    if (!targetChannel->isMember(targetClient)) {
        std::string error = ":" + client->getNickname() + 
                            "!" + client->getUserName() + 
                            "@localhost 401 " + targetClient->getNickname() + 
                            " :No such nick\r\n";
        
        client->sendMessage(error);
        return (1);
    }
    
    targetChannel->clientCounter('-');
    targetClient->RemoveGetOnChannels(channelName);
    server->broadcastToChannel(channelName, ":" + client->getUserName() + " KICK " + channelName + " " + toBeKicked + " :" + reason + "\n", NULL);
    targetClient->setCurrentChannel("");
    targetClient->sendMessage("You have been kicked from channel " + channelName + ": " + reason + "\n");
    targetChannel->removeClient(targetClient);
    server->leaveChannel(channelName, targetClient);
    return (0);
}
