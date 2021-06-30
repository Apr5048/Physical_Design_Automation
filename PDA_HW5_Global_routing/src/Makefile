CXX := g++
CXXFLAGS := -std=c++14 -O3 -Wall -Wextra
LIBS := -lm
EXEC := ../bin/FM_Partitioner
SRC_DIRS := Parser\
			Structure\
			Router
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
	./$(EXEC) ../testcase/ibm$(TEST_CASE_ID).modified.txt ../output/ibm$(TEST_CASE_ID).result
	../verifier/verify ../testcase/ibm$(TEST_CASE_ID).modified.txt ../output/ibm$(TEST_CASE_ID).result

.PHONY: all clean test
-include $(DEPS)