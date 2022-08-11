#ifndef THREAD_POOL_H
# define THREAD_POOL_H

#define THREAD_NUM 4
#include "mlx_part.h"

// typedef struct s_thread_local_object
// {
// 	t_mlx	*mlx;
// 	int		x;
// 	int		y;
// }t_thread_local_object;

#define THREAD_NUM 4

typedef struct Task{
	t_mlx	*mlx;
	int		x;
	int		y;
}Task;

void thread_start();
void thread_end();
int is_all_thread_end();

void *thread_routine(void *ptr);
void pool_execute(t_mlx *mlx);
void pool_init();

int start;

#endif