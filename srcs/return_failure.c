/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   return_failure.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sede <adrien.de.sede@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/17 17:15:57 by ade-sede          #+#    #+#             */
/*   Updated: 2017/05/18 15:03:01 by ade-sede         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "env.h"

int	return_failure(const char *str, const char *error_msg)
{
	ft_putstr(str);
	ft_putendl(error_msg);
	return (EXIT_FAILURE);
}