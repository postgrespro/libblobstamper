CXX ?= g++
ACK := ack
LIBTAPPP := ../libtappp/libtap++.so
WARNINGS := -Wall -Wextra -Weffc++
DEBUG := -ggdb3 -DDEBUG
CXXFLAGS := $(DEBUG) $(WARNINGS) -fPIC
PREFIX := /usr/local
LIBRARY_VAR := LD_LIBRARY_PATH
TEST_GOALS := ./00-sanity.t ./001-blob-generic.t ./100-stamp-base.t ./110-stamp-atomic.t ./200-dict.t ./300-galley.t ./500-stamp-pg-type-geo.t ./700-wrapper-pg-type-geo.t

all: build-libtappp $(TEST_GOALS)


# FIXME this list dublicates same list in ../Makefile
# should remove it for deduplication
BLOB_STAMPER_OBJ = ../blobstamper/blob.o \
../blobstamper/helpers.o \
../blobstamper/dict.o \
../blobstamper/stamp.o \
../blobstamper/stamp_atomic.o \
../blobstamper/stamp_dict.o \
../blobstamper/galley.o \
../blobstamper/stamp_pg_type_geo.o \
../pg_op_wrappers.o \


build-libtappp:
	$(MAKE) -C ../libtappp

%.t: %.cpp
	$(CXX) $(CXXFLAGS) -I../libtappp/include -I.. -o $@ $< $(BLOB_STAMPER_OBJ)  -L../libtappp -ltap++

test: all
	@echo run_tests.pl $(TEST_GOALS)
	@$(LIBRARY_VAR)=../libtappp ../libtappp/run_tests.pl $(TEST_GOALS)

clean:
	-rm *.t 2>/dev/null

.PHONY: test clean all build-libtappp build-libblobstamper

