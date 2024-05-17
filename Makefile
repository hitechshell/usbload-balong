TARGET_EXEC := usbload

CFLAGS = -Wall -I.
LDFLAGS = -lcrypto

.PHONY: all clean

all: $(TARGET_EXEC)

$(TARGET_EXEC): main.o file.o protocol.o
	$(CXX) $(CFLAGS) $^ -o $@

clean:
	rm *.o
	rm $(TARGET_EXEC)
