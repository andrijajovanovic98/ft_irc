#include "Client.hpp"

Client::Client(int fd, const std::string& nickname = "") : client_fd(fd) , authenticated(false) {
    if (nickname.empty()) {
        this->nickname = generateRandomNickname();
    } else {
        this->nickname = nickname;
    }
	isAdmin = false;
	setUsername(generateUsername(fd));
	kick = 0;
	ipAddress = "127.0.0.1";
    dccPort = 12345;
	_onChannels.resize(10);
	_onChannels[0] = "";
    currentChannel = "";
}

std::string Client::getIpAddress() const {
    return ipAddress;
}

int Client::getDccPort() const {
    return dccPort;
}


std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

int stringToInt(const std::string& str) {
    std::stringstream ss(str);
    int num;
    ss >> num;
    return num;
}


std::string Client::generateUsername(int fd) {
    return "user" + intToString(fd);
}

//	~Client() { close(client_fd); }

int Client::getFd() const {
	return (this->client_fd);
}
std::string Client::getNickname() const {
	return (this->nickname);
}
void Client::setNickname(const std::string &nickname) {
	this->nickname = nickname;
}
void Client::setUsername(const std::string &userName) {
    this->userName = userName;
}

bool Client::sendMessage(const std::string&  message) const {
	
	int bytesSent = send(client_fd, message.c_str(), message.size(), 0);
	if (bytesSent == -1)
		return false;
	return true;
}

void Client::appendToMessageBuffer(const std::string &data) {
	messageBuffer.insert(messageBuffer.end(), data.begin(), data.end());
}

void Client::clearMessageBuffer() {
	messageBuffer.clear();
}

std::string Client::getMessageBuffer() const {
	return std::string(messageBuffer.begin(), messageBuffer.end());
}

void Client::setCurrentChannel(const std::string &channel) { 
    if (channel != "") {
        currentChannel = channel;
    } else {
        std::vector<std::string>::iterator it = std::find(_onChannels.begin(), _onChannels.end(), currentChannel);

        if (it != _onChannels.end() && it != _onChannels.begin()) {
            --it; 
            currentChannel = *it; 
        } else {
            currentChannel = "";
        }
    }
}

std::string Client::getCurrentChannel() const {
    return currentChannel; 
}

std::string Client::generateRandomNickname() {
    int randomNum = std::rand() % 10000;
    std::ostringstream oss;
    oss << "Guest" << randomNum;
    return oss.str();
}

void Client::setAdmin() {
	if (isAdmin == false) {
		isAdmin = true;
		send(this->getFd(), BEADMIN, strlen(BEADMIN), 0); 
	}
	else
		send(this->getFd(), "You are already admin...\n", strlen("You are already admin...\n"), 0); 
}

bool Client::getIsAdmin() {

	return (isAdmin);
}


bool Client::isClientAuthenticated() const {
	return (this->authenticated);
}

void Client::setClientAuthenticated(bool value) {
	this->authenticated = value;
}


std::string Client::getUserName() const  {
	return (this->userName);
}

std::string Client::getRealName() const {
    return realName;
}

void Client::setRealName(const std::string &realName) {
    this->realName = realName;
}

void Client::SetOnChannels(const std::string& channelName, int position) {
    if (position > 0 && position < 10) {
        _onChannels[position] = channelName;
    }
}

void Client::PrintChannels() {
    for (size_t i = 0; i < _onChannels.size(); i++) {
        std::cout << "Channel[" << i << "]: " 
                  << (_onChannels[i].empty() ? "lobby" : _onChannels[i]) << std::endl;
    }
}

void Client::AddChannelToList(const std::string& channelName) {
    _onChannels.push_back(channelName);
}


void Client::broadcastNickChange(const std::string& oldNick, const std::string& newNick, Server* server) {
    for (size_t i = 0; i < _onChannels.size(); i++) {
        if (!_onChannels[i].empty()) {
            Channel* targetChannel = server->getChannelByName(_onChannels[i]);
            if (targetChannel) {
                targetChannel->broadcastMessage(": NOTICE " +
                                     targetChannel->getChannelname() + " :" +
                                     "User " + oldNick +
                                     " changed his nick to " + newNick + "\r\n",
                                     NULL);
        
            }
        }
    }

}

const std::vector<std::string>& Client::getOnChannels() const {
    return _onChannels;
}

void Client::RemoveGetOnChannels(const std::string &channelName) {
    std::vector<std::string>::iterator it = std::find(_onChannels.begin(), _onChannels.end(), channelName);

    if (it != _onChannels.end()) {
        _onChannels.erase(it);
    }
}

bool Client::FindGetOnChannels(const std::string &channelName) {
    std::vector<std::string>::iterator it = std::find(_onChannels.begin(), _onChannels.end(), channelName);

    if (it != _onChannels.end()) {
        return (true);
    }
    return (false);
}

void Client::appendToBuffer(const std::string& data) {
    buffer += data;
}

std::string Client::getBuffer() const {
    return buffer;
}

void Client::eraseFromBuffer(size_t startPos, size_t length) {
    buffer.erase(startPos, length);
}

bool Client::hasCompleteCommand() const {
    return buffer.find("\r\n") != std::string::npos;
}

std::string Client::extractCommand() {
    size_t pos = buffer.find("\r");
    if (pos != std::string::npos) {
        std::string command = buffer.substr(0, pos);
        buffer.erase(0, pos + 2);
        return command;
    }
    return ""; 
}

void Client::clearBuffer() {
    if (!buffer.empty()) {
        buffer.clear();
    }
}
