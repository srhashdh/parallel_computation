#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
// Karatsuba算法
int karatsuba(int x, int y) {
    // 基本情況：當x或y很小時，直接返回乘積
    if (x < 10 || y < 10) {
        return x * y;
    }

    // 計算中點
    int n = 1;
    int temp = x > y ? x : y;
    while (temp /= 10) {
        n++;
    }
    int m = n / 2;

    // 拆分數字
    int X1 = x / (int)pow(10, m);
    int X0 = x % (int)pow(10, m);
    int Y1 = y / (int)pow(10, m);
    int Y0 = y % (int)pow(10, m);

    // 定義子乘法的結果
    int A, B, C;
    
    // 使用多線程並行化計算三個乘法
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, karatsuba, (void *)X1, (void *)Y1);
    pthread_create(&tid2, NULL, karatsuba, (void *)X0, (void *)Y0);
    pthread_create(&tid3, NULL, karatsuba, (void *)(X1 + X0), (void *)(Y1 + Y0));
    
    pthread_join(tid1, &A);
    pthread_join(tid2, &B);
    pthread_join(tid3, &C);

    // 合併結果
    return A * (int)pow(10, 2*m) + (C - A - B) * (int)pow(10, m) + B;
}

// 測試
int main() {
    int result = karatsuba(1234, 5678);
    printf("Result: %d\n", result);
    return 0;
}

