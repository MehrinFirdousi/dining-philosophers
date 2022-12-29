/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/23 14:41:28 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/23 14:41:28 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_atoi(const char *nptr)
{
	int	i;
	int	sum;
	int	sign;

	i = 0;
	sum = 0;
	sign = 1;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}	
	while (nptr[i] >= 48 && nptr[i] <= 57)
		sum = (sum * 10) + (nptr[i++] - 48);
	return (sign * sum);
}

t_philo	*set_simulation_args(t_sim *s, char **args)
{
	t_philo *philos;

	s->num_philo = ft_atoi(args[1]);
	s->ttd = ft_atoi(args[2]);
	s->tte = ft_atoi(args[3]);
	s->tts = ft_atoi(args[4]);
	s->num_eat = -1;
	if (args[5])
		s->num_eat = ft_atoi(args[5]);
	s->mutexes = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * s->num_philo);
	s->forks = (int *)malloc(sizeof(int) * s->num_philo);
	s->fork_users = (int *)malloc(sizeof(int) * s->num_philo);
	philos = (t_philo *)malloc(sizeof(t_philo) * s->num_philo);
	
	return (philos);
}

void	init_args(t_sim *s, t_philo *p)
{
	int	i;

	i = -1;
	while (++i < s->num_philo)
	{
		pthread_mutex_init(&s->mutexes[i], NULL);
		s->forks[i] = 1;
		s->fork_users[i] = -1;
		p[i].s = s;
		p[i].id = i + 1;
		p[i].num_meals = 0;
		p[i].sleep_flag = 1;
		p[i].think_flag = 0;
	}
	pthread_mutex_init(&s->end_mutex, NULL);
	pthread_mutex_init(&s->print_mutex, NULL);
	pthread_mutex_init(&s->meal_count_mutex, NULL);
	s->end = 0;
	s->count_completed = 0;
}

void	clean_up(t_sim *s, t_philo *p)
{
	int	i;

	i = -1;
	while (++i < s->num_philo)
		pthread_mutex_destroy(&s->mutexes[i]);
	pthread_mutex_destroy(&s->end_mutex);
	pthread_mutex_destroy(&s->print_mutex);
	pthread_mutex_destroy(&s->meal_count_mutex);
	free(p);
	free(s->mutexes);
	free(s->forks);
	free(s->fork_users);
}