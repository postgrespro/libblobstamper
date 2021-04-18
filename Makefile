#DEPS = function.h
#LIB_OBJS = function.o

# https://stackoverflow.com/questions/18007326/how-to-change-default-values-of-variables-like-cc-in-makefile
ifeq ($(origin CC),default) 
	CC = gcc
endif

.PHONY: all
all: test test_pg_op_wrappers test_libblobstamper
	@echo All done!


test: $(LIB_OBJS) test.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test_pg_op_wrappers: $(LIB_OBJS) test_pg_op_wrappers.o libblobstamper.o pg_op_wrappers.o
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test_libblobstamper: $(LIB_OBJS) libblobstamper.o test_libblobstamper.o
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)



%.o: %.cpp $(DEPS)
	$(CXX) -c -g $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -g $(CXXFLAGS) $<

.PHONY: clean
clean:
	rm -f *.o test test_pg_op_wrappers test_libblobstamper
	@echo Clean done!
