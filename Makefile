# ══════════════════════════════════════════════════════════════════════════════
#  libcpp / libftpp — C++17 utility & systems library
#
#  Targets:
#    make          → build/lib/libcpp.a  + build/lib/libftpp.a  (static)
#    make shared   → build/lib/libcpp.so + build/lib/libftpp.so (shared)
#    make both     → static + shared
#
#  Build tree:
#    build/obj/    — position-independent object files (usable for both .a/.so)
#    build/dep/    — auto-generated header dependency files (.d)
#    build/lib/    — static (.a) and/or shared (.so) libraries
#    build/bin/    — demo and test binaries
#
#  Usage (static):
#    $(CXX) ... -Lpath/to/libcpp/build/lib -Ipath/to/libcpp/include -lcpp
#  Usage (shared, runtime path baked in):
#    $(CXX) ... -Lbuild/lib -Wl,-rpath,build/lib -lcpp
# ══════════════════════════════════════════════════════════════════════════════

CXX      = c++
# -fPIC is always on so the same .o files work for both .a and .so
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Iinclude -pthread -fPIC
AR       = ar rcs

# ── Build directories ─────────────────────────────────────────────────────────
BUILD_DIR = build
OBJ_DIR   = $(BUILD_DIR)/obj
DEP_DIR   = $(BUILD_DIR)/dep
LIB_DIR   = $(BUILD_DIR)/lib
BIN_DIR   = $(BUILD_DIR)/bin

# Static libraries
NAME   = $(LIB_DIR)/libcpp.a
FTNAME = $(LIB_DIR)/libftpp.a

# Shared libraries
SONAME   = $(LIB_DIR)/libcpp.so
FTSONAME = $(LIB_DIR)/libftpp.so

# Parallel jobs — use all available cores
MAKEFLAGS += -j$(shell nproc) --no-print-directory

# All .cpp files under src/
SRC = $(shell find src -name '*.cpp')
OBJ = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEP = $(patsubst src/%.cpp,$(DEP_DIR)/%.d,$(SRC))

