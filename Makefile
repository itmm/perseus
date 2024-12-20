.PHONY: all clean deps

SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
TEST_APP := t_perseus

CXXFLAGS = -Wall -std=c++20

all: $(TEST_APP)
	./$^

$(TEST_APP): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJECTS) $(TEST_APP)

deps:
	$(CXX) $(CXXFLAGS) -MM $(SOURCES) > deps.mk
	cat deps.mk

include deps.mk

