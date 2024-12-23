#include "../CommandHandler.hpp"

void    caseInvite(std::string command, Channel * targetChannel,Client *client, bool addingMode) {
     
    if (getNumOfArgument(command) != 3 || getArgument(command, 3).length() != 2) {
         std::string threeArgNeeded = ":" + client->getNickname() + 
                               "!" + client->getUserName() + 
                               "@localhost 461 " + 
                               targetChannel->getChannelname() + 
                               " MODE :+/- i requires 3 parameters.\r\n";  


        client->sendMessage(threeArgNeeded);
        return ;
    }
    
    targetChannel->setInviteOnly(addingMode);
    targetChannel->broadcastMessage(":" + client->getNickname() + "!" + client->getUserName() +
                                     "@" + "localhost" + " MODE " +
                                     targetChannel->getChannelname() + " " +
                                     (addingMode ? "+i" : "-i") + "\r\n",
                                     NULL);

    targetChannel->broadcastMessage(":" + std::string(SERVER_NAME) + " NOTICE " +
                                     targetChannel->getChannelname() + " :" +
                                     "[Invite only mode " +
                                     (addingMode ? "set" : "removed") +
                                     "] by " + client->getNickname() + "\r\n",
                                     NULL);
}

void    caseTopicRestriction(Channel * targetChannel,Client *client, bool addingMode) {

    targetChannel->setTopicRestricted(addingMode);
    targetChannel->broadcastMessage(":" + client->getNickname() + "!" + client->getUserName() +
                                     "@" + "localhost" + " MODE " +
                                     targetChannel->getChannelname() + " " +
                                     (addingMode ? "+t" : "-t") + "\r\n",
                                     NULL);

    targetChannel->broadcastMessage(":" + std::string(SERVER_NAME) + " NOTICE " +
                                     targetChannel->getChannelname() + " :" +
                                     "[Topic-restricted mode " +
                                     (addingMode ? "enabled" : "disabled") +
                                     "] by " + client->getNickname() + "\r\n",
                                     NULL);           
}

void    caseUserLimitation(Channel * targetChannel,Client *client, std::string command, bool addingMode) {

      if (addingMode) {
            int userLimit = stringToInt(getArgument(command, 4));
            if (getNumOfArgument(command) == 3)
                userLimit = 100;
            targetChannel->setUserLimit(userLimit);
            targetChannel->broadcastMessage(":" + client->getNickname() + "!" + client->getUserName() +
                                     "@" + "localhost" + " MODE " +
                                     targetChannel->getChannelname() + " +l " +
                                     intToString(userLimit) + "\r\n",
                                     NULL);

            targetChannel->broadcastMessage(":" + std::string(SERVER_NAME) + " NOTICE " +
                                     targetChannel->getChannelname() + " :" +
                                     "[User limit set to " + intToString(userLimit) +
                                     "] by " + client->getNickname() + "\r\n",
                                     NULL);
        } else {
            targetChannel->removeUserLimit();
            targetChannel->broadcastMessage("Channel mode -l is set. User limit removed.\n", NULL);
        }

}

int    caseSetPassword(Channel * targetChannel,Client *client, std::string command, bool addingMode) {

    std::string modeParameter = getArgument(command, 4);
    std::string channelName = getArgument(command, 2);
        

    if (addingMode) {
    if (modeParameter.empty()) {
        

             std::string threeArgNeeded = ":" + client->getNickname() + 
                               "!" + client->getUserName() + 
                               "@localhost 461 " + 
                               channelName + 
                               " MODE :+k requires a parameter (password).\r\n";  


        client->sendMessage(threeArgNeeded);
        return 1;
    }

    targetChannel->setPassword(modeParameter);
    targetChannel->broadcastMessage(":" + client->getNickname() + "!" + client->getUserName() +
                                     "@" + "localhost" + " MODE " +
                                     targetChannel->getChannelname() + " +k " + modeParameter + "\r\n",
                                     NULL);

    targetChannel->broadcastMessage(":" + std::string(SERVER_NAME) + " NOTICE " +
                                     targetChannel->getChannelname() + " :" +
                                     "Channel password is set by " + client->getNickname() + "\r\n",
                                     NULL);
    }

    else {
    targetChannel->setPassword(""); 
    targetChannel->broadcastMessage(":" + client->getNickname() + "!" + client->getUserName() +
                                     "@" + "localhost" + " MODE " +
                                     targetChannel->getChannelname() + " -k\r\n",
                                     NULL);

    targetChannel->broadcastMessage(":" + std::string(SERVER_NAME) + " NOTICE " +
                                     targetChannel->getChannelname() + " :" +
                                     "Channel password is removed by " + client->getNickname() + "\r\n",
                                     NULL);
    }

    return (0);
}

