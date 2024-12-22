#include "../CommandHandler.hpp"

int CommandHandler::quitCommand(const std::string& command, Client* client, Server* server) {

	(void)command;

	for (size_t i = 0; i < client->getOnChannels().size(); i++) {
        const std::string& channelName = client->getOnChannels()[i];

        if (!channelName.empty()) {
        Channel* targetChannel = server->getChannelByName(channelName);
        
        if (targetChannel != NULL) {
            targetChannel->clientCounter('-');  
    		std::string joinMessage = ":" + client->getNickname() + "!" + client->getUserName() + "@localhost PART :" + client->getCurrentChannel() + "\r\n";
    		server->broadcastToChannel(client->getCurrentChannel(), joinMessage, client); // nemtom kell e ez a 2 sor
			
            server->leaveChannel(client->getCurrentChannel(), client);
        }
        }
    }
	
	server->disconnectClient(client->getFd());
	return (0);
}
