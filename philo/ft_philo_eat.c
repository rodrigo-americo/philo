/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_philo_eat.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 10:56:18 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/13 15:52:10 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	ft_choose_forks_by_pos(t_philo *philo,
				pthread_mutex_t **first, pthread_mutex_t **second)
{
	if (philo->r_fork < philo->l_fork)
	{
		*first = philo->r_fork;
		*second = philo->l_fork;
	}
	else
	{
		*first = philo->l_fork;
		*second = philo->r_fork;
	}
}

static void	ft_unlock_both(pthread_mutex_t *first, pthread_mutex_t *second)
{
	pthread_mutex_unlock(second);
	pthread_mutex_unlock(first);
}

void	ft_philo_eat(t_philo *philo)
{
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;

	ft_choose_forks_by_pos(philo, &first, &second);
	pthread_mutex_lock(first);
	ft_print_status(philo, "has taken a fork");
	pthread_mutex_lock(second);
	if (ft_is_simulation_over(philo->table))
	{
		ft_unlock_both(first, second);
		return ;
	}
	ft_print_status(philo, "has taken a fork");
	pthread_mutex_lock(&philo->table->data_mutex);
	philo->time_last_meal = ft_get_time_ms();
	philo->eats_count++;
	pthread_mutex_unlock(&philo->table->data_mutex);
	ft_print_status(philo, "is eating");
	sleep_precise(philo->table->time_to_eat, philo->table);
	ft_unlock_both(first, second);
}
