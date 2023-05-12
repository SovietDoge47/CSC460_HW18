#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MaxThread 4
#define size 100
int * arr;
pthread_t threads[4];
int counter = 0;
pthread_mutex_t mutex;

void merge(int * arr, int low, int mid, int high);
void printArr(int * arr);
void merge_sort(int * arr, int low, int high);
void * pthread_merge(void * arg);
void * pthread_merge_sort(void * arg);
void randomArrayGenerator(int * arr);

void * pthread_merge(void * arg){
    //lock
    int low = 0;
    int high = size - 1;
    pthread_mutex_lock(&mutex);
    if (counter == 4){
        counter++;
        int mid = low + (high - low + 1) / 4;
        int cur_high = (high + low) / 2;
        merge(arr, low, mid, cur_high);
    } else {
        counter++;
        int cur_low = (high + low) / 2 + 1;
        int mid = high - (high - low + 1) / 4;
        merge(arr, cur_low, mid, high);
    }
    pthread_mutex_unlock(&mutex);
    //unlock
    return NULL;
}

void * pthread_merge_sort(void * arg){
    int low =  0;
    int high = size - 1;
    pthread_mutex_lock(&mutex);
    if (counter == 0){
        merge_sort(arr, 0, low + (high - low + 1) / 4);
    } else if(counter == 1){
        merge_sort(arr, low + (high - low + 1) / 4 + 1, (high + low) / 2);
    } else if(counter == 2){
        merge_sort(arr, (high + low) / 2 + 1, high - (high - low + 1) / 4);
    } else {
        merge_sort(arr, high - (high - low + 1) / 4 + 1, high);
    }
    counter++;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

//arr, low, mid, high
//lower: 7
//upper: 
//sorted: 1, 2, 3, 4, 5, 6, 7
void merge(int * arr, int low, int mid, int high){
    //printf("low is %d, mid is %d, high is %d\n", low, mid, high);
    int * lower, * upper;
    lower = malloc(sizeof(int) * (mid - low + 1));
    upper = malloc(sizeof(int) * (high - mid));
    int j, k; //j is the counter for lower, and k is the counter for upper;
    int n = low;
    for(int j = 0; j < (mid - low + 1); j++){
        lower[j] = arr[n];
        n++;
    }
    for(int k = 0; k < high - mid; k++){
        upper[k] = arr[n];
        n++;
    }
    n = low;
    j = 0;
    k = 0;
    while(j < (mid - low + 1) && k < (high - mid)){
        if (lower[j] < upper[k]){
            arr[n] = lower[j];
            j++;
        } else {
            arr[n] = upper[k];
            k++;
        }
        n++;
    }
    while(j < mid - low + 1){
        arr[n] = lower[j];
        n++;
        j++;
    }
    while(k < high - mid){
        arr[n] = upper[k];
        n++;
        k++;
    }
    free(lower);
    free(upper);
}

void printArr(int * arr){
    for(int i = 0; i < size; i++){
        printf("%d, ", arr[i]);
    }
    printf("\n");
}

/*void printArr(int * arr){
    char * output;
    output = malloc(arr_size * 4 + 1);
    for(int i = 0; i < arr_size; i++){
        char strInt[3];
        sprintf(strInt, "%d", arr[i]);
        if (arr[i] > 9){
            output[i * 4] = strInt[0];
            output[i * 4 + 1] = strInt[1];
        } else {
            output[i * 4] = ' ';
            output[i * 4 + 1] = strInt[0];
        }
        output[i * 4 + 2] = ',';
        output[i * 4 + 3] = ' ';
    }
    output[4 * arr_size] = '\0';
    printf("%s\n", output);
    free(output);
}*/

//divide the array into two and recursively call merge_sort
void merge_sort(int * arr, int low, int high){
    if (low < high){
        int mid = (high + low) / 2;
        merge_sort(arr, low, mid);
        merge_sort(arr, mid + 1, high);
        merge(arr, low, mid, high);
    }
}

void randomArrayGenerator(int * arr){
    for(int i = 0; i < size; i++){
        arr[i] = i + 1;
    }
    time_t t;
    srand((unsigned) time(&t));
    for(int i = 0; i < size / 2; i++){
        int temp;
        temp = arr[i];
        int ran = rand() % size;
        arr[i] = arr[ran];
        arr[ran] = temp;
    }
}


int main(){
    pthread_mutex_init(&mutex, NULL);
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    //make your random array
    //merge sort
    //multithreading merge sort
    arr = malloc(sizeof(int) *  size);
    randomArrayGenerator(arr);
    printf("The original array is: \n");
    printArr(arr);
    //merge_sort(arr, 0, size - 1);
    //replace with multi-threading manner
    for(int i = 0; i < 4; i++){
        pthread_create(&threads[i], NULL, pthread_merge_sort, NULL);
    }
    for(int i = 0; i < 4; i++){
        pthread_join(threads[i], NULL);
    }
    printf("First Round Sorted array is: \n");
    printArr(arr);
    //call threads to merge first half and second half
    pthread_create(&threads[0], NULL, pthread_merge, NULL);
    pthread_join(threads[0], NULL);
    pthread_create(&threads[1], NULL, pthread_merge, NULL);
    pthread_join(threads[1], NULL);
    printf("Second Round Sorted array is: \n");
    printArr(arr);
    //call thread to merge the whole array.
    int low = 0;
    int high = size - 1;
    merge(arr, low, (low + high) / 2, high);
    printf("Final Round Sorted array is: \n");
    printArr(arr);
    free(arr);
    gettimeofday(&end_time, NULL);
    long ms = (end_time.tv_usec - start_time.tv_usec);
    printf("The running time is %ld ms.\n", ms);
    pthread_mutex_destroy(&mutex);
    return 0;
}