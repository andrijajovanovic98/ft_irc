#include "../CommandHandler.hpp"

int CommandHandler::privmsgCommand(const std::string& command, Client* client, Server* server) {
    std::string trimmedCommand = ft_trim(command);

    std::istringstream iss(trimmedCommand);
    std::string commandWord, recipient, message;
    iss >> commandWord >> recipient;

    if (recipient.empty()) {
        client->sendMessage("Error: No recipient is specified in the PRIVMSG command.\n");
        return 1;
    }
    
    size_t colonPos = trimmedCommand.find(" :");
    if (colonPos == std::string::npos) {
        client->sendMessage("Error: The message part of the PRIVMSG command must start with a colon (:).\n");
        return 1;
    }
    
    message = trimmedCommand.substr(colonPos + 2);
    if (message.empty()) {
        client->sendMessage("Error: The message cannot be blank.\n");
        return 1;
    }

    if (recipient[0] == '#') {
        
        std::string channelName = recipient;
        Channel* channel = server->getChannelByName(channelName);
        if (!channel) {
            client->sendMessage(":localhost 403 " + channelName + " :No such channel\r\n");
            return -1;
        }

        if (!client->FindGetOnChannels(channelName)) {
            client->sendMessage(":localhost 403 " + channelName + " :You are not on the channel\r\n");
            return -1;
        }

        std::string formattedMessage = ":" + client->getNickname() + "!" +
                                       client->getUserName() + "@localhost PRIVMSG " +
                                       channelName + " :" + message + "\r\n";
        channel->broadcastMessage(formattedMessage, client);
    } else {
        
        Client* recipientClient = server->getClientByNickname(recipient);
        if (!recipientClient) {
            client->sendMessage(":localhost 401 " + recipient + " :No such nick/channel\r\n");
            return -1;
        }

        std::string formattedMessage = ":" + client->getNickname() + "!" +
                                       client->getUserName() + "@localhost PRIVMSG " +
                                       recipient + " :" + message + "\r\n";
        recipientClient->sendMessage(formattedMessage);
    }

    return 0;
}
