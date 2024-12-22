#include "TicTacToeBot.hpp"
#include "Client.hpp"
#include <iostream>
#include <sstream>
#include <string>

TicTacToeBot::TicTacToeBot() : gameActive(false), botClient(NULL) {
    board = std::vector< std::vector<char> >(3, std::vector<char>(3, ' '));
}

TicTacToeBot::~TicTacToeBot() {
    delete botClient;
}

void TicTacToeBot::processCommand(const std::string& command, Client* client) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if ((cmd == "/tictactoe_start" || cmd == "/tic" || cmd == "TIC") && !gameActive) {
        startGame(client);
    } else if (cmd == "/tictactoe_move" || cmd == "/tic_move" || cmd == "/tic_mv" || cmd == "TIC_MOVE") {
        int row, col;
        iss >> row >> col;
        makeMove(client, row, col);
    } else if (cmd == "/tictactoe_board" || cmd == "/tic_board" || cmd == "/tic_br" || cmd == "TIC_BOARD") {
        displayBoard(client);
    }
}

void TicTacToeBot::displayBoard(const Client* client) {
    std::ostringstream oss;
    oss << "=====\n";
    for (std::vector< std::vector<char> >::iterator rowIt = board.begin(); rowIt != board.end(); ++rowIt) {
        for (std::vector<char>::iterator cellIt = rowIt->begin(); cellIt != rowIt->end(); ++cellIt) {
            oss << (*cellIt == ' ' ? '.' : *cellIt) << ' ';
        }
        oss << '\n';
    }
    client->sendMessage(oss.str());
}

void TicTacToeBot::startGame(Client* client) {
    board = std::vector< std::vector<char> >(3, std::vector<char>(3, ' '));
    playerX = client->getNickname();
    playerO = "Bot";
    currentPlayer = playerX;
    gameActive = true;
    botClient = new Client(-1, "Bot");
    if (botClient != NULL) {
        delete botClient;
    }
    std::cout << "Game started! " << playerX << " vs " << playerO << "\n";
    displayBoard(client);
}

void TicTacToeBot::makeMove(const Client* client, int row, int col) {
    if (!gameActive) {
        client->sendMessage("No active game. Start a new game with /tictactoe start\n");
        return;
    }

    if (client->getNickname() != currentPlayer) {
        client->sendMessage("It's not your turn!\n");
        return;
    }

    if (row < 0 || row >= 3 || col < 0 || col >= 3 || board[row][col] != ' ') {
        client->sendMessage("Invalid move. Try again.\n");
        return;
    }

    board[row][col] = (currentPlayer == playerX) ? 'X' : 'O';
    displayBoard(client);

    if (checkWin()) {
        client->sendMessage(currentPlayer + " wins!\n");
        gameActive = false;
    } else if (checkDraw()) {
        client->sendMessage("It's a draw!\n");
        gameActive = false;
    } else {
        switchPlayer();
        if (currentPlayer == playerO) {
            makeBotMove(client);
        }
    }
}

void TicTacToeBot::makeBotMove(const Client* client) {
    if (!gameActive) {
        return;
    }

    // Simple AI: Find the first empty spot and place the bot's mark there
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (board[row][col] == ' ') {
                std::ostringstream oss;
                oss << "Bot making move at (" << row << ", " << col << ")\n";
                client->sendMessage(oss.str());
                makeMove(botClient, row, col);
                displayBoard(client);
                return;
            }
        }
    }
}

bool TicTacToeBot::checkWin() {
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return true;
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return true;
    }
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) return true;
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) return true;
    return false;
}

bool TicTacToeBot::checkDraw() {
    for (std::vector< std::vector<char> >::const_iterator rowIt = board.begin(); rowIt != board.end(); ++rowIt) {
        for (std::vector<char>::const_iterator cellIt = rowIt->begin(); cellIt != rowIt->end(); ++cellIt) {
            if (*cellIt == ' ') return false;
        }
    }
    return true;
}

void TicTacToeBot::switchPlayer() {
    currentPlayer = (currentPlayer == playerX) ? playerO : playerX;
    std::cout << "It's " << currentPlayer << "'s turn.\n";
}
