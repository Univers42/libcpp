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
	@rm -f $(NAME) $(FTNAME) $(TEST_BIN) $(DEMO_BIN)
	@rm -rf build/bin/studio

re: fclean all

# ── Test runner ──────────────────────────────────────────────────────────────

TEST_SRC = $(wildcard tests/*.cpp)
TEST_BIN = test_runner

test: $(NAME)
	@$(CXX) $(CXXFLAGS) $(TEST_SRC) -L. -lcpp -o $(TEST_BIN)
	@./$(TEST_BIN)

# ── Demo ─────────────────────────────────────────────────────────────────────

DEMO_SRC = demo/demo_libftpp.cpp
DEMO_BIN = demo/demo_libftpp

demo: $(NAME)
	@$(CXX) $(CXXFLAGS) -I. $(DEMO_SRC) -L. -lcpp -o $(DEMO_BIN)
	@./$(DEMO_BIN)

# ── compile_studio ───────────────────────────────────────────────────────────
#  Compiles every .cpp in studio/ unitarily:
#    - studio/demo/*.cpp   → build/bin/studio/demo/<name>   (each standalone)
#    - studio/tests/*.cpp  → build/bin/studio/tests/test_runner (linked together)

STUDIO_DEMO_SRC   = $(wildcard studio/demo/*.cpp)
STUDIO_DEMO_BIN   = $(patsubst studio/demo/%.cpp,build/bin/studio/demo/%,$(STUDIO_DEMO_SRC))
STUDIO_TEST_SRC   = $(wildcard studio/tests/*.cpp)
STUDIO_TEST_BIN   = build/bin/studio/tests/test_runner

compile_studio: $(NAME) $(STUDIO_DEMO_BIN) $(STUDIO_TEST_BIN)
	@printf "  $(GREEN)●$(RESET) $(BOLD)compile_studio$(RESET) $(DIM)done$(RESET)\n"

# ── run_demos: compile and execute all studio demos ──────────────────────────

run_demos: compile_studio
	@printf "  $(BOLD)running all demos$(RESET)\n"
	@for bin in $(STUDIO_DEMO_BIN); do \
		printf "  $(DIM)→$(RESET) $(CYAN)%-40s$(RESET) " "$$(basename $$bin)"; \
		$$bin > /dev/null 2>&1 \
			&& printf "$(GREEN)OK$(RESET)\n" \
			|| printf "$(RED)FAIL$(RESET)\n"; \
	done

# ── run_tests: compile and run the studio test suite ─────────────────────────

run_tests: compile_studio
	@$(STUDIO_TEST_BIN)

build/bin/studio/demo/%: studio/demo/%.cpp $(NAME)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)studio/demo$(RESET)  $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $< -L. -lcpp -o $@

$(STUDIO_TEST_BIN): $(STUDIO_TEST_SRC) $(NAME)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)studio/tests$(RESET) $(CYAN)%-30s$(RESET)\n" "test_runner"
	@$(CXX) $(CXXFLAGS) $(STUDIO_TEST_SRC) -L. -lcpp -o $@

# ── Stats ────────────────────────────────────────────────────────────────────

stats:
	@printf "  $(BOLD)lib objects $(RESET) $(YELLOW)$(words $(OBJ))$(RESET)\n"
	@printf "  $(BOLD)headers     $(RESET) $(YELLOW)%s$(RESET)\n" "$$(find include -name '*.hpp' | wc -l)"
	@printf "  $(BOLD)source files$(RESET) $(YELLOW)%s$(RESET)\n" "$$(find src -name '*.cpp' | wc -l)"
	@printf "  $(BOLD)test files  $(RESET) $(YELLOW)%s$(RESET)\n" "$$(find tests -name '*.cpp' | wc -l)"
	@printf "  $(BOLD)total lines $(RESET) $(YELLOW)%s$(RESET)\n" "$$(find include src tests -name '*.hpp' -o -name '*.cpp' | xargs wc -l | tail -1 | awk '{print $$1}')"

# ── Python virtual-environment (no sudo required) ───────────────────────────
#  make pyenv       → create .pyenv/ venv and install pip deps
#  make format      → auto-format all .cpp/.hpp with clang-format (Google style)
#  make norminette  → lint all .cpp/.hpp with vendor/scripts/norminette.sh

VENV       = .pyenv
VENV_PY    = $(VENV)/bin/python3
SCRIPTS    = vendor/scripts

NORM_SRC   = $(shell find src include studio -name '*.cpp' -o -name '*.hpp' 2>/dev/null)

pyenv:
	@bash $(SCRIPTS)/setup_pyenv.sh

$(VENV_PY):
	@bash $(SCRIPTS)/setup_pyenv.sh

format: $(VENV_PY)
	@printf "\n  $(BOLD)$(CYAN) Auto-formatting C++ Source Files$(RESET)\n"
	@find src include studio -name "*.cpp" -o -name "*.hpp" | \
		xargs $(VENV)/bin/clang-format -i -style=file
	@printf "  $(GREEN)●$(RESET) Files formatted successfully using Google style.\n"

norminette: $(VENV_PY)
	@printf "\n  $(BOLD)$(CYAN) Running C++ Code Quality Checks$(RESET)\n"
	@PATH="$(VENV)/bin:$$PATH" $(VENV_PY) $(SCRIPTS)/norminette.sh src include studio \
		-- $(CXXFLAGS) -I.

.PHONY: all clean fclean re test demo stats compile_studio run_demos run_tests pyenv format norminette
