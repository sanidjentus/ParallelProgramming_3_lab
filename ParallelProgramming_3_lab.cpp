#include <iostream>
#include <thread>
#include <windows.h>
#include <atomic>

const size_t ARRAY_SIZE = 110;
const size_t THREADS = 4;

using namespace std;

void initRandomArray(int* arr) {
    for (size_t i = 0; i < ARRAY_SIZE; i++)
        arr[i] = rand() % 100 + 1;
    //arr[ARRAY_SIZE - 1] = -100; //мин четное число
    for (size_t i = 0; i < ARRAY_SIZE / 2; i++)
        arr[i] = 1;
}

void findMinEven(int* arr, size_t left, size_t right, atomic<int>& result){
    result = 1;
    for (size_t i = left; i < right; i++) {
        if (arr[i] % 2 == 0 && (result == 1 || arr[i] < result)) {
            result = arr[i];
        }
    }
}

int findMinEvenParallel(int* arr){
    atomic<int> result[THREADS];
    thread t[THREADS];
    size_t n = ARRAY_SIZE / THREADS;
    for (size_t i = 0; i < THREADS; i++) {
        if (i == THREADS - 1)
            t[i] = thread(findMinEven, arr, n * i, ARRAY_SIZE, ref(result[i])); //последний поток
        else
            t[i] = thread(findMinEven, arr, n * i, n * (i + 1), ref(result[i]));
    }

    for (size_t i = 0; i < THREADS; i++)
        t[i].join();
    
    int globalMinEven = 1;
    for (size_t i = 0; i < THREADS; i++) {
        if (result[i] % 2 == 0 && (globalMinEven == 1 || result[i] < globalMinEven))
            globalMinEven = result[i];
    }
    return globalMinEven;
}

int main(){
    int arr[ARRAY_SIZE];
    srand(GetTickCount64());
    initRandomArray(arr);
    atomic<int> minEven = 1;
    findMinEven(arr, 0, ARRAY_SIZE, minEven);
    cout << "Min even number without parallel: " << minEven << endl;
    cout << "Min even number with parallel: " << findMinEvenParallel(arr) << endl;
    return 0;
}