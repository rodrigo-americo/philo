/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 15:20:27 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/15 11:28:45 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_set_dead_and_log(t_philo *philo)
{
	long long	time_ms;

	pthread_mutex_lock(&philo->table->data_mutex);
	if (philo->table->is_dead || philo->table->stop)
	{
		pthread_mutex_unlock(&philo->table->data_mutex);
		return ;
	}
	philo->table->is_dead = 1;
	pthread_mutex_lock(&philo->table->print_mutex);
	time_ms = ft_get_time_ms() - philo->table->start_time;
	printf("%lld %d %s\n", time_ms, philo->id, "died");
	pthread_mutex_unlock(&philo->table->print_mutex);
	pthread_mutex_unlock(&philo->table->data_mutex);
}

static void	ft_all_sats(t_table *table)
{
	int	i;
	int	philos_satisfied;

	i = 0;
	philos_satisfied = 0;
	pthread_mutex_lock(&table->data_mutex);
	while (i < table->number_of_philosophers)
	{
		if (table->philos[i].eats_count >= table->max_eats)
			philos_satisfied++;
		i++;
	}
	pthread_mutex_unlock(&table->data_mutex);
	if (table->max_eats > 0
		&& philos_satisfied == table->number_of_philosophers)
	{
		pthread_mutex_lock(&table->data_mutex);
		table->stop = 1;
		pthread_mutex_unlock(&table->data_mutex);
	}
}

static void	ft_check_death(t_table *table)
{
	int			i;
	long long	time_to_die;

	i = 0;
	pthread_mutex_lock(&table->data_mutex);
	while (i < table->number_of_philosophers)
	{
		time_to_die = table->time_to_die;
		if (ft_get_time_ms() - table->philos[i].time_last_meal >= time_to_die)
		{
			pthread_mutex_unlock(&table->data_mutex);
			ft_set_dead_and_log(&table->philos[i]);
			return ;
		}
		i++;
	}
	pthread_mutex_unlock(&table->data_mutex);
}

int	ft_is_simulation_over(t_table *table)
{
	pthread_mutex_lock(&table->data_mutex);
	if (table->is_dead || table->stop)
	{
		pthread_mutex_unlock(&table->data_mutex);
		return (1);
	}
	pthread_mutex_unlock(&table->data_mutex);
	return (0);
}

void	philo_monitor(t_table *table)
{
	while (1)
	{
		if (ft_is_simulation_over(table))
			break ;
		ft_check_death(table);
		ft_all_sats(table);
		usleep(5000);
	}
}
