/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 11:07:58 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/17 15:35:21 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	ft_check_death(t_philo_state *state)
{
	long long	current;
	long long	elapsed;

	current = ft_get_time_ms();
	elapsed = current - state->last_meal;
	if (elapsed >= state->data.time_to_die)
	{
		sem_wait(state->print);
		printf("%lld %d died\n",
			current - state->data.start_time, state->data.id);
		exit(1);
	}
}

void	ft_check_finish(t_philo_state *state)
{
	if (state->data.max_eats > 0)
	{
		if (state->eat_count >= state->data.max_eats)
			exit(0);
	}
}
