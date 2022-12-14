CPPFLAGS +=
LDLIBS +=
all: tpi
analyzestack: tpi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
clean:
	rm -f *.o tpi
.PHONY: all clean
