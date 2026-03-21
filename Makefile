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
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -Iinclude
AR       = ar rcs

NAME     = libcpp.a
OBJ_DIR  = obj

# All .cpp files under src/
SRC = $(shell find src -name '*.cpp')
OBJ = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

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

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

# ── Test runner ──────────────────────────────────────────────────────────────

TEST_SRC = $(wildcard tests/*.cpp)
TEST_BIN = test_runner

test: $(NAME)
	@$(CXX) $(CXXFLAGS) $(TEST_SRC) -L. -lcpp -o $(TEST_BIN)
	@./$(TEST_BIN)

.PHONY: all clean fclean re test
