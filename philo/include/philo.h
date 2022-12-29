/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 14:22:44 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/29 19:57:19 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

# define YELLOW	"\033[0;33m"
# define WHITE	"\033[0;37m"

typedef pthread_mutex_t	t_mutex;
typedef struct s_sim
{
	int		p_count;
	int		tte;
	int		tts;
	int		ttd;
	int		num_eat;
	size_t	start_time;
	int		*forks;
	int		*fork_users;
	int		count_completed;
	t_mutex	*mutexes;
	t_mutex	end_mutex;
	t_mutex	meal_count_mutex;
	t_mutex	print_mutex;
	int		end;
}			t_sim;

typedef struct s_philo
{
	t_sim	*s;
	int		id;
	size_t	last_meal_time;
	int		num_meals;
	int		sleep_flag;
	int		think_flag;
}				t_philo;

// parsing, set up and clean up
int		ft_atoi(const char *nptr);
t_philo	*set_simulation_args(t_sim *s, char **args);
void	init_args(t_sim *s, t_philo *p);
void	clean_up(t_sim *s, t_philo *p);

// main
void	*run_lifecycle(void	*philo);
void	start_simulation(t_sim *s, t_philo *philos);
void	print_timestamp(t_philo *p, char *msg, char *color);
size_t	get_time(void);

// lifecycle functions
int		pick_up_forks(t_sim *s, t_philo *p, int fork1, int fork2);
int		release_forks(t_sim *s, t_philo *p, int fork1, int fork2);
int		try_to_eat(t_sim *s, t_philo *p);
int		philo_eat(t_philo *p);
int		philo_sleep(t_philo *p);

// simulation monitoring 
int		check_death(t_philo *p);
int		is_turn(t_philo *p, int fork1, int fork2);
void	update_meal_count(t_philo *p);
int		check_num_meals(t_philo *p);
void	check_one_philo(t_sim *s);

#endif