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
	
	if (sleep_in_inc(p, s->tte, "is eating", GREEN, 1) == 0)
		return (0);
	// p->last_meal_time = get_time();
	// print_timestamp(p, "is eating", GREEN);
	// usleep(s->tte * 1000);

	p->sleep_flag = !p->sleep_flag;
	p->think_flag = 1;
	update_meal_count(p);
	return (1);
}

int	release_forks(t_sim *s, t_philo *p, int fork1, int fork2)
{
	pthread_mutex_lock(&s->mutexes[fork1]);
	pthread_mutex_lock(&s->mutexes[fork2]);
	if (!check_death(p))
	{
		pthread_mutex_unlock(&s->mutexes[fork2]);
		pthread_mutex_unlock(&s->mutexes[fork1]);
		return (0);
	}
	s->fork_users[fork1] = p->id;
	s->fork_users[fork2] = p->id;
	s->forks[fork1] = 1;
	s->forks[fork2] = 1;
	return (1);
}

int	try_to_eat(t_sim *s, t_philo *p)
{
	int fork1;
	int fork2;

	fork1 = p->id % s->num_philo;
	fork2 = p->id - 1;
	if (p->id & 1)
	{
		fork1 = p->id - 1;
		fork2 = p->id % s->num_philo;
	}
	// if (!check_death(p))
	// 	return (0);
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

void	philo_sleep(t_sim *s, t_philo *p)
{
	p->sleep_flag = !p->sleep_flag;
	p->think_flag = 1;
	sleep_in_inc(p, s->tts, "is sleeping", BLUE, 0);
}

int	philo_think(t_philo *p)
{
	if (!check_death(p))
		return (0);
	p->think_flag = 0;
	print_timestamp(p, "is thinking", YELLOW);
	return (1);
}