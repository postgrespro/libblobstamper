
BLOBSTAMPER_SRC := $(wildcard  ../blobstamper/*.cpp)
BLOBSTAMPER_OBJ := $(addsuffix .o, $(basename $(BLOBSTAMPER_SRC)))

EXAMPLES_SRC = $(wildcard  *.cpp)
EXAMPLES_BIN = $(basename $(EXAMPLES_SRC))


all: $(EXAMPLES_BIN)

build-libtappp:
	$(MAKE) -C ../libtappp

../blobstamper-pg/%.o: ../blobstamper-pg/%.cpp
	$(CXX) -c -g $(CXXFLAGS) -I .. $< -o $@

../wrappers/%.o: ../wrappers/%.cpp
	$(CXX) -c -g $(CXXFLAGS) -I .. $< -o $@


%: %.cpp $(BLOBSTAMPER_OBJ) $(BLOBSTAMPER_PG_OBJ) $(WRAPPERS_OBJ)
	$(CXX) $(CXXFLAGS) -I.. -o $@ $< $(BLOBSTAMPER_OBJ) 

clean:
	-rm $(EXAMPLES_BIN) 2>/dev/null

.PHONY: test clean all

