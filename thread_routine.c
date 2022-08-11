#include "mlx_part.h"
#include "color.h"
#include "thread_pool.h"
#include <math.h>
#include <stdlib.h>
#include "objects.h"
#include "ray_cast.h"
#include <assert.h>

static t_color	intensity_attenuation(t_color color, t_vec3 pos1, t_vec3 pos2)
{
	const int	unit = 128;
	double		dist;
	double		a[3];
	double		attenuation;

	a[0] = 1;
	a[1] = 0;
	a[2] = 0;
	dist = v3_l2norm(v3_sub(pos1, pos2)) / unit;
	attenuation = fmin(1, 1 / (a[0] + a[1] * dist + a[2] * dist * dist));
	return (color_scale(color, attenuation));
}

static void	ft_fill_pixel(t_mlx *mlx, int x, int y, unsigned int color)
{
	unsigned int	s[2];

	s[0] = -1;
	while (++s[0] < (mlx->edit + 1) && s[0] + x < mlx->width)
	{
		s[1] = -1;
		while (++s[1] < (mlx->edit + 1) && s[1] + y < mlx->height)
			ft_mlx_set_pixel_color(mlx->image, x + s[0], y + s[1], color);
	}
}

static double	get_intersect_distancer(t_obj_base *objlst, \
								t_obj_base **intersecting_obj_out, \
								t_ray ray)
{
	t_obj_base			*target_obj;
	t_obj_base			*intersect_obj;
	double				dist[2];

	dist[0] = INFINITY;
	intersect_obj = NULL;
	target_obj = objlst;
	while (target_obj)
	{
		dist[1] = intersect(ray, target_obj);
		if ((isnan(dist[1]) == FALSE) && (dist[1] < dist[0]))
		{
			dist[0] = dist[1];
			*intersecting_obj_out = target_obj;
		}
		target_obj = target_obj->next;
	}
	if (*intersecting_obj_out == NULL)
		return (INFINITY);
	if (dist[0] < EPSILON)
		return (NAN);
	return (dist[0]);
}


static t_color	single_ray_castt(t_mlx *mlx, t_ray ray)
{
	t_obj_base	*intersect_obj;
	t_vec3		intersect;
	t_color		c;
	double		dist;

	intersect_obj = NULL;
	dist = get_intersect_distancer(mlx->scene->obj, &intersect_obj, ray);
	if (isinf(dist) == TRUE || isnan(dist) == TRUE)
		return (rgb_color(0, 0, 0));
	else
	{
		intersect = v3_mul(ray.dir, dist - EPSILON);
		intersect = v3_add(intersect, ray.org);
		c = phong_reflection(mlx, intersect_obj, intersect, ray.org);
		return (intensity_attenuation(c, intersect, ray.org));
	}
}



void *thread_routine(void *ptr)
{
	const Task	*tlo = ptr;
	t_mlx					*mlx;
	double			d;
	unsigned int	pixel[2];
	t_color			color;
	t_ray			ray;

	mlx = tlo->mlx;
	assert(mlx);
	d = ((double)tlo->mlx->width / 2) / tan(tlo->mlx->scene->cam->hfov / 2);
	pixel[1] = 0;
	while (pixel[1] < mlx->height/2)
	{
		pixel[0] = 0;
		while (pixel[0] < mlx->width/2)
		{
			ray.dir = v3_normalize(make_v3((int)(tlo->x + pixel[0] - mlx->width / 2), \
										(int)(tlo->y + pixel[1] - mlx->height / 2), d));
			ray.org = mlx->scene->cam->pos;
			color = single_ray_castt(mlx, ray);
			ft_fill_pixel(mlx, tlo->x + pixel[0], tlo->y + pixel[1], color_to_hex(color));
			pixel[0] += (mlx->edit + 1);
		}
		pixel[1] += (mlx->edit + 1);
	}

	return NULL;
}
