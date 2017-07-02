CXX=g++-7

FILES= \
  src/json.cpp \
  src/main.cpp

CXXFLAGS=-std=c++17 -O2 -g

TESTFLAGS=-lmettle

default: test

.PHONY: all
all: test
	$(CXX) $(CXXFLAGS) $(FILES) -o main
	./main

.PHONY: test
test: test_buffered_writer
	mettle ./test_buffered_writer

test_buffered_writer: src/test_buffered_writer.cpp
	$(CXX) $(CXXFLAGS) src/test_buffered_writer.cpp -o test_buffered_writer $(TESTFLAGS)
