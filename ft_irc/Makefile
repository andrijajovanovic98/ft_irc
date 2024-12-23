CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -Wunused -std=c++98  #-g3 -pedantic
TARGET = ircserv
SRCS = main.cpp Server.cpp CommandHandler.cpp Client.cpp Channel.cpp commands/Join.cpp commands/IrssiCommands.cpp commands/Invite.cpp commands/Mode.cpp \
	 commands/Privmsg.cpp commands/Topic.cpp commands/Nick.cpp commands/Part.cpp commands/Kick.cpp commands/Quit.cpp commands/User.cpp TicTacToeBot.cpp Dcc.cpp

OBJS_DIR = build
OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
