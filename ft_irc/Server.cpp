#include "Server.hpp"

Server* global_server = NULL;

Server::Server(int port, const std::string &password) : password(password){

	static CommandHandler instance;
    commandHandler = &instance; 
    setupServerSocket(port);
    setPort(port);
	std::cout << "The server is already initialized, waiting fo the clients :)" << std::endl;
    userCounter = 0;
    channelCounter = 0;
	setupEpoll();
	run();
	
}

void Server::setupServerSocket(int port)
{
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::string("Socket failed");
	}
	std::cout << "server socket succesfully created" << std::endl;
	int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Failed to set socket options");
	}
	struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr("127.0.0.1");
		address.sin_port = htons(port);
		if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            close(server_fd);
            throw std::string("Binding failed");

        }
	std::cout << "Binding successfull" << std::endl;

	int backlog = 10;
	if (listen(server_fd, backlog) == -1) {
		close(server_fd);
		throw std::string("Listen error");
	}

}


void Server::setupEpoll()
{
	epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        close (server_fd);
		throw std::string("epoll_create1 error");
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        close(server_fd);
        close(epoll_fd);
		throw std::string("epoll_ctl error");
    }
}

void Server::run()
{

    struct epoll_event events[MAX_EVENTS];
	while (true) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
        std::cerr << "epoll_wait error" << std::endl;
        break;
    }

    for (int i = 0; i < nfds; ++i) {
        if (events[i].data.fd == server_fd) {
            handleNewConnection();
        } else {
            handleClientData(events[i].data.fd);
        }
    }
}

}


void Server::handleNewConnection() {

 struct sockaddr_in client_address;
    socklen_t client_addr_len = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addr_len);
    if (client_fd == -1) {
        std::cerr << "Accept error" << std::endl;
        return;
    }

    int nonBlocking = 1;
    if (ioctl(client_fd, FIONBIO, &nonBlocking) == -1) {
        perror("ioctl error");
        close(client_fd);
        return;
    }
    else {
            std::cout << "Socket successfully set to non-blocking mode." << std::endl;
    }

    struct epoll_event client_ev;
    client_ev.events = EPOLLIN;
    client_ev.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) == -1) {
        std::cerr << "epoll_ctl error" << std::endl;
        close(client_fd);
        return;
    }
    if (getNumOfClientsServer() == MAX_CLIENTS) {

        rejectClient(client_fd, ":server_name 421 client_fd :Too many clients on the server\r\n");
        return ;

    }
    clientCounterServer('+');
    std::cout << "Number of online clients now: " << getNumOfClientsServer() << "\n";
    std::cout << "New client connected!" << std::endl;
    _clients.insert(std::make_pair(client_fd, Client(client_fd, "")));


}

void Server::handlingCommands(const std::string& command, Client* client, Server* server) {
        std::string currentChannel = client->getCurrentChannel();
       (void)server;
        if (getArgument(command,1) == "/tictactoe_start" || command.substr(0, 4) == "/tic" || command.substr(0, 3) == "TIC") {
            ticTacToeBot.processCommand(command, client);
            return;
        }
        
        if (!command.empty() && command[0] == '/') {
            commandHandler->processCommand(command, client, this);
            return ;
        }
        else if (getArgument(command, 1) == "PRIVMSG" || getArgument(command, 1) == "TOPIC" || getArgument(command, 1) == "NICK" || getArgument(command, 1) == "USER" ) {
            
            commandHandler->upperCaseCommand(command, client, this);
            return ;
        }
        else if (currentChannel.empty()) {
            return ;
        }
        else if (isAdminCommand(getArgument(command, 1))) {
                commandHandler->adminCommand(command, client, this);
                return ;
        }
        else if (!currentChannel.empty()) {
                    _channels[currentChannel].broadcastMessage(client->getNickname() + ": " + command, client);
        }

}

bool Server::isValidMessage(const std::string& message) {
    for (size_t i = 0; i < message.size(); ++i) {
        char c = message[i];
        if ((int)c == -1) {
            std::cerr << "Ctrl+C detected in message!" << std::endl;
            return false; 
        }
}
    return true;
}

