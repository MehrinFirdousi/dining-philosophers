/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 14:21:57 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/26 00:47:30 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @todo
 *	find solutions for fixing deadlock
 * 	use a mutex to protect when a philo dies so that another philo doesnt start eating at the same time
 *	remove forks from the array maybe
 *	how to kill other threads - kill all if one dies?
 *  release all mutexes when exiting a thread
 *  find a way to check forks values without data race - pick up fork only if both are available
 *	if num philos in a rotation is odd, prioritize last philo over first philo and vice versa over each rotation 
 * 
 * 
 */

#include "philo.h"

// returns current time in ms
size_t	print_timestamp(t_philo *p, char *msg, char *color)
{
	struct timeval	time;
	size_t			curr_time;

	gettimeofday(&time, NULL);
	curr_time = (time.tv_sec) * 1000 + (time.tv_usec) / 1000;
	pthread_mutex_lock(&p->s->print_mutex);
	printf("%s%ldms\t%d %s\n\033[0m", color, curr_time - p->s->start_time, p->id, msg);
	pthread_mutex_unlock(&p->s->print_mutex);
	return (curr_time);
}

// to check if a philo in another thread died
int	check_death_philo(t_sim *s)
{
	int exit;

	exit = 0;
	pthread_mutex_lock(&s->end_mutex);
	if (s->end)
		exit = 1;
	pthread_mutex_unlock(&s->end_mutex);
	return (exit);
}

// 
int	check_ttd(t_philo *p)
{
	struct timeval	time;
	size_t			curr_time;
	size_t			last_meal;

	gettimeofday(&time, NULL);
	curr_time = (time.tv_sec) * 1000 + (time.tv_usec) / 1000;
	last_meal = p->last_meal_time;
	if (last_meal == 0)
		last_meal = p->s->start_time;
	if (curr_time - last_meal >= p->s->ttd)
	{
		if (check_death_philo(p->s))
			return (0);
		pthread_mutex_lock(&p->s->end_mutex);
		p->s->end = 1;
		pthread_mutex_unlock(&p->s->end_mutex);
		pthread_mutex_lock(&p->s->print_mutex);
		printf("\033[0;31m%ldms\t%d has died\n\033[0m", curr_time - p->s->start_time, p->id);		
		pthread_mutex_unlock(&p->s->print_mutex);
		return (0); // dead
	}
	// pthread_mutex_lock(&p->s->print_mutex);
	// printf("\033[0;35mno death %d %ld\n\033[0m", p->id, curr_time - last_meal);
	// pthread_mutex_unlock(&p->s->print_mutex);
	
	return (1);
}

// int	check_forks(t_sim *s, int i)
// {
// 	int	free;

// 	free = 0;
// 	pthread_mutex_lock(&s->read_mutex);
// 	if (s->forks[i] && s->forks[(i + 1) % s->num_philo])
// 		free = 1;
// 	pthread_mutex_unlock(&s->read_mutex);
// 	return (free);
// }

int	check_num_meals(t_philo *p)
{
	if (p->s->num_eat > 0)
		if (p->num_meals == p->s->num_eat)
			return (0); // reached meal limit
	return (1);
}

void	*run_lifecycle(void	*ptr)
{
	t_sim	*s;
	t_philo	*p;
	int		i;
	int		flip;
	int		think;
	char	temp[500];
	int		fork1;
	int		fork2;
	
	p = (t_philo *)ptr;
	s = p->s;
	i = p->id - 1;
	flip = 0;
	think = 0;
	if (s->num_philo == 1)
		usleep(s->ttd * 1000);
	while (check_num_meals(p) && check_ttd(p) && !check_death_philo(s))
	{
		if ((i + flip) & 1)
		{
			// printf("flip = %d, i = %d\n", flip, i);
			flip = !flip; // so it doesnt sleep in the next iter
			think = 0;
			print_timestamp(p, "is sleeping", BLUE);
			usleep(s->tts * 1000);
		}
		// else if (s->forks[i] && s->forks[(i + 1) % s->num_philo])
		else
		{
			fork1 = i;
			fork2 = (i + 1) % s->num_philo;
			if (i & 1)
			{
				fork1 = (i + 1) % s->num_philo;
				fork2 = i;
			}
			// setting forks as not free if both are available
			pthread_mutex_lock(&s->mutexes[fork1]);
			pthread_mutex_lock(&s->mutexes[fork2]);
			if (s->forks[fork1] && s->forks[fork2])
			{
				if (check_death_philo(s) || !check_ttd(p))
				{
					pthread_mutex_unlock(&s->mutexes[fork2]);
					pthread_mutex_unlock(&s->mutexes[fork1]);
					break;
				}
				s->forks[fork1] = 0;
				sprintf(temp, "has taken fork %d", fork1 + 1);
				print_timestamp(p, temp, WHITE);
				s->forks[fork2] = 0;
				sprintf(temp, "has taken fork %d", fork2 + 1);
				print_timestamp(p, temp, WHITE);
				pthread_mutex_unlock(&s->mutexes[fork2]);
				pthread_mutex_unlock(&s->mutexes[fork1]);
				
				if (check_death_philo(s) || !check_ttd(p))
					break;
				p->last_meal_time = print_timestamp(p, "is eating", GREEN);
				usleep(s->tte * 1000);
				flip = !flip;
				think = 0;
				p->num_meals++;
				
				// setting forks as free
				pthread_mutex_lock(&s->mutexes[fork1]);
				pthread_mutex_lock(&s->mutexes[fork2]);
				if (check_death_philo(s) || !check_ttd(p))
				{
					pthread_mutex_unlock(&s->mutexes[fork2]);
					pthread_mutex_unlock(&s->mutexes[fork1]);
					break;
				}
				s->forks[fork1] = 1;
				s->forks[fork2] = 1;
			}
			pthread_mutex_unlock(&s->mutexes[fork2]);
			pthread_mutex_unlock(&s->mutexes[fork1]);
		}
		if (!think)
		{
			if (check_death_philo(s))
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
	s->num_meals = (int *)malloc(sizeof(int) * s->num_philo);
	
	// init mutexes for all forks, set initial state for all philos
	i = -1;
	while (++i < s->num_philo)
	{
		pthread_mutex_init(&s->mutexes[i], NULL);
		s->forks[i] = 1;
		philos[i].s = s;
		philos[i].id = i + 1;
		philos[i].last_meal_time = 0;
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
		pthread_create(&philos_th[i], NULL, run_lifecycle, (void *)&philos[i]);
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
	// pthread_mutex_destroy(&s->read_mutex);
	
	free(philos);
	free(philos_th);
	free(s->mutexes);
	free(s->forks);
	free(s->num_meals);
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
