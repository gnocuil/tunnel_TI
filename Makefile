#CC     := arm-openwrt-linux-uclibcgnueabi-gcc
CC     := gcc
CFLAGS := -O2 
TARGET := tunnel
OBJS   := main.o tun.o network.o socket.o 

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	
clean :
	rm -f $(TARGET)
	rm -f *.o
