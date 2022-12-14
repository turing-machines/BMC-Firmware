CPPFLAGS +=
LDLIBS +=
 
all: helloworld
 
analyzestack: helloworld.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
 
clean:
	rm -f *.o helloworld
 
.PHONY: all clean