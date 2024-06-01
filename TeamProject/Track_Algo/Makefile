# 컴파일러
CC = gcc

# 컴파일 옵션
CFLAGS = -Wall
LDFLAGS = -lwiringPi

# 타겟 파일명
TARGET = track

# 소스 파일
SRCS = alg.c track.c

# 오브젝트 파일
OBJS = $(SRCS:.c=.o)

# 기본 타겟
all: $(TARGET)

# 타겟 빌드 규칙
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# 오브젝트 파일 빌드 규칙
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# 클린 규칙
clean:
	rm -f $(TARGET) $(OBJS)

# 포스 클린 규칙
.PHONY: clean
