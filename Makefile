# ══════════════════════════════════════════════════════════════════════════════
#  libftpp — C++17 utility & systems library
#
#  Targets:
#    make          → build/lib/libftpp.a          (static)
#    make shared   → build/lib/libftpp.so         (shared)
#    make both     → static + shared
#
#  Build tree:
#    build/obj/    — position-independent object files (usable for both .a/.so)
#    build/dep/    — auto-generated header dependency files (.d)
#    build/lib/    — libftpp.a and/or libftpp.so
#    build/bin/    — demo and test binaries
#
#  Usage (static):
#    $(CXX) ... -Lpath/to/libftpp/build/lib -Ipath/to/libftpp/include -lftpp
#  Usage (shared, runtime path baked in):
#    $(CXX) ... -Lbuild/lib -Wl,-rpath,build/lib -lftpp
# ══════════════════════════════════════════════════════════════════════════════

CXX      = c++
AR       = ar rcs

# ── Language profile ──────────────────────────────────────────────────────────
#  STD selects which half of the library is built.
#
#    make                 STD=c++17 (default) — every module   -> libftpp.a
#    make STD=c++98       the C++98-clean subset               -> libftpp98.a
#    make c98             shorthand for the line above
#
#  Why a source-list split and not #ifdef: 16 of the 40 src/ modules pull in
#  headers using `= delete`, `= default`, default member initializers or
#  <thread>/<chrono>/<atomic>. A C++98 compiler cannot *tokenize* those files,
#  and a preprocessor guard inside a header does not help — the file still has
#  to lex before the branch is taken. So they are excluded from the build, and
#  the umbrella headers exclude them from the include graph (libcpp/config.hpp).
#
#  The two profiles keep separate build trees, so switching STD never reuses
#  objects compiled under the other standard.
STD ?= c++17

# ── Build directories ─────────────────────────────────────────────────────────
OBJ_DIR   = $(BUILD_DIR)/obj
DEP_DIR   = $(BUILD_DIR)/dep
LIB_DIR   = $(BUILD_DIR)/lib
BIN_DIR   = $(BUILD_DIR)/bin

SONAME = $(LIB_DIR)/libftpp.so

# ── Source partition ──────────────────────────────────────────────────────────
#  Membership below was established by compiling every module individually
#  under -std=c++98 -Wall -Wextra -Werror, not by reading the code. Re-run that
#  check before moving a name between the two lists.

#  Compile clean under C++98. (24 modules.)
SRC_C98_CORE = src/bench/benchmark.cpp src/bench/profiler.cpp \
               src/bench/timer.cpp src/data/csv.cpp src/data/database.cpp \
               src/data/date.cpp src/log/logger.cpp src/str/case.cpp \
               src/str/format.cpp src/str/secure.cpp src/str/utf8.cpp \
               src/term/color.cpp src/term/progress.cpp src/term/style.cpp \
               src/term/stylesheet.cpp src/term/table.cpp src/term/tree.cpp \
               src/term/writer.cpp src/test/fuzzer.cpp src/test/snapshot.cpp \
               src/test/spy.cpp src/test/suite.cpp src/util/argparser.cpp \
               src/util/config.cpp

#  The dedicated C++98 tier — generic building blocks (line framing, streaming
#  CSV, epoll registration, buffered sockets). Built only by STD=c++98; the
#  C++17 profile leaves libftpp.a byte-for-byte as it was before STD existed.
SRC_C98_TIER = c98/src/line_buffer.cpp c98/src/csv_writer.cpp \
               c98/src/reactor.cpp c98/src/buffered_socket.cpp

#  Require C++11 or later. (16 modules.) All but one fail on C++11 syntax in
#  their headers; src/mem/leak_guard.cpp is the odd one out — it fails C++98
#  on `operator new` missing its throw(std::bad_alloc) exception specification.
SRC_MODERN   = src/async/persistent_worker.cpp src/async/thread.cpp \
               src/async/worker_pool.cpp src/core/memento.cpp \
               src/data/data_buffer.cpp src/math/ivector2.cpp \
               src/math/ivector3.cpp src/math/perlin_noise_2d.cpp \
               src/math/random_2d_coordinate_generator.cpp \
               src/mem/leak_guard.cpp src/net/client.cpp \
               src/net/message.cpp src/net/server.cpp \
               src/term/thread_safe_iostream.cpp src/util/chronometer.cpp \
               src/util/timer.cpp

ifeq ($(STD),c++98)
  BUILD_DIR = build/c98
  NAME      = $(LIB_DIR)/libftpp98.a
  # No -fPIC/-pthread: this profile is archived into a single-threaded C++98
  # binary. -DLIBCPP_CXX98_PROFILE is what config.hpp keys the restricted
  # build off (a consumer on an old compiler gets it implicitly instead).
  CXXFLAGS  = -std=c++98 -Wall -Wextra -Werror -Iinclude -Ic98/include \
              -DLIBCPP_CXX98_PROFILE
  SRC       = $(SRC_C98_CORE) $(SRC_C98_TIER)
  # Serial on purpose: unbounded -j is what swap-freezes low-headroom
  # machines, and this profile is built from inside another make.
  MAKEFLAGS += --no-print-directory
