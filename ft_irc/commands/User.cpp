#include "../CommandHandler.hpp"


int CommandHandler::userCommand(const std::string& command, Client* client, Server* server) {
    (void)server;


	std::string currentChannel = client->getCurrentChannel();
    std::string username = getArgument(command, 2);

    if (getNumOfArgument(command) > 4) {
        return (1);
    }
    if (currentChannel == "") {
		if (username.length() > 25 || !isAlphanumeric(username) || !isalpha(username[0])) {
			 client->sendMessage(":" + std::string(SERVER_NAME) + 
                        " 432 " + client->getNickname() +
                        " " +
                        " :Erroneous username: must be at most 25 characters and contain only alphabetic letters\r\n");
		return (1);
		
	    }

	    else if (server->isUserNameTaken(username)) {
    	    client->sendMessage(":" + std::string(SERVER_NAME) + 
                        " 462 " + client->getNickname() +
                        " " +
                        "Username already in use\r\n");
    	    return (1);
	    }
	}

	else if (currentChannel != "") {
	    if (username.length() > 25 || !isAlphanumeric(username) || !isalpha(username[0])) {

			std::string error = ":" + client->getNickname() + 
                           "!" + client->getUserName() + 
                           "@localhost 432 " + 
                           currentChannel + 
                           " Erroneous username: must be at most 25 characters and contain only alphabetic letters\r\n";        
    		client->sendMessage(error);
			return (1);
	}

	else if (server->isUserNameTaken(username)) {

            client->sendMessage(":" + std::string(SERVER_NAME) + 
                        " 462 " + client->getNickname() +
                        " " +
                        "Username already in use\r\n");
    	    return (1);

	    }
    }
    client->setUsername(username);
    client->sendMessage(":localhost 001 " + client->getNickname() + " :Username sucessfuly changed to: " + username + "\r\n");
    return (0);
}
