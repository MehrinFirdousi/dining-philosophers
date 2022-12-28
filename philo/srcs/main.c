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
size_t	get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec) * 1000 + (time.tv_usec) / 1000);
}

void	print_timestamp(t_philo *p, char *msg, char *color)
{
	pthread_mutex_lock(&p->s->print_mutex);
	printf("%s%ldms\t%d %s\n\033[0m", color, get_time() - p->s->start_time, p->id, msg);
	pthread_mutex_unlock(&p->s->print_mutex);
}

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
		usleep(100);
		if (check_death(p) == 0)
			return (0);
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

void	*run_lifecycle(void	*philo)
{
	t_sim	*s;
	t_philo	*p;
	int		i;
	int		flip;
	int		think;
	int		fork1;
	int		fork2;
	
	p = (t_philo *)philo;
	s = p->s;
	i = p->id - 1;
	flip = 0;
	think = 0;
	if (s->num_philo == 1)
		usleep(s->ttd * 1000);
	// while ((p->num_meals != p->s->num_eat) && check_ttd(p))
	while (check_num_meals(p) && check_death(p))
	{
		if ((i + flip) & 1)
		{
			flip = !flip; // so it doesnt sleep in the next iter
			think = 0;
			// print_timestamp(p, "is sleeping", BLUE);
			// usleep(s->tts * 1000);
			sleep_in_inc(p, s->tts, "is sleeping", BLUE, 0);
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
				if (!check_death(p))
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
				
				if (!check_death(p))
					break;
				// p->last_meal_time = print_timestamp(p, "is eating", GREEN);
				// usleep(s->tte * 1000);
				if (sleep_in_inc(p, s->tte, "is eating", GREEN, 1) == 0)
					break;
				flip = !flip;
				think = 0;
				update_meal_count(p);
			 	// setting forks as free
				pthread_mutex_lock(&s->mutexes[fork1]);
				pthread_mutex_lock(&s->mutexes[fork2]);
				if (!check_death(p))
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
			if (!check_death(p))
				break;
			think = 1;
			print_timestamp(p, "is thinking", YELLOW);
		}
	}
	return (NULL);
}

void	start_simulation(t_sim *s, t_philo *philos)
{
	struct timeval	time;
	pthread_t		*philos_th;
	int				i;

	philos_th = (pthread_t *)malloc(sizeof(pthread_t) * s->num_philo);
	gettimeofday(&time, NULL);
	s->start_time = (time.tv_sec) * 1000 + (time.tv_usec) / 1000;
	i = -1;
	while (++i < s->num_philo)
		philos[i].last_meal_time = s->start_time;
	i = -1;
	while (++i < s->num_philo)
		pthread_create(&philos_th[i], NULL, run_lifecycle, (void *)&philos[i]);
	i = -1;
	while (++i < s->num_philo)
		pthread_join(philos_th[i], NULL);
	clean_up(s, philos);
	free(philos_th);
}

int	main(int argc, char **argv)
{
	t_sim	sim;
	t_philo	*philos;

	if (argc < 5 || argc > 6)
	{
		printf("Usage:\t./philo <number_of_philosophers>\n");
		printf("\t\t<time_to_die>\n");
		printf("\t\t<time_to_eat>\n");
		printf("\t\t<time_to_sleep>\n");
		printf("\t\t<number_of_times_each_philosopher_must_eat>\n");
		return (1);
	}
	philos = set_simulation_args(&sim, argv);
	init_args(&sim, philos);
	start_simulation(&sim, philos);
	return (0);
}
