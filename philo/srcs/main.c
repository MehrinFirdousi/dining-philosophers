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

void	*run_lifecycle(void	*philo)
{
	t_sim	*s;
	t_philo	*p;
	
	p = (t_philo *)philo;
	s = p->s;
	if (s->num_philo == 1)
		usleep(s->ttd * 1000);
	while (check_num_meals(p) && check_death(p))
	{
		if ((p->id + p->sleep_flag) & 1)
		{
			p->sleep_flag = !p->sleep_flag; // so it doesnt sleep in the next iter
			p->think_flag = 1;
			sleep_in_inc(p, s->tts, "is sleeping", BLUE, 0);
			// print_timestamp(p, "is sleeping", BLUE);
			// usleep(s->tts * 1000);
		}
		else if (!try_to_eat(s, p))
			break;
		if (p->think_flag)
		{
			if (!check_death(p))
				break;
			p->think_flag = 0;
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
