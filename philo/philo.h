/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 12:59:47 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/13 15:41:31 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>
# include <limits.h>

typedef pthread_mutex_t	t_fork;

typedef struct s_table
{
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				number_of_philosophers;
	int				max_eats;
	long long		start_time;
	int				is_dead;
	int				stop;
	pthread_mutex_t	data_mutex;
	pthread_mutex_t	print_mutex;
	t_fork			*forks;
	struct s_philo	*philos;
}	t_table;

typedef struct s_philo
{
	int			id;
	int			eats_count;
	long long	time_last_meal;
	pthread_t	thread_id;
	t_fork		*l_fork;
	t_fork		*r_fork;
	t_table		*table;
}	t_philo;

long		ft_atol(const char *nptr);
long long	ft_get_time_ms(void);
void		ft_destroy_table(t_table *table);
void		ft_destroy_forks(t_table *table, int qtd);
t_table		*ft_init_table(int argc, char **argv);
int			ft_init_forks(t_table *table);
int			ft_init_philos(t_table *table);
void		ft_print_status(t_philo *philo, const char *status);
void		*philo_routine(void *arg);
void		philo_monitor(t_table *table);
void		ft_start_simulation(t_table *table);
int			ft_is_simulation_over(t_table *table);
void		ft_philo_eat(t_philo *philo);
void		ft_philo_think(t_philo *philo);
void		ft_philo_sleep(t_philo *philo);
void		ft_set_dead_and_log(t_philo *philo);
int			sleep_precise(long long ms, t_table *table);

#endif