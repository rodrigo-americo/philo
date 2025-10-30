/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 11:02:46 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/17 15:25:50 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static int	ft_is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

long	ft_atol(const char *str)
{
	long	result;
	int		i;

	result = 0;
	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '+' || str[i] == '-')
		return (-1);
	while (str[i])
	{
		if (!ft_is_digit(str[i]))
			return (-1);
		result = result * 10 + (str[i] - '0');
		if (result > INT_MAX)
			return (-1);
		i++;
	}
	return (result);
}

long long	ft_get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_print_status(t_philo_state *state, char *status)
{
	long long	timestamp;

	sem_wait(state->print);
	timestamp = ft_get_time_ms() - state->data.start_time;
	printf("%lld %d %s\n", timestamp, state->data.id, status);
	sem_post(state->print);
}

void	ft_one_philo_eat(t_philo_state *state)
{
	long long	start;

	ft_take_fork(state);
	start = ft_get_time_ms();
	while (1)
	{
		if (ft_get_time_ms() - start >= state->data.time_to_die)
			ft_check_death(state);
		usleep(500);
	}
}
