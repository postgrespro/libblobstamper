
ALL_OBJS = blob.o helpers.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -g $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -g $(CXXFLAGS) $<


all: $(ALL_OBJS)

.PHONY: clean
clean:
	rm -f *.o 