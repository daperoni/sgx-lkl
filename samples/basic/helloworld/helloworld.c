#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>

#define HW_FILE "app/helloworld.txt"
#define BUFFER_SIZE 128000
#define BENCH_ITERATIONS 100000
#define DEFAULT_BLOCK_SIZE 128

#define SIZE 1
#define ENCLAVE_HOLESIZE (1024*1024)
#ifdef ENCLAVE_HOLESIZE
static char hole[SIZE][ENCLAVE_HOLESIZE] = {0};
#endif

void xor_benchmark(uint64_t *array_in, uint64_t *array_out, size_t count) {
    volatile uint64_t *p = array_in;
    volatile uint64_t *q = array_out;
    while (count--) {
        *q = *p ^ 0xFFFFFFFFFFFFFFFF;
        p++;
        q++;
    }
} 

void xor_block_copy_benchmark(uint64_t *array_in, uint64_t *array_out, size_t count, int block_size) {
    volatile uint64_t *in = array_in;
    volatile uint64_t *out = array_out;
    if(block_size < sizeof(uint64_t)){
        block_size = sizeof(uint64_t);
    }
    int step_size = block_size/sizeof(uint64_t);
    count = count/step_size;
    volatile uint64_t * temp_buff = (uint64_t *)malloc(block_size);
    while (count--) {
        memcpy((void*)temp_buff, (void*)in, block_size);
        for(int i = 0; i < step_size; i++){
            temp_buff[i] = temp_buff[i] ^ 0xFFFFFFFFFFFFFFFF;
        }
        memcpy((void*)out, (void*)temp_buff, block_size);
        in+=step_size;
        out+=step_size;
    }
    free((void*)temp_buff);
} 

void xor_block_copy_benchmark_no_memcpy(uint64_t *array_in, uint64_t *array_out, size_t count, int block_size) {
    volatile uint64_t *in = array_in;
    volatile uint64_t *out = array_out;
    if(block_size < sizeof(uint64_t)){
        block_size = sizeof(uint64_t);
    }
    int step_size = block_size/sizeof(uint64_t);
    count = count/step_size;
    volatile uint64_t * temp_buff = (uint64_t *)malloc(block_size);
    while (count--) { 
        for(int i = 0; i < step_size; i++){
            temp_buff[i] = in[i];
        }
        for(int i = 0; i < step_size; i++){
            temp_buff[i] = temp_buff[i] ^ 0xFFFFFFFFFFFFFFFF;
        }
        for(int i = 0; i < step_size; i++){
            out[i] = temp_buff[i];
        }
        in+=step_size;
        out+=step_size;
    }
    free((void*)temp_buff);
} 


void task(){
	size_t bufsize = BUFFER_SIZE;
	void *buffer_in;
	if (posix_memalign(&buffer_in, 64, bufsize) != 0 || !buffer_in) {
		printf("aligned buffer allocation failure\n");
		return;
	}
	memset(buffer_in, 0, bufsize);

	void *buffer_out;
	if (posix_memalign(&buffer_out, 64, bufsize) != 0 || !buffer_out) {
		printf("aligned buffer allocation failure\n");
		return;
	}
	memset(buffer_out, 0, bufsize);

	size_t count = bufsize / sizeof(uint64_t);

	for(int i = 0; i < BENCH_ITERATIONS; i++){
		//xor_benchmark((uint64_t*) buffer_in, (uint64_t*) buffer_out, count);
		xor_block_copy_benchmark((uint64_t *)buffer_in, (uint64_t *)buffer_out, count, DEFAULT_BLOCK_SIZE);
		//xor_block_copy_benchmark_no_memcpy((uint64_t *)buffer_in, (uint64_t *)buffer_out, count, DEFAULT_BLOCK_SIZE);
	}
}


int main(int argc, char** argv)
{
    char buf[100];
    unsigned long long a, b;
    unsigned int aux;
    long long cycles_per_second;
    clock_t t;
    struct timeval start, end;


    FILE* f = fopen(HW_FILE, "r");
    if (!f)
    {
        fprintf(
            stderr, "Could not open file %s: %s\n", HW_FILE, strerror(errno));
        exit(1);
    }

    volatile int val = 10;
    hole[0][val] = 'g';
    printf("Hello, I've modified the program\n");
    printf("The hole size is %ldMB\n", sizeof(hole)/(1024*1024));
    printf("%c", hole[0][val]);
    // Prints first line of file /app/helloworld.txt (max 100 characters)
    if (fgets(buf, sizeof(buf), f) == buf)
    {
        printf("%s", buf);
    }
    else
    {
        fprintf(
            stderr,
            "Could not read first line of file %s: %s\n",
            HW_FILE,
            strerror(errno));
        exit(1);
    }
   /* a = __rdtscp(aux);
    sleep(1);
    b = __rdtscp(aux);
    cycles_per_second = b - a;

    printf("Cycles per second %lld\n", cycles_per_second);
*/
    gettimeofday(&start, NULL);
    task();
    gettimeofday(&end, NULL);
    long secs_used, micros_used;
    secs_used=(end.tv_sec - start.tv_sec); //avoid overflow by subtracting first
    micros_used= ((secs_used*1000000) + end.tv_usec) - (start.tv_usec);
    printf("Time taken for task per: %lf\n", (double)micros_used/BENCH_ITERATIONS);


    t = clock();
    task();
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("Clock time for task: %f\n", time_taken);

    return 0;
}
