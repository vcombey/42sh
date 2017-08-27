/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcombey <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/27 05:07:29 by vcombey           #+#    #+#             */
/*   Updated: 2017/08/27 11:55:14 by vcombey          ###   ########.fr       */
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

static t_redir	g_redir[] =
{
	{TK_GREAT, &file_redir},
	{TK_DGREAT, &file_redir},
	{TK_LESS, &file_redir},
	{TK_LESSGREAT, &file_redir},
	{TK_LESSAND, &merge_fd},
	{TK_GREATAND, &merge_fd},
	{TK_HERE, &heredoc},
	{-1, NULL}
};

/*
**	This function is used to open the file with the right options depending on
**	the operator's id. FD of the opened file will be returned.
*/

/*
**	Fd merging with op >& and <&
*/

void	heredoc(int io_number, char *target, t_list **redir_stack, \
		t_token_id id)
{
	int		fd;
	char	*buff;

	(void)id;
	if (io_number == -1)
		io_number = 0;
	errno = 0;
	if ((fd = open("/tmp/42sh_heredoc", \
					O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0600)) < 0)
		dprintf(2, "Open : %s\n", strerror(errno));
	while (fd >= 0 && buff && !ft_strequ(buff, target))
	{
		load_prompt(singleton_env(), singleton_line(), \
				"heredoc", "heredoc> ");
		buff = line_editing_get_input(singleton_env(), \
				singleton_line(), singleton_hist());
		if (!ft_strequ(buff, target))
		{
			write(fd, buff, ft_strlen(buff));
			write(fd, "\n", 1);
		}
	}
	close(fd);
	errno = 0;
	if ((fd = open("/tmp/42sh_heredoc", O_RDONLY)) > 0)
		dprintf(2, "Open : %s\n", strerror(errno));
	exec_dup(io_number, fd, FALSE, redir_stack);
}

void	merge_fd(int io_number, char *target, t_list **redir_stack, \
		t_token_id id)
{
	int	target_fd;
	int	natural_fd;

	natural_fd = TRUE;
	if (io_number == -1)
		io_number = (id == TK_LESSAND) ? STDIN_FILENO : STDOUT_FILENO;
	if (ft_strequ(target, "-"))
	{
		natural_fd = FALSE;
		target_fd = open("/dev/null", O_WRONLY);
	}
	else
		target_fd = ft_atoi(target);
	errno = 0;
	if (target_fd >= STDIN_FILENO && (fcntl(target_fd, F_GETFD) \
				!= -1 && errno != EBADF))
		exec_dup(io_number, target_fd, natural_fd, redir_stack);
}

void	file_redir(int io_number, char *target, t_list **redir_stack, \
		t_token_id id)
{
	int	target_fd;

	if (io_number == -1)
		io_number = (id == TK_LESS || id == TK_LESSGREAT) ? \
					STDIN_FILENO : STDOUT_FILENO;
	target_fd = redir_open_file(target, id);
	if (target_fd >= STDIN_FILENO)
		exec_dup(io_number, target_fd, FALSE, redir_stack);
}

void	exec_redir(t_ast *ast, t_list **redir_stack)
{
	t_list		*child_list;
	t_ast		*child_node;
	int			io_number;
	void		(*f)(int, char*, t_list**, t_token_id);
	char		*target;
	t_token_id	id;
	size_t		i;

	i = 0;
	child_list = ast->child;
	io_number = -1;
	while (child_list)
	{
		child_node = child_list->data;
		if (child_node->token->id == TK_IO_NUMBER)
			io_number = ft_atoi(child_node->token->value);
		else if (TK_IS_REDIR(child_node->token->id))
		{
			while (g_redir[i].f && g_redir[i].id != (int)child_node->token->id)
				i++;
			f = g_redir[i].f;
			id = child_node->token->id;
		}
		else
			target = child_node->token->value;
		child_list = child_list->next;
	}
	if (f)
		f(io_number, target, redir_stack, id);
}