void Server::handleClientData(int client_fd)
{
    char buffer[4096] = {0};
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            std::cout << "The client closed the  connection." << std::endl;

        } else {
            std::cerr << "Reading error happend on the client soccet" << std::endl;
        }
        std::map<int, Client>::iterator it = _clients.find(client_fd);
        Client& client = it->second;
        removeClient(client_fd);
        clientCounterServer('-');
        close(client_fd);  

        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);

        for (size_t i = 0; i < client.getOnChannels().size(); i++) {
            const std::string& channelName = client.getOnChannels()[i];

            if (!channelName.empty()) {
                Channel* targetChannel = getChannelByName(channelName);
        
                if (targetChannel != NULL) {
                    targetChannel->clientCounter('-');  
                    leaveChannel(channelName, &client);
                    
                }
            }
        }
    
    }
    else {
    buffer[bytes_read] = '\0';
    const size_t MAX_MESSAGE_SIZE = 512;
        size_t total_bytes_read = 0;
        if ((size_t)bytes_read > MAX_MESSAGE_SIZE) {
            std::string errorMsg = "ERROR :Message too long. Maximum allowed size is 512 bytes.\r\n";
            send(client_fd, errorMsg.c_str(), errorMsg.size(), 0);

            int read_count = 0; 
            total_bytes_read = bytes_read;
            while (true) {
                int additional_bytes = read(client_fd, buffer, sizeof(buffer) - 1);
                if (additional_bytes <= 0) {
                    break;
                }
                total_bytes_read += additional_bytes;
                read_count++;
                if (read_count > 5 || total_bytes_read > MAX_MESSAGE_SIZE) {
                    std::cerr << "Too much data from client_fd: " << client_fd << std::endl;
                    close(client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    return;
                }
            }
            return;
        }
    std::map<int, Client>::iterator it = _clients.find(client_fd);
    Client& client = it->second;
    client.appendToBuffer(buffer);
    std::cout << "Message from the client: " << buffer << std::endl;

    while (_clients.find(client_fd) != _clients.end() && client.hasCompleteCommand()) {
        std::string message = client.extractCommand();
        
        if (message.empty()) {
            std::cerr << "Warning: Extracted empty message!" << std::endl;
            continue;
        }

        if (irssiAutomaticCommands(client, message, client_fd, getServerPassword()) == 1) {
            std::cout << "Ignored irssi automatic command." << std::endl;
            continue;
        }

        if (!client.isClientAuthenticated()) {
            client.sendMessage("Please enter the password. PASS <password>\r\n");
            continue;
        }

        if (getArgument(message, 1) == "NICK" || 
            getArgument(message, 1) == "JOIN" || 
            getArgument(message, 1) == "PART" || 
            getArgument(message, 1) == "QUIT" || 
            getArgument(message, 1) == "KICK") {
            message = commandTranslate(message);
        }

        handlingCommands(message, &client, this);
    }

    if (_clients.find(client_fd) != _clients.end()) {
    Client& client = _clients.at(client_fd);
        if (client.getBuffer().empty()) {
            client.clearBuffer();
        }
    }
    }
}

Server::~Server() {

	std::cout << "The server is closed" << std::endl;
    _clients.clear();
	close(server_fd);
}


void Server::disconnectClient(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clients.erase(fd);
	clientCounterServer('-');



}


void Server::listChannels(Client* client) {
    std::string channelList = "Channels available:\n";
    
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        channelList += "- " + it->first + "\n";
    }

    if (_channels.empty()) {
        channelList += "No available channels.\n";
    }

    client->sendMessage(channelList);
}

void Server::leaveChannel(const std::string& channelName, Client *client) {
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		
        if (it != _channels.end()) {

		    if (client != NULL)
                it->second.removeClient(client);
			if (it->second.isEmpty()) {
				_channels.erase(it);
	            channelCounterf('-');

			}
		if (client != NULL)
            client->setCurrentChannel("");

        return ;
		}
        
}

void Server::listChannelMembers(const std::string& channelName, Client* requester) {
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it != _channels.end()) {
        std::string memberList = it->second.getClientList();
        requester->sendMessage("Members in the channel: " + memberList + "\n");
    } else {
        requester->sendMessage("No channel by that name.\n");
    }
}

void Server::kickCommand(Client* client) {

    std::map<std::string, Channel>::iterator it = _channels.find(client->getCurrentChannel());
            if (it != _channels.end()) {
    	it->second.removeClient(client);
    	if (it->second.isEmpty()) {
    		_channels.erase(it);
    	}
		client->setCurrentChannel("");

        }
}

std::string ft_trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}


std::string getArgument(const std::string& command, int arg) {
    std::istringstream iss(command);
    std::string word;
    int count = 0;
    
    while (iss >> word) {
        count++;
        if (count == arg) { 
            return ft_trim(word);
        }
    }
    return ""; 
}


int getNumOfArgument(const std::string& command) {
    std::istringstream iss(command);
    std::string word;
    int count = 0;
    
    while (iss >> word) {
        count++;
    }
    return count;

}

Channel* Server::getChannelByName(const std::string& name) {
    std::map<std::string, Channel>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        return &(it->second);
    }
    return NULL;
}

