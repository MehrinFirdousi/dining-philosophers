/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 14:21:57 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/26 21:45:36 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @todo
 *	
 * dying while eating - if someone else dies while someone is eating
 * usleep delay
 * norm
 * parsing - negative nums
 * prints after dying in the same second
 * store id in s->end and print dying in main thread
 */

#include "philo.h"

// returns current time in ms
size_t	print_timestamp(t_philo *p, char *msg, char *color)
{	
	struct timeval	time;
	size_t			curr_time;

	pthread_mutex_lock(&p->s->print_mutex);
	gettimeofday(&time, NULL);
	curr_time = (time.tv_sec) * 1000 + (time.tv_usec) / 1000;
	printf("%s%ldms\t%d %s\n\033[0m", color, curr_time - p->s->start_time, p->id, msg);
	pthread_mutex_unlock(&p->s->print_mutex);
	return (curr_time);
}

// returns 0 if someone died 
int	check_ttd(t_philo *p)
{
	struct timeval	time;
	size_t			curr_time;

	gettimeofday(&time, NULL);
	curr_time = (time.tv_sec) * 1000 + (time.tv_usec) / 1000;
	pthread_mutex_lock(&p->s->end_mutex);
	// printf("end=%d\n", p->s->end);
	if (p->s->end) // another philo died
	{
		pthread_mutex_unlock(&p->s->end_mutex);
		return (0);
	}
	if (curr_time - p->last_meal_time >= p->s->ttd) // current philo died
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

// sleeps in increments of 100 microseconds each time
int	sleep_in_inc(t_philo *p, int time_to_sleep)
{
	int	time_slept;

	time_slept = 0;
	time_to_sleep *= 1000; // converting milliseconds to microseconds
	while (time_slept < time_to_sleep)
	{
		usleep(100);
		time_slept += 100;
		if (check_ttd(p) == 0)
			return (0);
	}
	// printf("slept for %d microseconds\n", time_slept);
	return (1);
}

/*void	eat(t_philo *p, t_sim *s, int i)
{
	int		fork1;
	int		fork2;
	
	fork1 = i;
	fork2 = (i + 1) % s->num_philo;
	if (i & 1)
	{
		fork1 = (i + 1) % s->num_philo;
		fork2 = i;
	}
	pthread_mutex_lock(&s->mutexes[fork1]);
	pthread_mutex_lock(&s->mutexes[fork2]);
	if (s->forks[fork1] && s->forks[fork2] && is_turn(p, fork1, fork2))
	{
		if (!check_ttd(p))
		{
			pthread_mutex_unlock(&s->mutexes[fork2]);
			pthread_mutex_unlock(&s->mutexes[fork1]);
			break;
		}
		s->forks[fork1] = 0;
		print_timestamp(p, "has taken a fork", WHITE);
		s->forks[fork2] = 0;
		print_timestamp(p, "has taken a fork", WHITE);
		pthread_mutex_unlock(&s->mutexes[fork2]);
		pthread_mutex_unlock(&s->mutexes[fork1]);
		
		if (!check_ttd(p))
			break;
		p->last_meal_time = print_timestamp(p, "is eating", GREEN);
		usleep(s->tte * 1000);
		flip = !flip;
		think = 0;
		p->num_meals++;
		
		// setting forks as free
		pthread_mutex_lock(&s->mutexes[fork1]);
		pthread_mutex_lock(&s->mutexes[fork2]);
		if (!check_ttd(p))
		{
			pthread_mutex_unlock(&s->mutexes[fork2]);
			pthread_mutex_unlock(&s->mutexes[fork1]);
			break;
		}
		s->fork_users[fork1] = p->id;
		s->fork_users[fork2] = p->id;
		s->forks[fork1] = 1;
		s->forks[fork2] = 1;
	}
	pthread_mutex_unlock(&s->mutexes[fork2]);
	pthread_mutex_unlock(&s->mutexes[fork1]);
}*/

void	*run_lifecycle(void	*ptr)
{
	t_sim	*s;
	t_philo	*p;
	int		i;
	int		flip;
	int		think;
	// char	temp[500];
	int		fork1;
	int		fork2;
	
	p = (t_philo *)ptr;
	s = p->s;
	i = p->id - 1;
	flip = 0;
	think = 0;
	if (s->num_philo == 1)
		usleep(s->ttd * 1000);
	if (print_timestamp(p, "hihi", WHITE))
	while ((p->num_meals != p->s->num_eat) && check_ttd(p))
	{
		if ((i + flip) & 1)
		{
			flip = !flip; // so it doesnt sleep in the next iter
			think = 0;
			print_timestamp(p, "is sleeping", BLUE);
			usleep(s->tts * 1000);
			// sleep_in_inc(p, s->tts);
		}
		else
		{
			fork1 = i;
			fork2 = (i + 1) % s->num_philo;
			if (i & 1)
			{
				fork1 = (i + 1) % s->num_philo;
				fork2 = i;
			}
			pthread_mutex_lock(&s->mutexes[fork1]);
			pthread_mutex_lock(&s->mutexes[fork2]);
			if (s->forks[fork1] && s->forks[fork2] && is_turn(p, fork1, fork2))
			{
				if (!check_ttd(p))
				{
					pthread_mutex_unlock(&s->mutexes[fork2]);
					pthread_mutex_unlock(&s->mutexes[fork1]);
					break;
				}
				s->forks[fork1] = 0;
				print_timestamp(p, "has taken a fork", WHITE);
				s->forks[fork2] = 0;
				print_timestamp(p, "has taken a fork", WHITE);
				pthread_mutex_unlock(&s->mutexes[fork2]);
				pthread_mutex_unlock(&s->mutexes[fork1]);
				
				if (!check_ttd(p))
					break;
				// p->last_meal_time = print_timestamp(p, "is eating", GREEN);
				// usleep(s->tte * 1000);
				if (sleep_in_inc(p, s->tte) == 0)
					break;
				flip = !flip;
				think = 0;
				p->num_meals++;
				
				// setting forks as free
				pthread_mutex_lock(&s->mutexes[fork1]);
				pthread_mutex_lock(&s->mutexes[fork2]);
				if (!check_ttd(p))
				{
					pthread_mutex_unlock(&s->mutexes[fork2]);
					pthread_mutex_unlock(&s->mutexes[fork1]);
					break;
				}
				s->fork_users[fork1] = p->id;
				s->fork_users[fork2] = p->id;
				s->forks[fork1] = 1;
				s->forks[fork2] = 1;
			}
			pthread_mutex_unlock(&s->mutexes[fork2]);
			pthread_mutex_unlock(&s->mutexes[fork1]);
		}
		if (!think)
		{
			if (!check_ttd(p))
				break;
			think = 1;
			print_timestamp(p, "is thinking", YELLOW);
		}
	}
	return (NULL);
}

void	testing(t_sim *s)
{
	struct timeval	time;
	pthread_t		*philos_th;
	t_philo			*philos;
	int	i;
	
	philos = (t_philo *)malloc(sizeof(t_philo) * s->num_philo);
	philos_th = (pthread_t *)malloc(sizeof(pthread_t) * s->num_philo);
	s->mutexes = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * s->num_philo);
	s->forks = (int *)malloc(sizeof(int) * s->num_philo);
	s->fork_users = (int *)malloc(sizeof(int) * s->num_philo);

	// init mutexes for all forks, set initial state for all philos
	i = -1;
	while (++i < s->num_philo)
	{
		pthread_mutex_init(&s->mutexes[i], NULL);
		s->forks[i] = 1;
		s->fork_users[i] = -1;
		philos[i].s = s;
		philos[i].id = i + 1;
		philos[i].num_meals = 0;
	}
	
	pthread_mutex_init(&s->end_mutex, NULL);
	pthread_mutex_init(&s->print_mutex, NULL);
	s->end = 0;
	
	// set start time of simulation
	gettimeofday(&time, NULL);
	s->start_time = (time.tv_sec) * 1000 + (time.tv_usec) / 1000;
	// create threads for all philos
	i = -1;
	while (++i < s->num_philo)
	{
		philos[i].last_meal_time = s->start_time;
		pthread_create(&philos_th[i], NULL, run_lifecycle, (void *)&philos[i]);
	}
	// wait for all threads
	i = -1;
	while (++i < s->num_philo)
		pthread_join(philos_th[i], NULL);
	
	// destroying all mutexes
	i = -1;
	while (++i < s->num_philo)
		pthread_mutex_destroy(&s->mutexes[i]);
	pthread_mutex_destroy(&s->end_mutex);
	pthread_mutex_destroy(&s->print_mutex);
	
	free(philos);
	free(philos_th);
	free(s->mutexes);
	free(s->forks);
	free(s->fork_users);
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
	set_simulation_args(&s, argv);
	testing(&s);
	
	return (0);
}
