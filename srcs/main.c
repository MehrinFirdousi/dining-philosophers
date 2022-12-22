/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 14:21:57 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/22 22:04:40 by mfirdous         ###   ########.fr       */
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

void	set_simulation_args(t_sim *s, char **args)
{
	int	i;
	
	s->num_philo = ft_atoi(args[0]);
	s->ttd = ft_atoi(args[1]);
	s->tte = ft_atoi(args[2]);
	s->tts = ft_atoi(args[3]);
	if (args[4])
		s->num_eat = ft_atoi(args[4]);
	s->p = (int *)malloc(sizeof(int) * s->num_philo);
	s->f = (int *)malloc(sizeof(int) * s->num_philo);
	i = -1;
	while (++i < s->num_philo)
	{
		s->p[i] = 1;
		s->f[i] = 1;
	}
}

void	*compute(void	*ptr)
{
	int	*a;
	int	sum;
	
	// acquiring the lock
	// pthread_mutex_lock(&mutex);
	
	a = (int *)ptr;
	sum = 0;
	for (int i = 0; i < 1000000000; i++)
		sum += *a;

	// releasing the lock 
	// pthread_mutex_unlock(&mutex);
	
	return (NULL);
}

void	testing(t_sim *s)
{
	(void)s;
	pthread_t	p1;			// v
	pthread_t	p2;			// v
	pthread_mutex_t	mutex;  // has to be an array but where?
	int	i;
	int	a;
	int	b;

	a = 1;
	b = 2;
	i = -1;
	
	// init mutexes for all forks
	while (++i < s->num_philo)
		pthread_mutex_init(&mutex, NULL);
	// create threads for all philos
	while (++i < s->num_philo)
		pthread_create(&p1, NULL, compute, (void *)&a);
	// wait for all threads
	while (++i < s->num_philo)
		pthread_join(p1, NULL);
	// destroying all mutexes
	while (++i < s->num_philo)
		pthread_mutex_destroy(&mutex);

}

int	main(int argc, char **argv)
{
	t_sim	s;

	if (argc < 5 || argc > 6)
	{
		printf("Usage: ./philo <number_of_philosophers> <time_to_die> \
				<time_to_eat> <time_to_sleep> \
				[number_of_times_each_philosopher_must_eat]");
		return (1);
	}
	set_simulation_args(&s, argv + 1);
	testing(&s);
	return (0);
}
