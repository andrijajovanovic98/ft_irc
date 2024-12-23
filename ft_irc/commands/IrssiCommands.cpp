#include "../Server.hpp"

int Server::irssiAutomaticCommands(Client &client, std::string &message, int &client_fd, std::string password) {

    if (message.substr(0, 6) == "CAP LS") {
    
		send(client_fd, "CAP * LS :\r\n", 12, 0);
      	std::string welcomeMessage = "001 " + client.getNickname() + " :Welcome to the IRC server!\r\n";
    	send(client_fd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
    	send(client_fd, "002 :Your host is localhost, running version ft_irc\r\n", 52, 0);
    	send(client_fd, "003 :This server was created today\r\n", 36, 0);

    	std::cout << "User setup complete for client_fd " << client_fd << std::endl;
		return (1);
	}

	if (message.substr(0, 7) == "CAP END") {
    	std::cout << "Capability negotiation ended for client_fd " << client_fd << std::endl;
    	return (1);
	}

	if (message.substr(0, 4) == "PING") {
    	std::string pongMessage = "PONG " + message.substr(5) + "\r\n";
    	send(client_fd, pongMessage.c_str(), pongMessage.size(), 0);
    	return (1);

	}

    if (message.substr(0, 5) == "WHOIS") {
        std::string targetNick = getArgument(message, 2);
        Client* targetClient = getClientByNickname(targetNick);
        if (!targetClient) {
            std::string errorMessage = ":localhost 401 " + client.getNickname() + " " + targetNick + " :No such nick/channel\r\n";
            send(client_fd, errorMessage.c_str(), errorMessage.size(), 0);
            return (1);
        }

        std::string whoisUser = ":localhost 311 " + client.getNickname() + " " + targetClient->getNickname() + " " +
                                targetClient->getUserName() + " " + targetClient->getIpAddress() + " * :" +
                                targetClient->getRealName() + std::string(SERVER_NAME) + "\r\n";
        send(client_fd, whoisUser.c_str(), whoisUser.size(), 0);

        std::string whoisServer = ":localhost 312 " + client.getNickname() + " " + targetClient->getNickname() +
                                  " localhost :localhost\r\n";
        send(client_fd, whoisServer.c_str(), whoisServer.size(), 0);


        std::string endOfWhois = ":localhost 318 " + client.getNickname() + " " + targetClient->getNickname() +
                                 " :End of WHOIS list\r\n";
        send(client_fd, endOfWhois.c_str(), endOfWhois.size(), 0);
 
        return (1);
    }

	if (message.substr(0, 4) == "MODE" && getArgument(message, 2)[0] != '#') {
    	std::string modeMessage = "MODE " + client.getNickname() + " +i\r\n";
    	send(client_fd, modeMessage.c_str(), modeMessage.size(), 0);
    	return (1);
	}

	 if (getArgument(message, 1) == "NICK" && getArgument(message, 2) == "ajovanov") {
       
		return (1);

	}

    if (getArgument(message, 1) == "PASS" && getArgument(message, 2) != password && client.isClientAuthenticated() == false) {
        clientCounterServer('-');

		rejectClient(client_fd, ":server_name 464 client_fd :Invalid password\r\n");
		return (1);

	}
	if (client.isClientAuthenticated() == false) {
		if ((getArgument(message, 3) == "PASS" && getArgument(message, 4) == password) ||
			(getArgument(message, 1) == "PASS" && getArgument(message, 2) == password)) {
    		std::string modeMessage = "PASS " + client.getNickname() + " +i\r\n";
    		send(client_fd, modeMessage.c_str(), modeMessage.size(), 0);
			client.setClientAuthenticated(true);
			std::cout << "Authentication successfull: " << client.getNickname() << std::endl;
    		return (1);
		}
	}
    return (0);
}
