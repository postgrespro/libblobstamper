
# Guide used while writing this makefile is
# https://stackoverflow.com/questions/68428103/how-to-create-a-makefile-for-a-c-project-with-multiple-directories


# https://stackoverflow.com/questions/18007326/how-to-change-default-values-of-variables-like-cc-in-makefile
ifeq ($(origin CC),default)
	CC = gcc
endif

BLOBSTAMPER_SRC := $(wildcard  src/*.cpp)
BLOBSTAMPER_OBJ := $(addsuffix .o, $(basename $(BLOBSTAMPER_SRC)))

EXAMPLES_SRC = $(wildcard  examples/*.cpp)
EXAMPLES_BIN = $(basename $(EXAMPLES_SRC))


.PHONY: all blob-stamper-all blob-stamper-clean clean test

all: blob-stamper-all
	@echo All done!

src/%.o: src/%.cpp
	$(CXX) -c -g $(CXXFLAGS) -I include $< -o $@

blob-stamper-all: $(BLOBSTAMPER_OBJ)

blob-stamper-clean:
	rm -f src/*.o

clean: blob-stamper-clean
	$(MAKE) -C t clean
	$(MAKE) -C examples clean
	$(MAKE) -C console_demo clean
	@echo Clean done!

test:
	$(MAKE) -C t test

examples: $(BLOBSTAMPER_OBJ)
	$(MAKE) -C examples

