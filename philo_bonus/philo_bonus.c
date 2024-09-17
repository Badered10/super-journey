/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: baouragh <baouragh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 18:12:59 by baouragh          #+#    #+#             */
/*   Updated: 2024/09/16 20:05:36 by baouragh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

int	parse(int argc, char **argv)
{
	int	x;
	int	y;

	if (argc < 5 || argc > 6)
		return (printf("Must be 5 or 6 args, you entered : %d !!!\n", argc), 0);
	x = 1;
	while (argv[x])
	{
		y = 0;
		if (argv[x][y] == '+')
			y++;
		while (ft_isdigit(argv[x][y]))
			y++;
		if (argv[x][y] && !ft_isdigit(argv[x][y]))
		{
			printf("Error in args.\n");
			printf("Stop '%c' on '%s'", argv[x][y], argv[x]);
			printf(", Please check out !\n");
			return (0);
		}
		x++;
	}
	return (1);
}

int	eating(t_philo *philo)
{
	time_t	time;

	if (take_forks(philo))
		return (put_forks(philo), -1);
	time = get_t() - philo->data->start;
	philo->last_meal_time = time;
	printf("%ld %ld is eating\n", time, philo->id);
	sem_wait(philo->p_semaphore);
	philo->eaten_meals++;
	sem_post(philo->p_semaphore);
	if (philo->eaten_meals == philo->data->num_of_meals)
		philo->full = 1;
	ft_usleep(philo->data->tte, philo->data);
	put_forks(philo);
	return (0);
}

void	philosophy(t_philo	*philo)
{
	char *name;

	name = ft_itoa(philo->id);
	philo->p_semaphore = sem_open(name, O_CREAT | S_IRUSR | S_IWUSR, 0 , 1);
	if (philo->p_semaphore == SEM_FAILED)
	{
		perror("child");
		unlink(name);
		free(name);
		printf("FAILD TO CREAT SEM\n");
		sem_close(philo->data->ps_semaphore);
		exit(0);
	}
	pthread_create(&philo->scanner, NULL, &scan_death, &philo);
	if (philo->id % 2)
		ft_usleep(1, philo->data);
	while (philo->full != 1)
	{
		if (eating(philo))
			break ;
		printf("%ld %ld is sleeping\n", get_t() - philo->data->start,
			philo->id);
		ft_usleep(philo->data->tts, philo->data);
		printf("%ld %ld is thinking\n", get_t() - philo->data->start,
			philo->id);
		if (philo->data->num_of_philos % 2)
			ft_usleep(1, philo->data);
	}
	pthread_join(philo->scanner, NULL);
	sem_unlink(name);
	free(name);
	sem_close(philo->data->ps_semaphore);
	exit (0);
}

void	simulation(t_data *data)
{
	int	i;
	int	id;
	int exit;
	
	id = 0;
	i = 0;
	exit = 200;
	data->die_flag = 0;
	data->start = get_t();
	while (i < data->num_of_philos)
	{
		init_philo(data , i);
		id = fork();
		if (id == 0)
			philosophy(&data->philos);
		i++;
	}
	while(waitpid(-1, &exit, 0) != -1 && exit != 1)
	;
	printf("DONE\n");
}

int	main(int argc, char **argv)
{
	t_data	*data;

	if (!parse(argc, argv))
		return (1);
	data = set_data(argc, argv);
	if (!data)
		return (2);
	if (data->num_of_meals != 0)
		simulation(data);
	clean_up(data);
}
