TARGET = ircserv
NAME = $(TARGET)

CC = c++

OBJ_DIR = obj
SRC_DIR = src
INC_DIR = includes

SRC_FILES = main.cpp setup/InitData.cpp \
			server/IRCServer.cpp \
			server/IRCClient.cpp \
			setup/SignalHandlers.cpp \
			server/IRCChannelManager.cpp \
			server/IRCChannel.cpp \
			server/IRCCommand.cpp \
			server/command_handling/HandleQUIT.cpp \
			server/command_handling/HandleCAP.cpp \
			server/command_handling/HandleJOIN.cpp \
			server/command_handling/HandleKICK.cpp \
			server/command_handling/HandleMODE.cpp \
			server/command_handling/HandleNICK.cpp \
			server/command_handling/HandlePART.cpp \
			server/command_handling/HandlePASS.cpp \
			server/command_handling/HandlePING.cpp \
			server/command_handling/HandlePRIVMSG.cpp \
			server/command_handling/HandleUSER.cpp \
			server/command_handling/HandleWHO.cpp \
			server/command_handling/HandleINVITE.cpp \
			server/command_handling/HandleTOPIC.cpp \
			utils/nullptr.cpp \
			server/IIRCServerOwned.cpp \
			server/BotResponse.cpp \

SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS = -g -Wall -Wextra -Werror -std=c++98 -I $(INC_DIR)

DEPS = $(wildcard $(INC_DIR)/*.h)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking $(NAME)..."
	@$(CC) -o $@ $^
	@echo "--- $(NAME) was built successfully ---"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	@echo "Building $<..."
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(CPPFLAGS)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "Cleaned build cache"

fclean: clean
	@rm -f ircserv
	@echo "Cleaned build"

re: fclean all
	@echo "--- recompile done ---"

.PHONY: all clean fclean re
.NOTPARALLEL: clean fclean all re
