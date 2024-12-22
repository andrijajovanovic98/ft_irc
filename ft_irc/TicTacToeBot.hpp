#pragma once
#include <string>
#include <vector>

class Client;

class TicTacToeBot {
public:

    TicTacToeBot();
    ~TicTacToeBot();
    void processCommand(const std::string& command, Client* client);
    void displayBoard(const Client* client);
    void startGame(Client* client);
    void makeMove(const Client* client, int row, int col);
    void makeBotMove(const Client* client);

private:

    std::vector< std::vector<char> > board;
    std::string currentPlayer;
    std::string playerX;
    std::string playerO;
    bool gameActive;
    Client* botClient;
    bool checkWin();
    bool checkDraw();
    void switchPlayer();
};
