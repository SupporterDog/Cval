# 컴파일러와 플래그 설정
CXX = g++
CC = gcc
CXXFLAGS = -Wall -lpthread `pkg-config --cflags --libs opencv2`
CFLAGS = -Wall

# 타겟 이름
TARGET = car_run

# 소스 파일
C_SOURCES = client.c car_run.c cal_path.c
CPP_SOURCES = qr_decoder.cpp

# 오브젝트 파일
C_OBJECTS = $(C_SOURCES:.c=.o)
CPP_OBJECTS = $(CPP_SOURCES:.cpp=.o)

# 기본 타겟
all: $(TARGET)

# 타겟 생성 규칙
$(TARGET): $(C_OBJECTS) $(CPP_OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

# 개별 오브젝트 파일 생성 규칙
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# 클린 규칙
clean:
	rm -f $(TARGET) $(C_OBJECTS) $(CPP_OBJECTS)

.PHONY: all clean
