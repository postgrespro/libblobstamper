
# Guide used while writing this makefile is
# https://stackoverflow.com/questions/68428103/how-to-create-a-makefile-for-a-c-project-with-multiple-directories


# https://stackoverflow.com/questions/18007326/how-to-change-default-values-of-variables-like-cc-in-makefile
ifeq ($(origin CC),default) 
	CC = gcc
endif

BLOBSTAMPER_SRC := $(wildcard  blobstamper/*.cpp)
BLOBSTAMPER_OBJ := $(addsuffix .o, $(basename $(BLOBSTAMPER_SRC)))

.PHONY: all blob-stamper-all blob-stamper-clean clean test gagaga

all: $(BLOBSTAMPER_OBJ)
	@echo All done!

blobstamper/%.o: blobstamper/%.cpp
	$(CXX) -c -g $(CXXFLAGS) $< -o $@


# blob-stamper-all:
#	$(MAKE) -C blobstamper

#%.o: %.cpp $(DEPS)
#	$(CXX) -c -g $(CFLAGS) $<

#%.o: %.c $(DEPS)
#	$(CC) -c -g $(CXXFLAGS) $<



blob-stamper-clean:
	rm -f *.o

clean: blob-stamper-clean
	$(MAKE) -C blobstamper clean
	$(MAKE) -C t clean
	$(MAKE) -C libtappp clean
	@echo Clean done!

test:
	$(MAKE) -C t test

#test_dict: test_dict.o blob-stamper-all
#	$(CXX) $(LDFLAGS) $@.o -o $@ $(BLOB_STAMPER_OBJ)


gagaga:
	@echo ------- $(BLOBSTAMPER_OBJ)

#all: blob-stamper-all test_dict $(WRAPPERS_OBJ)
#all: blob-stamper-all $(WRAPPERS_OBJ)
#	@echo All done!
