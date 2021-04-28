CXX ?= g++
ACK := ack
LIBTAPPP := ../libtappp/libtap++.so
WARNINGS := -Wall -Wextra -Weffc++
DEBUG := -ggdb3 -DDEBUG
CXXFLAGS := $(DEBUG) $(WARNINGS) -fPIC
PREFIX := /usr/local
LIBRARY_VAR := LD_LIBRARY_PATH
TEST_GOALS := ./00-sanity.t

all: $(TEST_GOALS)

build-libtappp:
	$(MAKE) -C ../libtappp

$(LIB): src/tap++.cpp include/tap++/tap++.h
	$(CXX) -shared -o $@ -Wl,-soname,$(LIB) $(CXXFLAGS) -Iinclude/ src/*.cpp

%.t: %.cpp build-libtappp
	$(CXX) $(CXXFLAGS) -I../libtappp/include -o $@ $<  -L../libtappp -ltap++

test: all
	@echo run_tests.pl $(TEST_GOALS)
	@$(LIBRARY_VAR)=../libtappp ../libtappp/run_tests.pl $(TEST_GOALS)

clean:
	-rm *.t 2>/dev/null

.PHONY: test clean all build-libtappp

