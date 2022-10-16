#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

#define NUM_THREADS 20

#define BENCH(code)                                                                                           \
  do                                                                                                          \
  {                                                                                                           \
    const int RUNS = 10;                                                                                      \
                                                                                                              \
    long long total_duration = 0;                                                                             \
    long long min_duration = __LONG_LONG_MAX__;                                                               \
    long long max_duration = 0;                                                                               \
                                                                                                              \
    for (int i = 0; i < RUNS; i++)                                                                            \
    {                                                                                                         \
      long long start = time_in_ms();                                                                         \
                                                                                                              \
      code;                                                                                                   \
                                                                                                              \
      long long duration = time_in_ms() - start;                                                              \
                                                                                                              \
      if (duration > max_duration)                                                                            \
      {                                                                                                       \
        max_duration = duration;                                                                              \
      }                                                                                                       \
                                                                                                              \
      if (duration < min_duration)                                                                            \
      {                                                                                                       \
        min_duration = duration;                                                                              \
      }                                                                                                       \
                                                                                                              \
      total_duration += duration;                                                                             \
    }                                                                                                         \
                                                                                                              \
    double average = total_duration / RUNS;                                                                   \
                                                                                                              \
    printf("min_time=%lld(ms) max_time=%lld(ms) average_time=%f(ms)\n", min_duration, max_duration, average); \
  } while (0)

long long time_in_ms(void)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

/* Each thread will begin control in this function */
void *runner(void *param)
{
  BENCH({
    const int SIZE = 1000;

    int mat[SIZE][SIZE];

    srand(time(NULL));

    for (int i = 0; i < SIZE; i++)
    {
      for (int j = 0; j < SIZE; j++)
      {
        mat[i][j] = rand() % 100;
      }
    }

    int sum = 0;
    for (int i = 0; i < SIZE; i++)
    {
      for (int j = 0; j < SIZE; j++)
      {
        sum += mat[i][j];
      }
    }
  });

  pthread_exit(0);
}

void threads_with_scope_system()
{
  int i, scope;
  pthread_t tid[NUM_THREADS];
  pthread_attr_t attr;
  /* get the default attributes */
  pthread_attr_init(&attr);
  /* set the scheduling algorithm to PCS or SCS */
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  /* create the threads */
  for (i = 0; i < NUM_THREADS; i++)
  {
    pthread_create(&tid[i], &attr, runner, NULL);
  }

  /* now join on each thread */
  for (i = 0; i < NUM_THREADS; i++)
    pthread_join(tid[i], NULL);
}

void threads_with_process_scope()
{
  int i, scope;
  pthread_t tid[NUM_THREADS];
  pthread_attr_t attr;
  /* get the default attributes */
  pthread_attr_init(&attr);
  /* set the scheduling algorithm to PCS or SCS */
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
  /* create the threads */
  for (i = 0; i < NUM_THREADS; i++)
  {
    pthread_create(&tid[i], &attr, runner, NULL);
  }

  /* now join on each thread */
  for (i = 0; i < NUM_THREADS; i++)
    pthread_join(tid[i], NULL);
}

void threads_with_priority()
{
  int i, scope;
  pthread_t tid[NUM_THREADS];

  /* create the threads */
  for (i = 0; i < NUM_THREADS; i++)
  {
    pthread_attr_t attr;
    struct sched_param param;
    /* get the default attributes */
    pthread_attr_init(&attr);
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = 20;
    pthread_attr_setschedparam(&attr, &param);
    /* set the scheduling algorithm to PCS or SCS */
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    pthread_create(&tid[i], &attr, runner, NULL);
  }

  /* now join on each thread */
  for (i = 0; i < NUM_THREADS; i++)
    pthread_join(tid[i], NULL);
}

int main(int argc, char *argv[])
{
  printf("threads_with_scope_system\n");
  threads_with_scope_system();

  // printf("threads_with_scope_system\n");
  // threads_with_process_scope();

  printf("threads_with_priority\n");
  threads_with_priority();
}