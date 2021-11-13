

BLOBSTAMPER_SRC := $(wildcard  ./*.cpp)
BLOBSTAMPER_OBJ := $(addsuffix .o, $(basename $(BLOBSTAMPER_SRC)))


%.o: %.cpp
	$(CXX) -c -g $(CXXFLAGS) -I .. $< -o $@


all: $(BLOBSTAMPER_OBJ)

.PHONY: clean
clean:
	rm -f *.o 