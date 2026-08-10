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

SRCS		= $(shell find $(SRCS_DIR) -name '*.cpp')
OBJS		= $(patsubst $(SRCS_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
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
