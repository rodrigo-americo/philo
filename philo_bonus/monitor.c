/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 11:10:07 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/17 15:35:55 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static void	ft_kill_all(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->number_of_philosophers)
	{
		kill(table->pids[i], SIGKILL);
		i++;
	}
}

static int	ft_wait_all_children(t_table *table)
{
	int		i;
	int		status;
	pid_t	pid;

	i = 0;
	while (i < table->number_of_philosophers)
	{
		pid = waitpid(table->pids[i], &status, 0);
		if (pid > 0 && WIFEXITED(status))
		{
			if (WEXITSTATUS(status) == 1)
			{
				ft_kill_all(table);
				return (1);
			}
		}
		i++;
	}
	return (0);
}

void	ft_monitor_philosophers(t_table *table)
{
	int		status;
	int		finished_count;
	pid_t	finished_pid;

	finished_count = 0;
	while (finished_count < table->number_of_philosophers)
	{
		finished_pid = waitpid(-1, &status, 0);
		if (finished_pid > 0 && WIFEXITED(status))
		{
			if (WEXITSTATUS(status) == 1)
			{
				ft_kill_all(table);
				ft_wait_all_children(table);
				return ;
			}
			finished_count++;
		}
		else if (finished_pid < 0)
			break ;
	}
}
