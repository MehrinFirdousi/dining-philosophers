/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/28 23:19:31 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/28 23:19:31 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// returns 0 if someone died 
int	check_death(t_philo *p)
{
	struct timeval	time;
	size_t			curr_time;

	gettimeofday(&time, NULL);
	curr_time = (time.tv_sec) * 1000 + (time.tv_usec) / 1000;
	pthread_mutex_lock(&p->s->end_mutex);
	if (p->s->end > 0)
	{
		pthread_mutex_unlock(&p->s->end_mutex);
		return (0);
	}
	if (curr_time - p->last_meal_time > (size_t)p->s->ttd)
	{
		p->s->end = p->id;
		pthread_mutex_unlock(&p->s->end_mutex);
		return (0);
	}
	pthread_mutex_unlock(&p->s->end_mutex);
	return (1);
}

int	is_turn(t_philo *p, int fork1, int fork2)
{
	if (p->s->fork_users[fork1] == p->id || p->s->fork_users[fork2] == p->id)
		return (0);
	return (1);
}

void	update_meal_count(t_philo *p)
{
	p->num_meals++;
	if (p->s->num_eat != -1)
	{
		pthread_mutex_lock(&p->s->meal_count_mutex);
		if (p->num_meals == p->s->num_eat)
			p->s->count_completed++;
		pthread_mutex_unlock(&p->s->meal_count_mutex);
	}
}

// returns 0 if all philos are done eating num_eat times
int	check_num_meals(t_philo *p)
{
	pthread_mutex_lock(&p->s->meal_count_mutex);
	if (p->s->count_completed == p->s->p_count)
	{
		pthread_mutex_unlock(&p->s->meal_count_mutex);
		return (0);
	}
	pthread_mutex_unlock(&p->s->meal_count_mutex);
	return (1);
}

void	check_one_philo(t_sim *s)
{
	if (s->p_count == 1)
	{
		usleep(s->ttd * 1000);
		pthread_mutex_lock(&s->end_mutex);
		s->end = 1;
		pthread_mutex_unlock(&s->end_mutex);
	}
}
