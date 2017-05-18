/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unsetenv.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcombey <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/03/30 18:51:04 by vcombey           #+#    #+#             */
/*   Updated: 2017/05/17 16:34:31 by ade-sede         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft.h"
#include <unistd.h>

int	builtin_unsetenv(t_env *env, char **argv)
{
	size_t		i;
	int		argc;

	argc = ft_arraylen(argv);
	if (argc == 1)
		return (return_failure("unsetenv: Too few arguments\n", 2));
	i = 1;
	while (i < argc)
	{
		env_remove_var(env, argv[i]);
		i++;
	}
}
