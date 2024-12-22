#include "../CommandHandler.hpp"

int CommandHandler::topicCommand(const std::string& command, Client* client, Server* server) {

    if (getNumOfArgument(command) < 2) {
        client->sendMessage(":localhost 461 " + client->getNickname() + " USER :Not enough parameters\r\n");
        return 1;
    } 

    std::string channelName = getArgument(command, 2);
    Channel* targetChannel = server->getChannelByName(channelName);
    if (!targetChannel) {
        client->sendMessage("Channel not found: " + channelName + "\n");
        return (1);
    }
    
    if (getNumOfArgument(command) == 2) {
        client->sendMessage("The current topic for channel " + channelName + " is: " + targetChannel->getTopic() + "\n");
        return (0);
    }

    std::string newTopic = getArgument(command, 3);


    newTopic = command.substr(channelName.length() + getArgument(command, 1).length() + 2);
    if (!newTopic.empty() && newTopic[0] == ':') {
    newTopic = newTopic.substr(1);
    }
  
    if (targetChannel->getIsTopicRestricted() == true)
        if (targetChannel->isClientAdminOnServer(client) == false)
        {
              std::string noAdminRight = ":" + client->getNickname() + 
                           "!" + client->getUserName() + 
                           "@localhost 482 " + 
                           channelName + 
                           " :You're not channel operator\r\n";        
    		client->sendMessage(noAdminRight);
            return (1);
        }

    if (newTopic.empty()) {
        std::string currentTopic = targetChannel->getTopic();
        if (currentTopic.empty()) {
            client->sendMessage("The topic for channel " + channelName + " is not set.\n");
        } else {
            client->sendMessage("The current topic for channel " + channelName + " is: " + currentTopic + "\n");
        }
        return (1);
    }

    if (!targetChannel->getIsClientAdminOnServer(client->getFd()) && targetChannel->getIsTopicRestricted()) {
        std::string noAdminRight = ":" + client->getNickname() + 
                               "!" + client->getUserName() + 
                               "@localhost 482 " + 
                               channelName + 
                               " :You are not channel operator\r\n";   
        client->sendMessage(noAdminRight);
        return (1);
    }

    targetChannel->setTopic(newTopic);
    targetChannel->broadcastMessage(":" + client->getNickname() + "!" + client->getUserName() +
                                     "@" + "@localhost" + " TOPIC " + channelName +
                                     " :" + newTopic + "\r\n", client);

    client->sendMessage(":" + std::string(SERVER_NAME) + " 332 " + client->getNickname() +
                        " " + channelName + " :" + newTopic + "\r\n");

	return (1);
}
