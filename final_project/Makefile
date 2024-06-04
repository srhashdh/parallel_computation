# 定义变量
CC = mpicc
CFLAGS = -Wall -g
LDFLAGS = -lgmp -lm
TARGET = a.out
SRCS = main.c gen_e.c gen_d.c gen_prime.c karatsuba.c

.PHONY: all clean run

# 默认目标
all: $(TARGET) run

# 目标规则
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)

run: $(TARGET)
	mpirun --oversubscribe -n $(shell getconf _NPROCESSORS_ONLN) ./$(TARGET)

# 清理规则
clean:
	rm -f $(TARGET)

