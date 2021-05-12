
ALL_OBJS = blob.o \
helpers.o \
stamp.o \
stamp_atomic.o \
stamp_dict.o \
galley.o \
dict.o \
stamp_pg_type_geo.o \



%.o: %.cpp $(DEPS)
	$(CXX) -c -g $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -g $(CXXFLAGS) $<


all: $(ALL_OBJS)

.PHONY: clean
clean:
	rm -f *.o 