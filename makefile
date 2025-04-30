CC = gcc
CFLAGS = -std=c11 -g -static
ASFLAGS = --generate-missing-build-notes=yes
LDFLAGS = -z noexecstack
SRCS = 1cc.c
OBJS = $(SRCS:.c=.o)
TARGET = 1cc

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -Wa,$(ASFLAGS) -c $< -o $@

test: $(TARGET)
	sh ./test.sh

clean:
	rm -f $(TARGET) $(OBJS) tmp tmp.s *~
