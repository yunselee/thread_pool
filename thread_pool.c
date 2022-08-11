#include <pthread.h>
#include "thread_pool.h"
#include "color.h"
#include <stdatomic.h>
#include <stdio.h>

static Task taskQueue[256];

static pthread_mutex_t mutexQueue;
static pthread_cond_t condQueue;
static int taskCount = 0;
static pthread_t th[THREAD_NUM];
atomic_int cnt;

void pool_destroy()
{
	int i;

	i = 0;
	for(i = 0; i < THREAD_NUM; i++)
	{
		pthread_join(th[i], NULL);
	}
	pthread_mutex_destroy(&mutexQueue);
	pthread_cond_destroy(&condQueue);
}

void submit_task(Task task)
{
	pthread_mutex_lock(&mutexQueue);
	taskQueue[taskCount] = task;
	taskCount++;
	pthread_mutex_unlock(&mutexQueue);
	pthread_cond_signal(&condQueue);
}

void thread_start()
{
	cnt++;
}

void thread_end()
{
	cnt--;
}

int is_all_thread_end()
{
	return (cnt == 0 && (start = TRUE));
}

void* start_thread(void* args)
{
	while(1)
	{
		Task task;
		int found = 0;
		pthread_mutex_lock(&mutexQueue);
		while (taskCount == 0)
		{
			pthread_cond_wait(&condQueue, &mutexQueue);
		}

		found = 1;
		task = taskQueue[0];
		int i;
		for (i =0 ; i < taskCount - 1 ; i++)
		{
			taskQueue[i] = taskQueue[i+1];
		}
		taskCount--;

		pthread_mutex_unlock(&mutexQueue);
		thread_start();
		start = TRUE;
		thread_routine(&task);
		thread_end();
	}
}

void	init_task(Task *task, t_mlx *mlx)
{
	int	i;

	i = 0;
	while (i < THREAD_NUM)
	{
		(task + i)->mlx = mlx;
		(task + i)->x = mlx->width/2 * (i/2);
		(task + i)->y = mlx->height/2 * (i%2);
		i++;
	}
}


void pool_init(void)
{
	pthread_mutex_init(&mutexQueue, NULL);
	pthread_cond_init(&condQueue, NULL);
	int i;
	for(i = 0 ; i <THREAD_NUM; i++)
	{
		if (pthread_create(&th[i], NULL, &start_thread,NULL) != 0)
		{
			printf("pthread create something error\n");
		}
	}
}


void pool_execute(t_mlx *mlx)
{
	static pthread_t					thread_data[THREAD_NUM];
	static Task				task[THREAD_NUM];
	int i;

	init_task(task, mlx);
	i = 0;
	while (i < THREAD_NUM)
	{
		submit_task(task[i]);
		i++;
	}
}

