#include <sys/time.h>

int main()
{

    int i;

    double diff;
    struct timeval start, end;

    printf("start\n");

    for(i = 0; i < 5; i++)
    {


        int test2 = gettimeofday(&start, NULL);
        sleep(5);
        int test3 = gettimeofday(&end, NULL);


        diff = 1e6*end.tv_sec + end.tv_usec - (1e6*start.tv_sec + start.tv_usec);
        printf("diff:%f\n", diff);

    }
}
