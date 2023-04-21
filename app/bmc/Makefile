CPPFLAGS += 
LDLIBS += -L./lib/ -lpthread -lgo -lcjson -ltpi_common -ldl -lm
 
all: bmc
 
bmc:  bmc.o comm_gpio.o param.o webserver.o  comm_device_serial.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	#cp $@ ~/tftpboot/
 
clean:
	rm -f *.o bmc
 
.PHONY: all clean
