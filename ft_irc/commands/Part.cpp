#include "../CommandHandler.hpp"

int CommandHandler::partCommand(const std::string &command, Client* client, Server *server) {

	std::string channelName = getArgument(command, 2);
    std::string joinMessage = ":" + client->getNickname() + "!" + client->getUserName() + "@localhost PART :" + channelName + "\r\n";
    client->sendMessage("482 " + client->getNickname() + " " + channelName + " :You left the channel\r\n");

    server->leaveChannel(channelName, client);
    client->setCurrentChannel("");
    client->sendMessage("You left the channel: " + channelName + "\r\n");
    if (server->getChannelByName(channelName) != NULL)
    {
        Channel *targetChannel = server->getChannelByName(channelName);
	    targetChannel->clientCounter('-');

    }
    client->RemoveGetOnChannels(channelName);
    
    server->broadcastToChannel(channelName, joinMessage, client);
    return (0);
}
