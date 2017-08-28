/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcombey <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/27 11:41:30 by vcombey           #+#    #+#             */
/*   Updated: 2017/08/27 11:44:56 by vcombey          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "libft.h"
#include "exec.h"
#include "line_editing.h"
#include "lexer.h"
#include "parser.h"

int		redir_open_file(char *target, t_token_id id)
{
	int	target_fd;
	int	mode;

	mode = 0;
	if (id == TK_LESS)
		mode |= O_RDONLY;
	if (id != TK_LESS)
	{
		mode |= (id == TK_LESSGREAT) ? O_RDWR : O_WRONLY;
		if (access(target, F_OK) == -1)
			mode |= O_CREAT;
		if (id == TK_DGREAT)
			mode |= O_APPEND;
		else if (id != TK_LESSGREAT && id != TK_DGREAT)
			mode |= O_TRUNC;
	}
	if (mode & O_CREAT)
		target_fd = open(target, mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	else
		target_fd = open(target, mode);
	return (target_fd);
}

/*
**	save[0] is the original fd (source)
**	save[1] The duplicate of the source fd save[0]
**	save[2] is the fd of the opened file. -1 if no file was opened. (target)
**	save[3] is a flag that indicates if save[1](target) is a fd that should or
**	not be closed. If the fd shouldnt be closed its value is 1.
**	Must be careful about never closing stdin stdout or stderr.
*/

/*
**	Function used to restore all FDs after work is done.
*/

void	close_redir(t_list *redir_stack)
{
	t_list	*first;
	int		*save;

	first = redir_stack;
	while (first)
	{
		save = first->data;
		dup2(save[1], save[0]);
		if (save[3] != TRUE)
			close(save[2]);
		close(save[1]);
		first = first->next;
	}
	ft_simple_lst_remove(&redir_stack, ft_free);
}

/*
**	The function that actually duplictes the fd, and pushes \
**	the operation onto our stack.
*/

void	exec_dup(int io_number, int target_fd, int natural_fd, \
		t_list **redir_stack)
{
	int	*save;

	save = palloc(sizeof(*save) * 4);
	save[0] = io_number;
	save[1] = dup(io_number);
	save[2] = target_fd;
	save[3] = natural_fd;
	ft_simple_lst_add(redir_stack, ft_simple_lst_create(save));
	dup2(target_fd, io_number);
}