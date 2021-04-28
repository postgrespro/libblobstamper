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
blobstamper/stamp_pg_type_geo.o \



.PHONY: all
all: blob-stamper-all test test_pg_op_wrappers test_libblobstamper 
	@echo All done!

.PHONY: blob-stamper-all
blob-stamper-all:
	$(MAKE) -C blobstamper

test_pg_op_wrappers: blob-stamper-all $(LIB_OBJS) test_pg_op_wrappers.o  pg_op_wrappers.o
	$(CXX) $(LDFLAGS) $@.o -o $@  $(LDLIBS) $(BLOB_STAMPER_OBJ) pg_op_wrappers.o

test_libblobstamper: $(LIB_OBJS) test_libblobstamper.o blob-stamper-all
	$(CXX) $(LDFLAGS) $@.o -o $@ $(LDLIBS) $(BLOB_STAMPER_OBJ)


%.o: %.cpp $(DEPS)
	$(CXX) -c -g $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -g $(CXXFLAGS) $<

.PHONY: blob-stamper-clean

blob-stamper-clean:
	$(MAKE) -C blobstamper clean
	$(MAKE) -C t clean
	$(MAKE) -C libtappp clean



.PHONY: clean
clean: blob-stamper-clean
	rm -f *.o test test_pg_op_wrappers test_libblobstamper
	@echo Clean done!

.PHONY: test

test:
	$(MAKE) -C t test

