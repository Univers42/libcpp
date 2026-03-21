# ══════════════════════════════════════════════════════════════════════════════
#  libcpp / libftpp — C++17 utility & systems library
#
#  Builds two identical static archives:
#    libcpp.a   — primary name
#    libftpp.a  — alias required by the libftpp subject
#
#  Usage:
#    $(MAKE) -C ../libcpp
#    $(CXX) ... -L../libcpp -I../libcpp/include -lcpp
# ══════════════════════════════════════════════════════════════════════════════

CXX      = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Iinclude -pthread
AR       = ar rcs

NAME     = libcpp.a
FTNAME   = libftpp.a
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

all: $(NAME) $(FTNAME)

$(NAME): $(OBJ)
	@$(AR) $@ $(OBJ)
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(NAME)$(RESET) $(DIM)archived ($(words $(OBJ)) objects)$(RESET)\n"

$(FTNAME): $(NAME)
	@cp $(NAME) $(FTNAME)
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(FTNAME)$(RESET) $(DIM)(alias of $(NAME))$(RESET)\n"

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME) $(FTNAME)

re: fclean all

# ── Test runner ──────────────────────────────────────────────────────────────

TEST_SRC = $(wildcard tests/*.cpp)
TEST_BIN = test_runner

test: $(NAME)
	@$(CXX) $(CXXFLAGS) $(TEST_SRC) -L. -lcpp -o $(TEST_BIN)
	@./$(TEST_BIN)

.PHONY: all clean fclean re test
