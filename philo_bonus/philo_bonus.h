/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rgregori <rgregori@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 11:01:49 by rgregori          #+#    #+#             */
/*   Updated: 2025/10/17 14:49:30 by rgregori         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <semaphore.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/time.h>
# include <signal.h>
# include <limits.h>

# define SEM_FORKS_NAME "/philo_forks"
# define SEM_PRINT_NAME "/philo_print"
# define SEM_SEATS_NAME "/philo_seats"

typedef struct s_philo_data
{
	int			id;
	long		time_to_die;
	long		time_to_eat;
	long		time_to_sleep;
	int			max_eats;
	int			number_of_philosophers;
	long long	start_time;
}	t_philo_data;

typedef struct s_philo_state
{
	long long		last_meal;
	int				eat_count;
	sem_t			*forks;
	sem_t			*print;
	sem_t			*seats;
	t_philo_data	data;
}	t_philo_state;

typedef struct s_table
{
	int			number_of_philosophers;
	long		time_to_die;
	long		time_to_eat;
	long		time_to_sleep;
	int			max_eats;
	long long	start_time;
	pid_t		*pids;
	sem_t		*forks_sem;
	sem_t		*print_sem;
	sem_t		*seats_sem;
}	t_table;

long long	ft_get_time_ms(void);
void		ft_print_status(t_philo_state *state, char *status);
int			ft_init_semaphores(t_table *table);
void		ft_unlink_all_sems(void);
int			ft_open_semaphores(t_philo_state *state);
void		ft_philo_think(t_philo_state *state);
void		ft_philo_sleep(t_philo_state *state);
void		ft_philo_eat(t_philo_state *state);
void		ft_philosopher_routine(t_philo_data data);
void		ft_check_death(t_philo_state *state);
void		ft_check_finish(t_philo_state *state);
void		ft_cleanup(t_table *table);
t_table		*ft_init_table(int argc, char **argv);
int			ft_init_philos(t_table *table);
int			ft_init_semaphores(t_table *table);
void		ft_monitor_philosophers(t_table *table);
long		ft_atol(const char *str);
void		ft_one_philo_eat(t_philo_state *state);
void		ft_take_fork(t_philo_state *state);
#endif