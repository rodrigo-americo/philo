/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:07:34 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/30 15:25:22 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static void	ft_init_state(t_philo_state *state, t_philo_data data)
{
	state->data = data;
	state->last_meal = data.start_time;
	state->eat_count = 0;
	if (ft_open_semaphores(state))
		exit(1);
	if (data.id % 2 == 0)
		usleep(1000);
}

static void	ft_release_forks(t_philo_state *state)
{
	sem_post(state->forks);
	sem_post(state->forks);
	sem_post(state->seats);
}

void	*philo_monitor(void *arg)
{
	t_philo_state	*state;

	state = (t_philo_state *)arg;
	while (1)
	{
		ft_check_death(state);
		ft_check_finish(state);
		usleep(1000);
	}
	return (NULL);
}

void	ft_philosopher_routine(t_philo_data data)
{
	t_philo_state	state;
	pthread_t		monitor_tid;

	ft_init_state(&state, data);
	if (state.data.number_of_philosophers == 1)
	{
		ft_one_philo_eat(&state);
		exit(0);
	}
	if (pthread_create(&monitor_tid, NULL, &philo_monitor, (void *)&state) != 0)
		exit(1);
	pthread_detach(monitor_tid);
	while (1)
	{
		ft_philo_think(&state);
		sem_wait(state.seats);
		ft_philo_eat(&state);
		ft_release_forks(&state);
		ft_philo_sleep(&state);
	}
}
