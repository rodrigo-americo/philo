/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 11:53:50 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/13 10:57:50 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	ft_parse_and_validate_args(t_table *table, int argc, char **argv)
{
	long	val;

	table->number_of_philosophers = (int)ft_atol(argv[1]);
	if (table->number_of_philosophers < 1)
		return (1);
	table->time_to_die = ft_atol(argv[2]);
	if (table->time_to_die <= 0)
		return (1);
	val = ft_atol(argv[3]);
	if (val <= 0)
		return (1);
	table->time_to_eat = val;
	val = ft_atol(argv[4]);
	if (val <= 0)
		return (1);
	table->time_to_sleep = val;
	table->max_eats = 0;
	if (argc == 6)
	{
		val = ft_atol(argv[5]);
		if (val <= 0)
			return (1);
		table->max_eats = (int)val;
	}
	return (0);
}

t_table	*ft_init_table(int argc, char **argv)
{
	t_table	*table;

	table = malloc(sizeof(t_table));
	if (!table)
		return (NULL);
	if (ft_parse_and_validate_args(table, argc, argv) != 0)
	{
		free(table);
		return (NULL);
	}
	table->is_dead = 0;
	table->stop = 0;
	if (pthread_mutex_init(&table->data_mutex, NULL) != 0)
	{
		free(table);
		return (NULL);
	}
	table->start_time = ft_get_time_ms();
	table->forks = NULL;
	table->philos = NULL;
	return (table);
}

int	ft_init_forks(t_table *table)
{
	int	i;
	int	num;

	num = table->number_of_philosophers;
	table->forks = malloc(sizeof(t_fork) * num);
	if (!table->forks)
		return (1);
	i = 0;
	while (i < num)
	{
		if (pthread_mutex_init(&table->forks[i], NULL) != 0)
		{
			ft_destroy_forks(table, i);
			return (1);
		}
		i++;
	}
	return (0);
}

int	ft_init_philos(t_table *table)
{
	int	i;
	int	num;

	num = table->number_of_philosophers;
	table->philos = malloc(sizeof(t_philo) * num);
	if (!table->philos)
		return (1);
	i = 0;
	while (i < num)
	{
		table->philos[i].id = i + 1;
		table->philos[i].eats_count = 0;
		table->philos[i].time_last_meal = table->start_time;
		table->philos[i].table = table;
		table->philos[i].l_fork = &table->forks[i];
		table->philos[i].r_fork = &table->forks[(i + 1) % num];
		i++;
	}
	return (0);
}
