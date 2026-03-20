# ══════════════════════════════════════════════════════════════════════════════
#  libcpp — shared utility library for all CPP Module 04 exercises
#
#  Builds a static archive  libcpp.a  that exercises link against.
#
#  Usage from an exercise Makefile:
#    $(MAKE) -C ../libcpp
#    $(CXX) ... -L../libcpp -I../libcpp ... -lcpp
#  or simply link the .a directly:
#    $(CXX) ... objs... ../libcpp/libcpp.a
# ══════════════════════════════════════════════════════════════════════════════

CXX      = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror
AR       = ar rcs

NAME     = libcpp.a
OBJ_DIR  = obj

# Every .cpp in this directory is part of the library
SRC = Decorator.cpp \
      VerboseDecorator.cpp \
      ColorDecorator.cpp \
      Srgb.cpp \
      TermUtils.cpp \
      TermStyle.cpp \
      TermConf.cpp \
      TermTable.cpp \
      TermWriter.cpp \
      LeakGuard.cpp \
      postman.cpp \
      assertion.cpp \
      log.cpp \
      ft_string.cpp

OBJ = $(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

# Colors
RESET  = \033[0m
BOLD   = \033[1m
DIM    = \033[2m
GREEN  = \033[92m
CYAN   = \033[96m
YELLOW = \033[93m

# ── Targets ──────────────────────────────────────────────────────────────────

all: $(NAME)

$(NAME): $(OBJ)
	@$(AR) $@ $(OBJ)
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(NAME)$(RESET) $(DIM)archived ($(words $(OBJ)) objects)$(RESET)\n"

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) -I. -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
