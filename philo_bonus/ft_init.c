/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 11:53:50 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/30 15:25:03 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static int	validate_table(t_table *table)
{
	if (table->max_eats <= 0 || table->number_of_philosophers < 1)
		return (1);
	if (table->time_to_die < 1 || table->time_to_eat < 1
		|| table->time_to_sleep < 1)
	{
		return (1);
	}
	return (0);
}

t_table	*ft_init_table(int argc, char **argv)
{
	t_table	*table;

	table = malloc(sizeof(t_table));
	if (!table)
		return (NULL);
	table->number_of_philosophers = atol(argv[1]);
	table->time_to_die = atol(argv[2]);
	table->time_to_eat = atol(argv[3]);
	table->time_to_sleep = atol(argv[4]);
	table->max_eats = 0;
	if (argc == 6)
		table->max_eats = atol(argv[5]);
	if (validate_table(table))
	{
		free(table);
		return (NULL);
	}
	table->start_time = ft_get_time_ms();
	return (table);
}

static void	ft_init_data(t_table *table, t_philo_data *data)
{
	data->time_to_die = table->time_to_die;
	data->time_to_eat = table->time_to_eat;
	data->time_to_sleep = table->time_to_sleep;
	data->max_eats = table->max_eats;
	data->start_time = table->start_time;
	data->number_of_philosophers = table->number_of_philosophers;
}

int	ft_init_philos(t_table *table)
{
	int				i;
	pid_t			pid;
	t_philo_data	data;

	table->pids = malloc(sizeof(pid_t) * table->number_of_philosophers);
	if (!table->pids)
		return (1);
	ft_init_data(table, &data);
	i = 0;
	while (i < table->number_of_philosophers)
	{
		data.id = i + 1;
		pid = fork();
		if (pid == 0)
		{
			ft_philosopher_routine(data);
			exit(1);
		}
		else if (pid > 0)
			table->pids[i] = pid;
		else
			return (1);
		i++;
	}
	return (0);
}
