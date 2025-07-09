CXX ?= g++
ACK := ack
LIBTAPPP := ./libtappp/libtap++.so
WARNINGS := -Wall -Wextra -Weffc++
DEBUG := -ggdb3 -DDEBUG
#CXXFLAGS := $(DEBUG) $(WARNINGS) -fPIC
PREFIX := /usr/local
LIBRARY_VAR := LD_LIBRARY_PATH


BLOBSTAMPER_SRC := $(wildcard  ../src/*.cpp)
BLOBSTAMPER_OBJ := $(addsuffix .o, $(basename $(BLOBSTAMPER_SRC)))

LIBTAPPP_SRC := $(wildcard  ./libtappp/src/*.cpp)
LIBTAPPP_OBJ := $(addsuffix .o, $(basename $(LIBTAPPP_SRC)))

TEST_SRC := $(wildcard  ./*.cpp)
TEST_BIN := $(addsuffix .t, $(basename $(TEST_SRC)))


all: $(TEST_BIN)

libtappp/src/%.o: libtappp/src/%.cpp
	$(CXX) -c -g $(CXXFLAGS) -I libtappp/include $< -o $@

../src/%.o: ../src/%.cpp
	$(CXX) -c -g $(CXXFLAGS) -I ../include $< -o $@


%.t: %.cpp $(BLOBSTAMPER_OBJ) $(LIBTAPPP_OBJ)
	$(CXX) $(CXXFLAGS) -I./libtappp/include -g -I../include -o $@ $< $(BLOBSTAMPER_OBJ) $(LIBTAPPP_OBJ)

test: all
	@echo run_tests.pl $(TEST_BIN)
	libtappp/run_tests.pl $(TEST_BIN)

clean:
	-rm *.t 2>/dev/null

.PHONY: test clean all build-libtappp build-libblobstamper

