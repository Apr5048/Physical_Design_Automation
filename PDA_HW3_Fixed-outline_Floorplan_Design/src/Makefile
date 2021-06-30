CXX := g++
CXXFLAGS := -std=c++14 -O3 -Wall -Wextra
LIBS := -lm
EXEC := ../bin/FM_Partitioner
SRC_DIRS := Parser\
			Structure\
			Solver
SRCS := main.cpp $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS)

ifeq (test,$(firstword $(MAKECMDGOALS)))
  TEST_CASE_ID := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TEST_CASE_ID):;@:)
endif

test: $(EXEC)
	@echo test on $(TEST_CASE_ID)
	time ./$(EXEC) ../testcase/n$(TEST_CASE_ID).hardblocks ../testcase/n$(TEST_CASE_ID).nets ../testcase/n$(TEST_CASE_ID).pl ../output/n$(TEST_CASE_ID).floorplan 0.1
	../verifier/verify ../testcase/n$(TEST_CASE_ID).hardblocks ../testcase/n$(TEST_CASE_ID).nets ../testcase/n$(TEST_CASE_ID).pl ../output/n$(TEST_CASE_ID).floorplan 0.1

.PHONY: all clean test
-include $(DEPS)
