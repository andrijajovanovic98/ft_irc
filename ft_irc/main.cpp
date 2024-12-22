#include "Server.hpp"

int main(int argc, char **argv)
{
    try {
        if (argc != 3)
            throw std::string("Usage: ./ircserv <port> <password>");
        signal(SIGPIPE, SIG_IGN);
        signal(SIGINT, signalHandler);
        Server server(stringToInt(argv[1]), argv[2]);
        global_server = &server;
    }
    catch (const std::string &e) {
        std::cerr << e << std::endl;
    }
    catch (...)
    {
        std::cerr << "Error detected" << std::endl;
    }
}
