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
	if (p->s->end) // another philo died
	{
		pthread_mutex_unlock(&p->s->end_mutex);
		return (0);
	}
	if (curr_time - p->last_meal_time > p->s->ttd) // current philo died
	{
		p->s->end = 1;
		pthread_mutex_unlock(&p->s->end_mutex);
		pthread_mutex_lock(&p->s->print_mutex);
		printf("\033[0;31m%ldms\t%d has died\n\033[0m", curr_time - p->s->start_time, p->id);
		// instead of printing here, we can leave the id of the philo who died in s->end and THEN print death msg in main thread
		pthread_mutex_unlock(&p->s->print_mutex);
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

// sleeps in increments of 1ms each time
int	sleep_in_inc(t_philo *p, size_t time_to_sleep, char *msg, char *color, int change_time)
{
	size_t	start_time;

	start_time = get_time();
	if (change_time)
		p->last_meal_time = start_time;
	pthread_mutex_lock(&p->s->print_mutex);
	printf("%s%ldms\t%d %s\n\033[0m", color, start_time - p->s->start_time, p->id, msg);
	pthread_mutex_unlock(&p->s->print_mutex);
	while (get_time() - start_time < time_to_sleep)
	{
		usleep(500);
		if (check_death(p) == 0) // making all threads wait
		{
			printf("died here: %d %s\n", p->id, msg);
			return (0);
		}	
	}
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
	if (p->s->count_completed == p->s->num_philo) // all philos have eaten at least num_eat times
	{
		pthread_mutex_unlock(&p->s->meal_count_mutex);
		return (0);
	}
	pthread_mutex_unlock(&p->s->meal_count_mutex);
	return (1);
}
