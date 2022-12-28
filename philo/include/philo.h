/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 14:22:44 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/28 20:25:20 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

# define RED	"\033[0;31m"	// dead
# define GREEN	"\033[0;32m"	// eating
# define BLUE	"\033[0;34m"	// sleeping
# define YELLOW	"\033[0;33m"	// thinking
# define WHITE	"\033[0;37m"
# define RESET	"\033[0m"

typedef struct s_sim
{
	int				num_philo;
	size_t			tte;
	size_t			tts;
	size_t			ttd;
	int				num_eat;
	size_t			start_time;
	int				*forks;
	int				*fork_users;
	int				count_completed; // counter for num_eat 
	pthread_mutex_t	*mutexes;
	pthread_mutex_t end_mutex;
	pthread_mutex_t meal_count_mutex;
	pthread_mutex_t print_mutex;
	int				end;
}			t_sim;

typedef struct s_philo
{
	t_sim	*s;
	int		id;
	size_t	last_meal_time;
	int		num_meals;
	// pthread_mutex_t end_mutex;
	// int		*end; // reference to simulation status
}				t_philo;

t_philo	*set_simulation_args(t_sim *s, char **args);
void	init_args(t_sim *s, t_philo *p);
int		ft_atoi(const char *nptr);
void	clean_up(t_sim *s, t_philo *p);

#endif