/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_simulation.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 14:52:01 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/13 10:57:50 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	ft_create_philo_threads(t_table *table)
{
	int	i;
	int	num;

	i = 0;
	num = table->number_of_philosophers;
	while (i < num)
	{
		if (pthread_create(&table->philos[i].thread_id,
				NULL, philo_routine, &table->philos[i]) != 0)
		{
			return (1);
		}
		i++;
	}
	return (0);
}

static void	ft_join_philo_threads(t_table *table)
{
	int	i;
	int	num;

	i = 0;
	num = table->number_of_philosophers;
	while (i < num)
	{
		pthread_join(table->philos[i].thread_id, NULL);
		i++;
	}
}

void	ft_start_simulation(t_table *table)
{
	if (ft_create_philo_threads(table) != 0)
		return ;
	philo_monitor(table);
	ft_join_philo_threads(table);
}
