/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lifecycle.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/28 21:26:21 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/28 21:26:21 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	pick_up_forks(t_sim *s, t_philo *p, int fork1, int fork2)
{
	if (!check_death(p))
	{
		pthread_mutex_unlock(&s->mutexes[fork2]);
		pthread_mutex_unlock(&s->mutexes[fork1]);
		return (0);
	}
	s->forks[fork1] = 0;
	print_timestamp(p, "has taken a fork", WHITE);
	s->forks[fork2] = 0;
	print_timestamp(p, "has taken a fork", WHITE);
	pthread_mutex_unlock(&s->mutexes[fork2]);
	pthread_mutex_unlock(&s->mutexes[fork1]);
	if (!check_death(p))
		return (0);
	if (!philo_eat(p))
		return (0);
	p->sleep_flag = !p->sleep_flag;
	p->think_flag = 1;
	update_meal_count(p);
	return (1);
}

int	release_forks(t_sim *s, t_philo *p, int fork1, int fork2)
{
	pthread_mutex_lock(&s->mutexes[fork1]);
	pthread_mutex_lock(&s->mutexes[fork2]);
	s->fork_users[fork1] = p->id;
	s->fork_users[fork2] = p->id;
	s->forks[fork1] = 1;
	s->forks[fork2] = 1;
	return (1);
}

int	try_to_eat(t_sim *s, t_philo *p)
{
	int	fork1;
	int	fork2;

	fork1 = p->id % s->p_count;
	fork2 = p->id - 1;
	if (p->id & 1)
	{
		fork1 = p->id - 1;
		fork2 = p->id % s->p_count;
	}
	pthread_mutex_lock(&s->mutexes[fork1]);
	pthread_mutex_lock(&s->mutexes[fork2]);
	if (s->forks[fork1] && s->forks[fork2] && is_turn(p, fork1, fork2))
	{
		if (!pick_up_forks(s, p, fork1, fork2))
			return (0);
		if (!release_forks(s, p, fork1, fork2))
			return (0);
	}
	pthread_mutex_unlock(&s->mutexes[fork2]);
	pthread_mutex_unlock(&s->mutexes[fork1]);
	return (1);
}

// sleeps in increments of 1ms each time
int	philo_eat(t_philo *p)
{
	size_t	start_time;

	start_time = get_time();
	p->last_meal_time = start_time;
	pthread_mutex_lock(&p->s->print_mutex);
	printf("\033[0;32m%ldms\t%d is eating\n\033[0m", \
	start_time - p->s->start_time, p->id);
	pthread_mutex_unlock(&p->s->print_mutex);
	while (get_time() - start_time < (size_t)p->s->tte)
	{
		usleep(500);
		if (check_death(p) == 0)
			return (0);
	}
	return (1);
}

int	philo_sleep(t_philo *p)
{
	size_t	start_time;

	start_time = get_time();
	pthread_mutex_lock(&p->s->print_mutex);
	printf("\033[0;34m%ldms\t%d is sleeping\n\033[0m", \
	start_time - p->s->start_time, p->id);
	pthread_mutex_unlock(&p->s->print_mutex);
	while (get_time() - start_time < (size_t)p->s->tts)
	{
		usleep(500);
		if (check_death(p) == 0)
			return (0);
	}
	return (1);
}
