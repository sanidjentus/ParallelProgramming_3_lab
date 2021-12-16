#include <iostream>
#include <thread>
#include <windows.h>
#include <atomic>

const size_t ARRAY_SIZE = 110;
const size_t THREADS = 4;

using namespace std;

CRITICAL_SECTION cs;

atomic_flag aflag = ATOMIC_FLAG_INIT;

void initRandomArray(int* arr) {
    for (size_t i = 0; i < ARRAY_SIZE; i++)
        arr[i] = rand() % 100 + 1;
    //arr[ARRAY_SIZE - 1] = -100; //мин четное число
    for (size_t i = 0; i < ARRAY_SIZE / 2; i++)
        arr[i] = 1;
}

void findMinEven(int* arr, size_t left, size_t right, int& result) {
    int local_result = 1;
    
    for (size_t i = left; i < right; i++) {
        if (arr[i] % 2 == 0 && (local_result == 1 || arr[i] < local_result)) 
            local_result = arr[i];
    }
    
    while (aflag.test_and_set())
        Sleep(0);
    if (local_result % 2 == 0 && (result == 1 || local_result < result))
        result = local_result;
    aflag.clear();
    

    /*InitializeCriticalSection(&cs);
    EnterCriticalSection(&cs);
    result = local_result;
    Sleep(0);
    LeaveCriticalSection(&cs);
    */
}

int findMinEvenParallel(int* arr) {
    int globalMinEven = 1;
    thread t[THREADS];
    size_t n = ARRAY_SIZE / THREADS;
    for (size_t i = 0; i < THREADS; i++) {
        if (i == THREADS - 1)
            t[i] = thread(findMinEven, arr, n * i, ARRAY_SIZE, ref(globalMinEven));
        else
            t[i] = thread(findMinEven, arr, n * i, n * (i + 1), ref(globalMinEven));
    }

    for (size_t i = 0; i < THREADS; i++)
        t[i].join();

   /* int globalMinEven = 1;
    for (size_t i = 0; i < THREADS; i++) {
        if (globalMinEven == 1 || result[i] < globalMinEven)
            globalMinEven = result[i];
    }*/
    return globalMinEven;
}

int main() {
    int arr[ARRAY_SIZE];
    srand(GetTickCount64());
    initRandomArray(arr);
    int minEven = 1;
    //atomic<int> minEven;
    findMinEven(arr, 0, ARRAY_SIZE, minEven);
    cout << "Min even number without parallel: " << minEven << endl;
    cout << "Min even number with parallel: " << findMinEvenParallel(arr) << endl;
    return 0;
}