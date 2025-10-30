/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 11:05:23 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/17 11:23:39 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	ft_unlink_all_sems(void)
{
	sem_unlink(SEM_FORKS_NAME);
	sem_unlink(SEM_PRINT_NAME);
	sem_unlink(SEM_SEATS_NAME);
}

int	ft_init_semaphores(t_table *table)
{
	ft_unlink_all_sems();
	table->forks_sem = sem_open(SEM_FORKS_NAME, O_CREAT | O_EXCL,
			0644, table->number_of_philosophers);
	if (table->forks_sem == SEM_FAILED)
		return (1);
	table->print_sem = sem_open(SEM_PRINT_NAME, O_CREAT | O_EXCL,
			0644, 1);
	if (table->print_sem == SEM_FAILED)
		return (1);
	table->seats_sem = sem_open(SEM_SEATS_NAME, O_CREAT | O_EXCL,
			0644, table->number_of_philosophers - 1);
	if (table->seats_sem == SEM_FAILED)
		return (1);
	return (0);
}

int	ft_open_semaphores(t_philo_state *state)
{
	state->forks = sem_open(SEM_FORKS_NAME, 0);
	state->print = sem_open(SEM_PRINT_NAME, 0);
	state->seats = sem_open(SEM_SEATS_NAME, 0);
	if (state->forks == SEM_FAILED)
		return (1);
	if (state->print == SEM_FAILED)
		return (1);
	if (state->seats == SEM_FAILED)
		return (1);
	return (0);
}
