#include "line_editing.h"
#include "lexer.h"
#include "parser.h"
#include "color.h"
#include "libft.h"
#include "exec.h"
#include "printf.h"

/*
**	If we have an assignement work token, such as key=value, where value is
**	contained in another token, we join the two tokens.
**	Example:
**
**	`key="value"' is a valid assignement. token 1 has value `key=' and token 2
**	and unquoted `value' (quote removal already occured).
**
**	`key= "value"' is not a valid assignement.
**	We are doing this only and only if the assignement isnt complete, and the
**	character following the first equal sign in token1 is null.
*/

static void	complete_assignement_word(t_token *token, t_lexer *lex)
{
	t_token	*next_token;
	char	*eq_sign;

	eq_sign = NULL;
	eq_sign = ft_strchr(token->value, '=');
	if (*(eq_sign + 1) == 0)
		ft_simple_lst_del_one(&lex->stack, lex->stack, NULL);
	if ((next_token = start_lex(lex)))
	{
		if (next_token->type == DQUOTED || next_token->type == QUOTED)
		{
			token->value = ft_strchange(token->value, \
					ft_strjoin(token->value, next_token->value));
		}
	}
}

/*
**	Creates a node from a word token, returns it.
*/

static t_ast		*ast_create_node_from_word(t_token *token, t_lexer *lex)
{
	t_ast	*node;

	if (token->id == TK_ASSIGNMENT_WORD && \
			(token->delimiter == '"' || token->delimiter == 39))
		complete_assignement_word(token, lex);
	if ((token->id == TK_WORD || token->type == EXPAND) \
			&& token->cmd_name)
		node = ast_create_node(token, NULL, CMD_NAME);
	else
	{
		if (token->cmd_name)
			node = ast_create_node(token, NULL, CMD_PREFIX);
		else
			node = ast_create_node(token, NULL, CMD_SUFFIX);
	}
	return (node);
}

t_ast		*create_simple_command(t_lexer *lex)
{
	t_ast	*ast;

	ast = ast_create_node(NULL, NULL, SIMPLE_COMMAND);
	ast = fill_simple_command(ast, lex);
	return (ast);
}

t_ast		*fill_simple_command(t_ast *simple_cmd, t_lexer *lex)
{
	t_ast	*new_node;
	t_token	*token;

	if ((token = start_lex(lex)))
	{
		if (TK_IS_SEP(token->id))
			return (simple_cmd);
		else
		{
			if (TK_IS_REDIR(token->id))
			{
				if (!(simple_cmd = append_redir(simple_cmd, token, lex)))
					return (NULL);
			}
			else
			{
				new_node = ast_create_node_from_word(token, lex);
				ft_simple_lst_del_one(&lex->stack, lex->stack, NULL);
				ft_simple_lst_pushback(&((simple_cmd)->child), \
						ft_simple_lst_create(new_node));
			}
			simple_cmd = fill_simple_command(simple_cmd, lex);
		}
	}
	return (simple_cmd);
}