else
  BUILD_DIR = build
  NAME      = $(LIB_DIR)/libftpp.a
  # -fPIC is always on so the same .o files work for both .a and .so
  CXXFLAGS  = -std=$(STD) -Wall -Wextra -Werror -Iinclude -pthread -fPIC
  SRC       = $(SRC_C98_CORE) $(SRC_MODERN)
  MAKEFLAGS += -j$(shell nproc) --no-print-directory
endif

#  Objects mirror the full source path (src/... and c98/src/...), so the two
#  source roots cannot collide on a basename.
OBJ = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEP = $(patsubst %.cpp,$(DEP_DIR)/%.d,$(SRC))

ifeq ($(STD),c++98)
.NOTPARALLEL:
endif

# Colors
RESET  = \033[0m
BOLD   = \033[1m
DIM    = \033[2m
GREEN  = \033[92m
CYAN   = \033[96m
YELLOW = \033[93m
RED    = \033[91m

# ── Targets ──────────────────────────────────────────────────────────────────

ifeq ($(STD),c++98)
all: $(NAME)
else
all: $(NAME) $(SONAME)
endif

#  Shorthand so consumers can write `make -C vendor/libcpp c98`.
c98:
	@$(MAKE) STD=c++98 all

# Static archive
$(NAME): $(BUILD_STAMP) $(OBJ)
	@$(AR) $@ $(OBJ)
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(notdir $(NAME))$(RESET) $(DIM)archived ($(words $(OBJ)) objects)$(RESET)\n"

# Shared library (-shared + SONAME baked in)
$(SONAME): $(BUILD_STAMP) $(OBJ)
	@$(CXX) -shared -Wl,-soname,$(notdir $(SONAME)) $(OBJ) -o $@
	@printf "  $(GREEN)●$(RESET) $(BOLD)$(notdir $(SONAME))$(RESET) $(DIM)linked ($(words $(OBJ)) objects)$(RESET)\n"

# Pre-create the full obj/dep mirror of src/ before any parallel compile job runs
SRC_DIRS   = $(sort $(dir $(SRC)))
OBJ_SUBDIRS = $(addprefix $(OBJ_DIR)/,$(SRC_DIRS))
DEP_SUBDIRS = $(addprefix $(DEP_DIR)/,$(SRC_DIRS))

# Stamp file — created once, triggers all subdirectory creation atomically
BUILD_STAMP = $(BUILD_DIR)/.dirs

$(BUILD_STAMP):
	@mkdir -p $(LIB_DIR) $(OBJ_SUBDIRS) $(DEP_SUBDIRS)
	@touch $@

# Compile: -MMD -MP generates .d dependency files alongside objects
$(OBJ_DIR)/%.o: %.cpp | $(BUILD_STAMP)
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
	@rm -rf $(BUILD_DIR)
	@printf "  $(DIM)fclean$(RESET) — lib and bin removed\n"

re: fclean
	@$(MAKE) all

# ── Test runner ──────────────────────────────────────────────────────────────

TEST_SRC = $(wildcard tests/*.cpp)
TEST_BIN = $(BIN_DIR)/test_runner

test: $(NAME)
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) $(TEST_SRC) -L$(LIB_DIR) -l:libftpp.a -o $(TEST_BIN)
	@$(TEST_BIN)

# ── Demo ─────────────────────────────────────────────────────────────────────

DEMO_SRC = demo/demo_libftpp.cpp
DEMO_BIN = $(BIN_DIR)/demo_libftpp

demo: $(NAME)
	@mkdir -p $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) -I. $(DEMO_SRC) -L$(LIB_DIR) -l:libftpp.a -o $(DEMO_BIN)
	@$(DEMO_BIN)

# ── compile_studio ───────────────────────────────────────────────────────────
#  Compiles every .cpp in studio/ unitarily:
#    - studio/demo/**/*.cpp → build/bin/studio/demo/**/<name> (each standalone)
#    - studio/tests/*.cpp   → build/bin/studio/tests/test_runner (linked together)

STUDIO_DEMO_SRC = $(shell find studio/demo -name '*.cpp')
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
	@$(CXX) $(CXXFLAGS) $< -L$(LIB_DIR) -l:libftpp.a -o $@

$(STUDIO_TEST_BIN): $(STUDIO_TEST_SRC) $(NAME)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)studio/tests$(RESET) $(CYAN)%-30s$(RESET)\n" "test_runner"
	@$(CXX) $(CXXFLAGS) $(STUDIO_TEST_SRC) -L$(LIB_DIR) -l:libftpp.a -o $@

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

.PHONY: all c98 shared both clean fclean re test demo stats compile_studio run_demos run_tests pyenv format norminette
