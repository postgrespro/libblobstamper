#DEPS = function.h
#LIB_OBJS = function.o

# https://stackoverflow.com/questions/18007326/how-to-change-default-values-of-variables-like-cc-in-makefile
ifeq ($(origin CC),default) 
	CC = gcc
endif

BLOB_STAMPER_OBJ = blobstamper/blob.o \
blobstamper/helpers.o \
blobstamper/stamp.o \
blobstamper/stamp_atomic.o \
blobstamper/stamp_dict.o \
blobstamper/galley.o \
blobstamper/dict.o \
blobstamper/stamp_pg_type_geo.o \


WRAPPERS_OBJ = pg_op_wrappers.o


.PHONY: all blob-stamper-all blob-stamper-clean clean test

#all: blob-stamper-all test_dict $(WRAPPERS_OBJ)
all: blob-stamper-all $(WRAPPERS_OBJ)
	@echo All done!

blob-stamper-all:
	$(MAKE) -C blobstamper

%.o: %.cpp $(DEPS)
	$(CXX) -c -g $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -g $(CXXFLAGS) $<

blob-stamper-clean:
	$(MAKE) -C blobstamper clean
	$(MAKE) -C t clean
	$(MAKE) -C libtappp clean

clean: blob-stamper-clean
	rm -f *.o
	@echo Clean done!

test:
	$(MAKE) -C t test

#test_dict: test_dict.o blob-stamper-all
#	$(CXX) $(LDFLAGS) $@.o -o $@ $(BLOB_STAMPER_OBJ)

