/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 10:51:26 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/17 11:23:03 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	ft_cleanup(t_table *table)
{
	if (table->pids)
		free(table->pids);
	if (table->forks_sem)
		sem_close(table->forks_sem);
	if (table->print_sem)
		sem_close(table->print_sem);
	if (table->seats_sem)
		sem_close(table->seats_sem);
	ft_unlink_all_sems();
	free(table);
}

int	ft_erro(char *msg)
{
	int	i;

	i = 0;
	while (msg[i])
		i++;
	write(2, msg, i);
	return (1);
}

int	main(int argc, char **argv)
{
	t_table	*table;

	if (argc < 5 || argc > 6)
		return (ft_erro("Error.\n"));
	table = ft_init_table(argc, argv);
	if (!table)
		return (ft_erro("Error.\n"));
	if (ft_init_semaphores(table))
	{
		free(table);
		return (ft_erro("Error\n"));
	}
	if (ft_init_philos(table) != 0)
	{
		ft_cleanup(table);
		return (ft_erro("Error.\n"));
	}
	ft_monitor_philosophers(table);
	ft_cleanup(table);
	return (0);
}