# Colors
RESET  = \033[0m
BOLD   = \033[1m
DIM    = \033[2m
GREEN  = \033[92m
CYAN   = \033[96m
YELLOW = \033[93m
RED    = \033[91m

# ── Targets ──────────────────────────────────────────────────────────────────

all: $(NAME) $(FTNAME)

shared: $(SONAME) $(FTSONAME)

both: all shared

# Static archives
$(NAME): $(BUILD_STAMP) $(OBJ)
	@$(AR) $@ $(OBJ)
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(notdir $(NAME))$(RESET) $(DIM)archived ($(words $(OBJ)) objects)$(RESET)\n"

$(FTNAME): $(NAME)
	@cp $(NAME) $(FTNAME)
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(notdir $(FTNAME))$(RESET) $(DIM)(alias of $(notdir $(NAME)))$(RESET)\n"

# Shared libraries  (-shared + SONAME baked in)
$(SONAME): $(BUILD_STAMP) $(OBJ)
	@$(CXX) -shared -Wl,-soname,$(notdir $(SONAME)) $(OBJ) -o $@
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(notdir $(SONAME))$(RESET) $(DIM)linked ($(words $(OBJ)) objects)$(RESET)\n"

$(FTSONAME): $(SONAME)
	@cp $(SONAME) $(FTSONAME)
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(notdir $(FTSONAME))$(RESET) $(DIM)(alias of $(notdir $(SONAME)))$(RESET)\n"

# Pre-create the full obj/dep mirror of src/ before any parallel compile job runs
SRC_DIRS   = $(sort $(dir $(SRC)))
OBJ_SUBDIRS = $(patsubst src/%,$(OBJ_DIR)/%,$(SRC_DIRS))
DEP_SUBDIRS = $(patsubst src/%,$(DEP_DIR)/%,$(SRC_DIRS))

# Stamp file — created once, triggers all subdirectory creation atomically
BUILD_STAMP = $(BUILD_DIR)/.dirs

$(BUILD_STAMP):
	@mkdir -p $(LIB_DIR) $(OBJ_SUBDIRS) $(DEP_SUBDIRS)
	@touch $@

# Compile: -MMD -MP generates .d dependency files alongside objects
$(OBJ_DIR)/%.o: src/%.cpp | $(BUILD_STAMP)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) -MMD -MP -MF $(patsubst $(OBJ_DIR)/%.o,$(DEP_DIR)/%.d,$@) \
		-c $< -o $@

$(OBJ_DIR) $(DEP_DIR) $(LIB_DIR):
	@mkdir -p $@

# Include generated dependency files (silently skip if missing)
-include $(DEP)

clean:
	@rm -rf $(OBJ_DIR) $(DEP_DIR) $(BUILD_STAMP)
	@printf "  $(DIM)clean$(RESET) — obj and dep removed\n"

fclean: clean
	@rm -rf $(LIB_DIR) $(BIN_DIR)
	@printf "  $(DIM)fclean$(RESET) — lib and bin removed\n"

re: fclean
	@$(MAKE) all

# ── Test runner ──────────────────────────────────────────────────────────────

TEST_SRC = $(wildcard tests/*.cpp)
TEST_BIN = $(BIN_DIR)/test_runner

test: $(NAME)
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) $(TEST_SRC) -L$(LIB_DIR) -l:libcpp.a -o $(TEST_BIN)
	@$(TEST_BIN)

# ── Demo ─────────────────────────────────────────────────────────────────────

DEMO_SRC = demo/demo_libftpp.cpp
DEMO_BIN = $(BIN_DIR)/demo_libftpp

demo: $(NAME)
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) -I. $(DEMO_SRC) -L$(LIB_DIR) -l:libcpp.a -o $(DEMO_BIN)
	@$(DEMO_BIN)

# ── compile_studio ───────────────────────────────────────────────────────────
#  Compiles every .cpp in studio/ unitarily:
#    - studio/demo/*.cpp   → build/bin/studio/demo/<name>   (each standalone)
#    - studio/tests/*.cpp  → build/bin/studio/tests/test_runner (linked together)

STUDIO_DEMO_SRC = $(wildcard studio/demo/*.cpp)
STUDIO_DEMO_BIN = $(patsubst studio/demo/%.cpp,$(BIN_DIR)/studio/demo/%,$(STUDIO_DEMO_SRC))
STUDIO_TEST_SRC = $(wildcard studio/tests/*.cpp)
STUDIO_TEST_BIN = $(BIN_DIR)/studio/tests/test_runner

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

$(BIN_DIR)/studio/demo/%: studio/demo/%.cpp $(NAME)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)studio/demo$(RESET)  $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $< -L$(LIB_DIR) -l:libcpp.a -o $@

$(STUDIO_TEST_BIN): $(STUDIO_TEST_SRC) $(NAME)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)studio/tests$(RESET) $(CYAN)%-30s$(RESET)\n" "test_runner"
	@$(CXX) $(CXXFLAGS) $(STUDIO_TEST_SRC) -L$(LIB_DIR) -l:libcpp.a -o $@

# ── Stats ────────────────────────────────────────────────────────────────────

stats:
	@printf "  $(BOLD)lib objects $(RESET) $(YELLOW)$(words $(OBJ))$(RESET)\n"
	@printf "  $(BOLD)headers     $(RESET) $(YELLOW)%s$(RESET)\n" "$$(find include -name '*.hpp' | wc -l)"
	@printf "  $(BOLD)source files$(RESET) $(YELLOW)%s$(RESET)\n" "$$(find src -name '*.cpp' | wc -l)"
	@printf "  $(BOLD)total lines $(RESET) $(YELLOW)%s$(RESET)\n" "$$(find include src -name '*.hpp' -o -name '*.cpp' | xargs wc -l | tail -1 | awk '{print $$1}')"

# ── Python virtual-environment (no sudo required) ───────────────────────────
#  make pyenv       → create .pyenv/ venv and install pip deps
#  make format      → auto-format all .cpp/.hpp with clang-format (Google style)
#  make norminette  → lint all .cpp/.hpp with vendor/scripts/norminette.sh

VENV    = .pyenv
VENV_PY = $(VENV)/bin/python3
SCRIPTS = vendor/scripts

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

.PHONY: all shared both clean fclean re test demo stats compile_studio run_demos run_tests pyenv format norminette
