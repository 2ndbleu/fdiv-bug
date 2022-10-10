

TARGET	= fdiv-bug
SRCS	= main.c srt.c srt_table.c
CC	= gcc
CFLAGS	= -mabm -g -O2 -Wall
OBJS	= $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS)