int    caseSetAdminPremission(Server* server,Client *client, std::string command, bool addingMode) {

     
    std::string modeParameter = getArgument(command, 4);
    std::string channelName = getArgument(command, 2);

    
    Client* targetClient = server->getClientByNickname(modeParameter);
    if (targetClient == NULL) {
        std::string notfound = ":" + client->getNickname() + 
                               "!" + client->getUserName() + 
                               "@localhost 401 " + 
                               channelName + 
                               "\r\n";        
        client->sendMessage(notfound);
        return (1);

    }
    
    Channel* targetChannel = server->getChannelByName(channelName);
    if (!targetChannel->isMember(targetClient)) {
        std::string error = ":" + client->getNickname() + 
                            "!" + client->getUserName() + 
                            "@localhost 401 " + targetClient->getNickname() + 
                            " :No such nick\r\n";
        
        client->sendMessage(error);
        return (1);
    }

    if (modeParameter.empty()) {
                        client->sendMessage("MODE +o/-o requires a parameter (nickname).\n");
                        return (1);
                    }
                    if (addingMode) {
                        targetChannel->addAdmin(targetClient);
                        targetChannel->broadcastMessage(":" + client->getNickname() + "!" + client->getUserName() +
                                     "@" + "localhost" + " MODE " +
                                     targetChannel->getChannelname() + " +o " +
                                     targetClient->getNickname() + "\r\n",
                                     client);

    targetChannel->broadcastMessage(":" + std::string(SERVER_NAME) + " NOTICE " +
                                     targetChannel->getChannelname() + " :" +
                                     "User " + targetClient->getNickname() +
                                     " is now an operator, assigned by " + client->getNickname() + "\r\n",
                                     NULL);
                    } else {
                        targetChannel->removeAdmin(targetClient);
                        targetChannel->broadcastMessage(":" + client->getNickname() + "!" + client->getUserName() +
                                 "@" + "localhost" + " MODE " +
                                 targetChannel->getChannelname() + " -o " +
                                 targetClient->getNickname() + "\r\n",
                                 client);

    targetChannel->broadcastMessage(":" + std::string(SERVER_NAME) + " NOTICE " +
                                 targetChannel->getChannelname() + " :" +
                                 "User " + targetClient->getNickname() +
                                 " is no longer an operator, removed by " + client->getNickname() + "\r\n",
                                 NULL);
                    }
    return (0);
}

int CommandHandler::modeCommand(const std::string& command, Client* client, Server* server) {

    int numOfArgs = getNumOfArgument(command);
	if (numOfArgs < 3 || numOfArgs > 4)
    {
        client->sendMessage("MODE <channelname> <mode> [parameter]\n");
        return (1);
    }
    std::string channelName = getArgument(command, 2);
    std::string modeChanges = getArgument(command, 3);
    std::string modeParameter = getArgument(command, 4);
    Channel* targetChannel = server->getChannelByName(channelName);

    if (!targetChannel) {
        std::string notfound = ":" + client->getNickname() + 
                               "!" + client->getUserName() + 
                               "@localhost 401 " + 
                               channelName + 
                               "\r\n";       
        client->sendMessage(notfound);
        return (1);
    }

    if (!targetChannel->getIsClientAdminOnServer(client->getFd())) {
        std::string noAdminRight = ":" + client->getNickname() + 
                           "!" + client->getUserName() + 
                           "@localhost 482 " + 
                           channelName + 
                           " :You're not channel operator\r\n";        
    		client->sendMessage(noAdminRight);
        return (1);
    }

    bool addingMode = true; // `+` or `-` mod change
    for (size_t i = 0; i < modeChanges.length(); i++) {
        char mode = modeChanges[i];
        if (mode == '+') {
            addingMode = true;
            continue;
        } else if (mode == '-') {
            addingMode = false;
            continue;
        }
        
        switch (mode) {
            case 'i': // Invitation-mode
                caseInvite(command, targetChannel, client, addingMode);
                break;
            case 't': // Topic-restricted mod
                caseTopicRestriction(targetChannel, client, addingMode);
                break;
            case 'l': // User limit mode
                caseUserLimitation(targetChannel, client, command, addingMode);
                break;
            case 'k': // Channel password
                caseSetPassword(targetChannel, client, command, addingMode);
                break;
            case 'o': // Admin premission add/entfernen
               caseSetAdminPremission(server, client, command, addingMode);
                break;
        }
    }
    return (1);
}
