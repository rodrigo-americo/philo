/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_clear.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 14:20:03 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/13 10:57:50 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_destroy_forks(t_table *table, int qtd)
{
	int	i;

	i = 0;
	while (i < qtd - 1)
	{
		pthread_mutex_destroy(&table->forks[i]);
		i++;
	}
	free(table->forks);
	table->forks = NULL;
}

void	ft_destroy_table(t_table *table)
{
	if (table)
	{
		pthread_mutex_destroy(&table->data_mutex);
		if (table->philos)
			free(table->philos);
		if (table->forks)
			ft_destroy_forks(table, table->number_of_philosophers);
		free(table);
	}
}
