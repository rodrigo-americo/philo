/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 12:07:34 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/15 11:28:27 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	sleep_precise(long long ms, t_table *table)
{
	long long	start;

	start = ft_get_time_ms();
	while (ft_get_time_ms() - start < ms)
	{
		if (ft_is_simulation_over(table))
			return (1);
		usleep(200);
	}
	return (0);
}

void	ft_print_status(t_philo *philo, const char *status)
{
	long long	time_ms;

	pthread_mutex_lock(&philo->table->data_mutex);
	if (philo->table->is_dead || philo->table->stop)
	{
		pthread_mutex_unlock(&philo->table->data_mutex);
		return ;
	}
	pthread_mutex_lock(&philo->table->print_mutex);
	time_ms = ft_get_time_ms() - philo->table->start_time;
	printf("%lld %d %s\n", time_ms, philo->id, status);
	pthread_mutex_unlock(&philo->table->print_mutex);
	pthread_mutex_unlock(&philo->table->data_mutex);
}

static void	one_philo(t_philo *philo)
{
	ft_print_status(&philo->table->philos[0], "has taken a fork");
	usleep(philo->table->time_to_die * 1000);
	ft_set_dead_and_log(&philo->table->philos[0]);
}

void	*philo_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->table->number_of_philosophers == 1)
	{
		one_philo(philo);
		return (NULL);
	}
	if (philo->id % 2 == 0)
		usleep(philo->table->time_to_eat);
	while (1)
	{
		if (ft_is_simulation_over(philo->table) == 1)
			break ;
		ft_philo_eat(philo);
		if (ft_is_simulation_over(philo->table) == 1)
			break ;
		ft_philo_sleep(philo);
		if (ft_is_simulation_over(philo->table) == 1)
			break ;
		ft_philo_think(philo);
	}
	return (NULL);
}
