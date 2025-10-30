/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 10:51:26 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/15 11:51:03 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
	if (ft_init_forks(table) != 0)
	{
		ft_destroy_table(table);
		return (ft_erro("Error: Failed to initialize forks.\n"));
	}
	if (ft_init_philos(table) != 0)
	{
		ft_destroy_table(table);
		return (ft_erro("Error: Failed to initialize philosophers.\n"));
	}
	ft_start_simulation(table);
	ft_destroy_table(table);
	return (0);
}