Client* Server::getClientByNickname(const std::string& nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == nickname) {
            return &it->second;
        }
    }
    return NULL;
}

bool Server::isAdminCommand(const std::string& command) {
    std::string adminCommands[] = {"KICK", "INVITE", "TOPIC", "MODE"};
    std::string arg1 = ft_trim(getArgument(command, 1));
    for (size_t i = 0; i < sizeof(adminCommands) / sizeof(adminCommands[0]); ++i) {
        if (arg1 == adminCommands[i]) {
            return true;
        }
    }
    return false;
}


int Server::getPort() const {
    return (this->port);
}

void Server::setPort(int port) {
    if (port >= 6665 && port <= 6669) {
        this->port = port;
        return;
    }
    throw std::string("Available ports are: 6665 - 6669\n");
}


std::string Server::getServerPassword() const {
    return (this->password);
}


std::string Server::commandTranslate(const std::string& command) {
    static const std::map<std::string, std::string> commandMap =
        std::map<std::string, std::string>();

    if (commandMap.empty()) {
        const_cast<std::map<std::string, std::string>&>(commandMap).insert(std::make_pair("NICK", "/nick"));
        const_cast<std::map<std::string, std::string>&>(commandMap).insert(std::make_pair("JOIN", "/join"));
        const_cast<std::map<std::string, std::string>&>(commandMap).insert(std::make_pair("PART", "/part"));
        const_cast<std::map<std::string, std::string>&>(commandMap).insert(std::make_pair("QUIT", "/quit"));
        const_cast<std::map<std::string, std::string>&>(commandMap).insert(std::make_pair("KICK", "/kick"));
    }

    if (command.empty()) {
    return ""; }
    

    std::string cmd = getArgument(command, 1);

    if (cmd == "QUIT") {
        return "/quit " + getArgument(command, 2); }
    

    if (getNumOfArgument(command) < 2) {
        return ""; }
    
    if (cmd == "KICK") {
        std::string result = "/kick " + getArgument(command, 2) + " " + getArgument(command, 3);
        if (getNumOfArgument(command) >= 4) {
            result += " " + getArgument(command, 4);
        }
        return result;
    }

    std::map<std::string, std::string>::const_iterator it = commandMap.find(cmd);
    if (it != commandMap.end()) {
        return it->second + " " + getArgument(command, 2) + " " + getArgument(command, 3) ;
    }

    return command; 
    }


void Server::broadcastToChannel(const std::string& channelName, const std::string& message, Client* sender) {
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it == _channels.end()) {
        return;
    }

    it->second.broadcastMessage(message, sender);
}


bool Server::isNickNameTaken(std::string nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == nickname) {
            return true; 
        }
    }
    return false; 
}

bool Server::isUserNameTaken(std::string nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getUserName() == nickname) {
            return true; 
        }
    }
    return false; 
}

void Server::clientCounterServer(char sign)
{
	if (sign == '-')
		userCounter--;
	else
		userCounter++;
}


int Server::getNumOfClientsServer() {
    return (this->userCounter);
}

void Server::channelCounterf(char sign)
{
	if (sign == '-') {
		channelCounter--;
    }
    else
		channelCounter++;
}


int Server::getNumOfChannels() {
    return (this->channelCounter);
}

void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "SIGINT received. Cleaning up and exiting..." << std::endl;
        if (global_server) {
            global_server->shutdownServer();
        }
    }
}

void Server::shutdownServer() {
    std::cout << "Shutting down server..." << std::endl;

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        disconnectClient(it->first);
    }
    _clients.clear();

    if (epoll_fd != -1) {
        close(epoll_fd);
        epoll_fd = -1;
    }


    std::cout << "Server shut down successfully." << std::endl;
}

void Server::rejectClient(int client_fd, std::string message) {
    if (send(client_fd, message.c_str(), message.size(), 0) == -1) {
        std::cerr << "Failed to send error message to client." << std::endl;
    }
    sleep(1);
    shutdown(client_fd, SHUT_WR);

    close(client_fd);

    std::cout << "Rejected client " << client_fd << " due to server overload." << std::endl;
}

void Server::removeClient(int client_fd) {
    std::map<int, Client>::iterator it = _clients.find(client_fd);
    if (it != _clients.end()) {
        std::cout << "Removing client with fd: " << client_fd << " (nickname: " << it->second.getNickname() << ")" << std::endl;

        leaveAllChannels(&(it->second));

        close(client_fd);
    } else {
        std::cerr << "Client with fd: " << client_fd << " not found in the client list." << std::endl;
    }
}

void Server::leaveAllChannels(Client* client) {
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        it->second.removeClient(client);
    }
    std::cout << "Client removed from all channels." << std::endl;
}

