CPPFLAGS += 
LDLIBS += -L./lib/ -lpthread -lgo -lcjson -ltpi_rs -ldl -lm -lrt -lusb-1.0
 
all: bmc
 
bmc:  bmc.o comm_gpio.o param.o webserver.o  comm_device_serial.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	#cp $@ ~/tftpboot/
 
clean:
	rm -f *.o bmc
 
.PHONY: all clean
