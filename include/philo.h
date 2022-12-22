/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/22 14:22:44 by mfirdous          #+#    #+#             */
/*   Updated: 2022/12/22 21:53:30 by mfirdous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>

typedef struct s_sim
{
	int	num_philo;
	int	tte;
	int	tts;
	int	ttd;
	int	num_eat;
	int	*p; // array of philosophers, len=num_philo, each element is a philo and the value is the philo's state
			// 1 - thinking, 2 - eating, 3 - dead
	int	*f; // array of forks, len=num_philo, each element is a fork and the value is 0 or 1 -> free or being used
			// 0 - not free, 1 - free
}			t_sim;

void	get_simulation_args(t_sim *s, char **args);

#endif