CXX := c++
CXXFLAGS := -std=c++98 -Wall -Wextra -Werror -I.

TESTDIR := tests
BINDIR  := bin
OBJDIR  := obj

SOURCES_TEST := $(wildcard $(TESTDIR)/*.cpp)
TESTS := $(patsubst $(TESTDIR)/%.cpp,%,$(SOURCES_TEST))

SRCS_ROOT := $(wildcard *.cpp)
ROOT_OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS_ROOT))
TEST_OBJS := $(patsubst $(TESTDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES_TEST))

.PHONY: all test clean fclean re

all: test

# Build all test executables into bin/
test: $(BINDIR) $(OBJDIR) $(addprefix $(BINDIR)/,$(TESTS))

# Ensure bin/ and obj/ exist
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Compile test sources to obj/<name>.o
$(OBJDIR)/%.o: $(TESTDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile root sources to obj/<name>.o
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link rule: produce bin/<name> from obj/<name>.o plus root objects
$(BINDIR)/%: $(OBJDIR)/%.o $(ROOT_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Special rule for repl: link with readline
$(BINDIR)/repl: $(OBJDIR)/repl.o $(ROOT_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -lreadline -o $@

# Remove object files
clean:
	rm -rf $(OBJDIR)

# Remove objects and binaries
fclean: clean
	rm -rf $(BINDIR)

# Full rebuild
re: fclean test
