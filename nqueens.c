// from the Cilk manual: http://supertech.csail.mit.edu/cilk/manual-5.4.6.pdf
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

int safe(char *config, int i, int j);
void *thread_wrapper(void *config_arg);
void nqueens(char *config, int i);

int safe(char *config, int i, int j)
{
    int r, s;

    for (r = 0; r < i; r++)
    {
        s = config[r];
        if (j == s || i - r == j - s || i - r == s - j)
            return 0;
    }
    return 1;
}

int count = 0;
sem_t s;
int n;

void *thread_wrapper(void *config_arg)
{
    char p = *(char *)config_arg;
    char *config = malloc(n * sizeof(char));
    config[0] = p;
    nqueens(config, 1);
    free(config);
    return NULL;
}

void nqueens(char *config, int i)
{
    char *new_config;
    int j;

    if (i >= n)
    {
        sem_wait(&s);{
            count++;
        }sem_post(&s);
    }

    /* try each possible position for queen <i> */
    for (j = 0; j < n; j++)
    {
        /* allocate a temporary array and copy the config into it */
        new_config = malloc((i + 1) * sizeof(char));
        memcpy(new_config, config, i * sizeof(char));
        if (safe(new_config, i, j))
        {
            new_config[i] = j;
            nqueens(new_config, i + 1);
        }
        free(new_config);
    }
    return;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("%s: number of queens required\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);
    printf("running queens %d\n", n);

    sem_init(&s, 0, 1);
    pthread_t pt[n];
    char pr[n];
    for (int i = 0; i < n; i++)
    {
        // pthread create
        pr[i] = (char)i;
        pthread_create(&pt[i], NULL, thread_wrapper, &pr[i]);
    }
    for (int i = 0; i < n; i++)
    {
        // pthread join
        pthread_join(pt[i], NULL);
    }
    sem_destroy(&s);
    printf("# solutions: %d\n", count);
    return 0;
}
