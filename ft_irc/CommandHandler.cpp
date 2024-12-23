#include "CommandHandler.hpp"

std::string CommandHandler::errorMessage(const std::string& command) const
{
    if (command == "/quit")
        return std::string("Usage: /quit");
    if (command == "/setnick")
        return std::string("Usage: /setnick");
    if (command == "/list")
        return std::string("Usage: /list");
    if (command == "/listmembers")
        return std::string("Usage: /listmembers <channel>");
    if (command == "/mychannel")
        return std::string("Usage: /mychannel");
    if (command == "/part")
        return std::string("Usage: /part");
    return std::string("Error");
}

bool CommandHandler::isCommandCorrect(const std::string& command, Client* client) {
    std::string theCommand = getArgument(command, 1);
    if (theCommand == "MODE" || theCommand == "/join" || theCommand == "/kick" || theCommand == "TOPIC" || theCommand == "INVITE" || theCommand == "/part" || theCommand == "/nick" || 
     theCommand == "/quit" || theCommand == "USER")
        return (true);
    if (commandBook.find(theCommand) != commandBook.end()) {     
            client->sendMessage(errorMessage(theCommand) + "\n");
            return (false);
    } 
    else {
        client->sendMessage("ERROR: Unknown command: " + theCommand + "\n");
        return (false);
    }
    return (true);
}

void  CommandHandler::processCommand(const std::string& command, Client* client, Server* server)
{
    if (isCommandCorrect(command,client) == false) {
        return ; }

    std::string theCommand = getArgument(command, 1);
    
    if (theCommand == "/quit")
		quitCommand(command, client, server);

    else if (theCommand == "/nick")
        nickCommand(command, client, server);

	else if (theCommand == "/join")
        joinCommand(command, client, server);

	else if (theCommand == "/part")
        partCommand(command, client, server);

    else if (getArgument(command, 1) == "/kick")
        kickCommand(command, client, server);

    else if (getArgument(command, 1) == "/dcc")
        dccCommand(command, client, server);
}

int CommandHandler::dccCommand(const std::string& command, Client* client, Server* server) {
    return Dcc::dccCommand(command, client, server);
}

int CommandHandler::adminCommand(const std::string& command, Client* client, Server* server) {
    
    if (isCommandCorrect(command,client) == false) {
        return (1); }
    
    if (getArgument(command,1) == "INVITE")
        inviteCommand(command,client,server);
            
    else if (getArgument(command,1) == "MODE")
        modeCommand(command,client,server);

    return (0);
}

int CommandHandler::upperCaseCommand(const std::string& command, Client* client, Server* server) {
    std::string trimmedCommand = ft_trim(command);

    if (getArgument(command,1) == "PRIVMSG") 
        privmsgCommand(command, client, server);

    else if (getArgument(command,1) == "TOPIC")
        topicCommand(command, client, server);

    else if (getArgument(command,1) == "USER")
        userCommand(command, client, server);

    return 0; 
}

CommandHandler::CommandHandler()
{
    commandBook["/nick"] = 2;
    commandBook["/mynick"] = 1;
    commandBook["/help"] = 1;
    commandBook["/list"] = 1;
    commandBook["/listmembers"] = 2;
    commandBook["/part"] = 2;
    commandBook["/mychannel"] = 1;
    commandBook["/quit"] = 1;
    commandBook["/tictactoe_start"] = 1;
    commandBook["/tictactoe_move"] = 3;
    commandBook["/tictactoe_board"] = 1;
    commandBook["/tic"] = 1;
    commandBook["/tictactoe"] = 2;
    commandBook["INVITE"] = 3;  
    commandBook["KICK"] = 2;    
    commandBook["JOIN"] = 1;    
    commandBook["TOPIC"] = 3;  
    commandBook["PRIVMSG"] = 2; 
    commandBook["NICK"] = 1;  
    commandBook["USER"] = 4;   
    commandBook["QUIT"] = 1;   
}
