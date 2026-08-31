NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

# (génération de dépendances) : si un .hpp est modifié,
# seuls les .cpp qui l'incluent sont recompilés
# ça évite le "relinking inutile" 
DEPFLAGS	= -MMD -MP

INCLUDES	= -Iincludes

SRCS_DIR	= srcs
OBJ_DIR		= obj

SRCS_FILES	= main.cpp \
              Client.cpp \
			  Server.cpp \
			  Channel.cpp \
			  Parser.cpp \
			  commands/Invite.cpp \
			  commands/Join.cpp \
			  commands/Kick.cpp \
			  commands/Mode.cpp \
			  commands/Nick.cpp \
			  commands/Part.cpp \
			  commands/Pass.cpp \
			  commands/PrivMsg.cpp \
			  commands/Topic.cpp \
			  commands/Quit.cpp \
			  commands/User.cpp

OBJ_FILES	= $(SRCS_FILES:.cpp=.o)
			  
SRCS		= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))
OBJS		= $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))
DEPS		= $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
