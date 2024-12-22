#include "../CommandHandler.hpp"

bool isAlphanumeric(const std::string &str) {

	for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
		if (!std::isalnum(*it))
			return (false);
	return (true);


}

int CommandHandler::nickCommand(const std::string& command, Client* client, Server *server) {

	std::string nickName = getArgument(command, 2);
	std::string oldNick = client->getNickname();
    
	std::string currentChannel = client->getCurrentChannel();
    
	if (nickName == "ajovanov") {
		return (1);
	}
	if (currentChannel == "") {
		if (nickName.length() > 25 || !isAlphanumeric(nickName) || !isalpha(nickName[0])) {
			 client->sendMessage(":" + std::string(SERVER_NAME) + 
                        " 432 " + client->getNickname() +
                        " " +
                        " :Erroneous nickname: must be at most 25 characters and contain only alphabetic letters\r\n");
			return (1);
		
		}

		else if (server->isNickNameTaken(nickName)) {
    		client->sendMessage(":" + std::string(SERVER_NAME) + 
                        " 433 " + client->getNickname() +
                        " " + nickName +
                        "\r\n");
    		return (1);
		}
	}

	else {
		    if (nickName.length() > 25 || !isAlphanumeric(nickName) || !isalpha(nickName[0])) {
			  std::string error = ":" + client->getNickname() + 
                           "!" + client->getUserName() + 
                           "@localhost 432 " + 
                           currentChannel + 
                           " Erroneous nickname: must be at most 25 characters and contain only alphabetic letters\r\n";        
    			client->sendMessage(error);
				return (1);
			}

			else if (server->isNickNameTaken(nickName)) {
				  std::string error = ":" + client->getNickname() + 
                           "!" + client->getUserName() + 
                           "@localhost 433 " + 
                           currentChannel +
                           "\r\n";        
    			client->sendMessage(error);
				return (1);

			}

			/* else {
			Channel *targetChannel = server->getChannelByName(currentChannel);
			targetChannel->broadcastMessage(": NOTICE " +
                                     targetChannel->getChannelname() + " :" +
                                     "User " + oldNick +
                                     " changed his nick to " + nickName + "\r\n",
                                     NULL);
			client->sendMessage(":" + oldNick + "!" + client->getUserName() + 
                    "@" + "localhost" + " NICK :" + nickName + "\r\n");
			} */
	}
	client->sendMessage(":" + oldNick + "!" + client->getUserName() + 
                    "@" + "localhost" + " NICK :" + nickName + "\r\n");
	client->broadcastNickChange(oldNick, nickName, server);
	client->setNickname(nickName);
	
	return (0);
}
