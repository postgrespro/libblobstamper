CXX ?= g++
ACK := ack
LIBTAPPP := ../libtappp/libtap++.so
WARNINGS := -Wall -Wextra -Weffc++
DEBUG := -ggdb3 -DDEBUG
#CXXFLAGS := $(DEBUG) $(WARNINGS) -fPIC
PREFIX := /usr/local
LIBRARY_VAR := LD_LIBRARY_PATH


BLOBSTAMPER_SRC := $(wildcard  ../blobstamper/*.cpp)
BLOBSTAMPER_OBJ := $(addsuffix .o, $(basename $(BLOBSTAMPER_SRC)))

TEST_SRC := $(wildcard  ./*.cpp)
TEST_BIN := $(addsuffix .t, $(basename $(TEST_SRC)))


all: build-libtappp $(TEST_BIN)

build-libtappp:
	$(MAKE) -C ../libtappp

%.t: %.cpp $(BLOBSTAMPER_OBJ)
	$(CXX) $(CXXFLAGS) -I../libtappp/include -g -I.. -o $@ $< $(BLOBSTAMPER_OBJ)  -L../libtappp -ltap++

test: all
	@echo run_tests.pl $(TEST_BIN)
	@$(LIBRARY_VAR)=../libtappp ../libtappp/run_tests.pl $(TEST_BIN)

clean:
	-rm *.t 2>/dev/null

.PHONY: test clean all build-libtappp build-libblobstamper

