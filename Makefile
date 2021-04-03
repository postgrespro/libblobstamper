#DEPS = function.h
#LIB_OBJS = function.o

# https://stackoverflow.com/questions/18007326/how-to-change-default-values-of-variables-like-cc-in-makefile
ifeq ($(origin CC),default) 
	CC = gcc
endif

.PHONY: all
all: test cpp_test
	@echo All done!


test: $(LIB_OBJS) test.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

cpp_test: $(LIB_OBJS) cpp_test.o libwaffleizer.o
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.o: %.cpp $(DEPS)
	$(CXX) -c -g $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -g $(CXXFLAGS) $<

.PHONY: clean
clean:
	rm -f *.o test cpp_test
	@echo Clean done!
