CPPFLAGS +=
LDLIBS += -L./lib/ -lpthread -lgo -lcjson
 
all: bmctest
 
bmctest:  bmctest.o comm_gpio.o param.o webserver.o  comm_device_serial.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	cp $@ ~/tftpboot/
 
clean:
	rm -f *.o bmctest
 
.PHONY: all clean