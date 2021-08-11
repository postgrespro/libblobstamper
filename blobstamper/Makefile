
ALL_OBJS = blob.o \
helpers.o \
stamp.o \
stamp_arithm.o \
stamp_dict.o \
galley.o \
dict.o \


%.o: %.cpp $(DEPS)
	$(CXX) -c -g $(CXXFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -g $(CFLAGS) $<


all: $(ALL_OBJS)

.PHONY: clean
clean:
	rm -f *.o 