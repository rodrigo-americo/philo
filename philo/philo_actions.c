/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 11:29:26 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/15 11:29:16 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_philo_sleep(t_philo *philo)
{
	ft_print_status(philo, "is sleeping");
	sleep_precise(philo->table->time_to_sleep, philo->table);
}

void	ft_philo_think(t_philo *philo)
{
	long long	time_to_think;

	ft_print_status(philo, "is thinking");
	if (philo->table->number_of_philosophers % 2 != 0)
	{
		time_to_think = (philo->table->time_to_eat * 2) - philo->table->time_to_sleep;
		if (time_to_think < 0)
			time_to_think = 0;
		sleep_precise(time_to_think, philo->table);
	}
}
