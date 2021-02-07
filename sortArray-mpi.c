#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int size, rank;
int process_sayisi=3;
int process_per_self=12/3;
int total;
int l_max;//Loaclly max
int l_min;//Loaclly min
int start,stop;
int* array;

//her process için 4 array elemanı düşecek

/*bu projede tek fonksiyonda en küçük ve en büyüğün bulunma sebebi
tekrar tekrar sıralama yapmamak ve bundan oluşan negatif etkiden kaçmaktır  -->buna bak*/


//küçükten büyüğe sıralama yaptırıyoruz
int min(int* array, int length){
    /**
     bu kısımda her process kendi içinde sıralama yapacak ve basa  en küçük olan gelecek
     */
    
    int min = array[0];
   
    for(int i=1; i < length; i++){
        if(array[i] < min){
            min = array[i];
        }
    }
    
    return min;
}

int max(int* array, int length) {
    int max = array[0];
    
    for(int i=1; i < length; i++){
        if(array[i] > max){
            max = array[i];
        }
    }
    
    return max;
}

void preProcessing(){
    //send the per by per for any process
  
    int j;
        if (rank == 0) { // Master process will send the data to all processes including itself
            array = (int*)malloc(12*sizeof(int));
            memset(array, 0, 12*sizeof(int));
            array[3] = -1;
            array[7] = -22;
            array[9] = -12;
            array[11]=10;
            
            for (j = 0; j < size; j++) {
                
                int index = j*process_per_self;
                MPI_Send(&array[index], process_per_self, MPI_INT, j, 0, MPI_COMM_WORLD);
            }
        }
    
    int* local_arr = (int*)malloc(sizeof(int)*process_per_self);
    MPI_Recv(local_arr, process_per_self, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int l_min = min(local_arr, process_per_self);
    int l_max = max(local_arr, process_per_self);
    
    int resp[] = {l_min, l_max};
    MPI_Send(resp, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
    
    
    if (rank == 0) {
        int* min_arr = (int*)malloc(sizeof(int)*process_sayisi);
        int* max_arr = (int*)malloc(sizeof(int)*process_sayisi);
        for (j = 0; j < size; j++) {
            int l_arr[2];
            MPI_Recv(l_arr, 2, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            min_arr[j] = l_arr[0];
            max_arr[j] = l_arr[1];
        }
    
        int minimum = min(min_arr, size);
        int maximum = max(max_arr, size);
        printf("Minimum: %d\n", minimum);
        printf("Maximum: %d\n", maximum);
    }
    
}



int main(int argc, char **argv) {
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    preProcessing();
   
    MPI_Finalize();
    return 0;
}
