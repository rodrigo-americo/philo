/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 11:06:56 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/17 14:47:25 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	ft_philo_think(t_philo_state *state)
{
	long long	sleep_start;
	long long	elapsed;

	ft_check_death(state);
	ft_print_status(state, "is thinking");
	sleep_start = ft_get_time_ms();
	if (state->data.number_of_philosophers % 2 != 0)
	{
		while (1)
		{
			elapsed = ft_get_time_ms() - sleep_start;
			if (elapsed >= state->data.time_to_sleep)
				break ;
			ft_check_death(state);
			usleep(200);
		}
	}
}

void	ft_philo_sleep(t_philo_state *state)
{
	long long	sleep_start;
	long long	elapsed;

	ft_check_death(state);
	ft_print_status(state, "is sleeping");
	sleep_start = ft_get_time_ms();
	while (1)
	{
		elapsed = ft_get_time_ms() - sleep_start;
		if (elapsed >= state->data.time_to_sleep)
			break ;
		ft_check_death(state);
		usleep(200);
	}
}

void	ft_take_fork(t_philo_state *state)
{
	sem_wait(state->forks);
	ft_check_death(state);
	ft_print_status(state, "has taken a fork");
}

static void	ft_eat_with_checks(t_philo_state *state)
{
	long long	eat_start;
	long long	elapsed;

	eat_start = ft_get_time_ms();
	while (1)
	{
		elapsed = ft_get_time_ms() - eat_start;
		if (elapsed >= state->data.time_to_eat)
			break ;
		ft_check_death(state);
		usleep(1000);
	}
}

void	ft_philo_eat(t_philo_state *state)
{
	ft_take_fork(state);
	ft_take_fork(state);
	state->last_meal = ft_get_time_ms();
	ft_print_status(state, "is eating");
	ft_eat_with_checks(state);
	state->eat_count++;
}
