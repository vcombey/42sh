#include "libft.h"
#include <unistd.h>
#include "env.h"
#include "exec.h"
#include "builtin.h"
#include "line_editing.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>

/*
**	Deletes the entire tree, from the given node to the last leaf
*/

t_ast		*flush_tree(t_ast *ast)
{
	t_list	*child_list;
	t_ast	*child_node;

	if (ast)
	{
		child_list = ast->child;
		while (child_list)
		{
			child_node = child_list->data;
			if (child_node)
				flush_tree(child_node);
			child_list = child_list->next;
		}
		free_ast_node(ast);
	}
	return (NULL);
}

/*
** 	List of pipes should be generated during parsing and creation of the tree.
** 	Fork and execute the 1st branch in the child, meanwhile everything keeps going in the parent.
** 	Redirection should occure here, (in the child ?) not at the binary / builtin call time.
*/

static int	exec_pipe(t_ast *ast, t_lst_head *head)
{
	if (exec_tree(ast->child->data, head) == 0)
		exec_tree(ast->child->next->data, head);
	else
	{
		ast->child->data = flush_tree(ast->child->next->data);
		head->middle = head->middle->next;
	}
	return (0);
}

/*
**	The first time exec_tree is called, we give it our root node as an
**	argument. Depending on the node symbol, it will start the execution of the
**	various commands.
*/

int			end_branch(int error, t_ast *ast)
{
	ast = free_ast_node(ast);
	return (error);
}

int			exec_tree(t_ast *ast, t_lst_head *head)
{
	t_token		*token;

	token = NULL;
	if (ast)
	{
		if (ast->symbol == SIMPLE_COMMAND)
		{
			pid_t		child;
			int			*p_right;
			int			*p_left;
			int			save;
			t_list_d	*cur;

			cur = head->middle;
			p_right = (cur != NULL) ? cur->data : NULL;
			p_left = (cur && cur->prev) ? cur->prev->data : NULL;

			if (p_right) // If our command is on the left of a pipe (WRITE_END)
			{
				child = fork();
				if (child == 0) // In child process
				{
					dprintf(2, "Addr of p_right in child is %p\n", p_right);
					dprintf(2, "Addr of p_left in child is %p\n", p_left);

					// Write to the pipe to the right
					dup2(p_right[WRITE_END], STDOUT_FILENO);
					close(p_right[READ_END]);

					if (p_left) // Between pipes
					{
						dprintf(2, RED"YUP\n"RESET);//			REMOVE		
						// Read from the pipe to the left
						dup2(p_left[READ_END], STDIN_FILENO);
						close(p_left[WRITE_END]);
					}

					// Execute the command (execve call)
					exec_simple_command(ast, head);
					
					/* All FDs are closed from this process */
					close(p_right[WRITE_END]);
					if (p_left)
						close(p_left[READ_END]);
					exit(0);
					
					/* Checking exit did happen */
					dprintf(2, RED"Didnt exit\n");
				}
			}
			else if (p_left) // Right of the pipe ; happens in the main process.
			{
				dprintf(2, "Addr of p_right in parent is %p\n", p_right);
				dprintf(2, "Addr of p_left in parent is %p\n", p_left);
				// Save STD INPUT
				save = dup(STDIN_FILENO);

				/* Read from the pipe on the left */
				dup2(p_left[READ_END], STDIN_FILENO);
				close(p_left[WRITE_END]);

				/* Execute the command */
				exec_simple_command(ast, head);

				/* Restore fd 0 as the STD INPUT */
				close(p_left[READ_END]);
				dup2(save, 0);
				close(save);
			}
			if (!p_right && !p_left) // Not related to a pipe in anyway.
				exec_simple_command(ast, head);

			// Command execution is done : move to the next node in the list.
			if (head->middle)
				head->middle = head->middle->next;
		}
		else if (ast->symbol == COMPLEXE_COMMAND)
		{
			token = ast->token;
			if (ft_strequ(token->value, "|"))
				exec_pipe(ast, head);
			else if (ft_strequ(token->value, ";"))
				return (end_branch(semi_colon(ast, head), ast));
			else if (ft_strequ(token->value, "&&"))
				return (end_branch(logical_and(ast, head), ast));
			else if (ft_strequ(token->value, "||"))
				return (end_branch(logical_or(ast, head), ast));
		}
		if (ast)
			return (end_branch(0, ast));
	}
	return (1);
}
