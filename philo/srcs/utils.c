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

void	set_simulation_args(t_sim *s, char **args)
{
	s->num_philo = ft_atoi(args[1]);
	s->ttd = ft_atoi(args[2]);
	s->tte = ft_atoi(args[3]);
	s->tts = ft_atoi(args[4]);
	s->num_eat = 0;
	if (args[5])
		s->num_eat = ft_atoi(args[5]);
}