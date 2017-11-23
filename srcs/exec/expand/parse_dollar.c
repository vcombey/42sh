#include <stdio.h>
#include "libft.h"
#include "lexer.h"
#include "parser.h"
#include "t_env.h"
#include "local.h"
#include "exec.h"
#include "glob.h"
#include "expand.h"

char	*get_param(t_expand *exp, int *seen_hash)
{
	t_word	env;
	int		brace;

	brace = exp->words[exp->offset] == '{';
	w_newword(&env);
	if (brace)
		++exp->offset;
	if (exp->words[exp->offset] == '#')
	{
		*seen_hash = 1;
		if (!brace)
			return (NULL);
		++exp->offset;
	}
	if (ft_isalpha(exp->words[exp->offset]) || exp->words[exp->offset] == '_')
	{
		while (ft_isalnum(exp->words[exp->offset]) ||
				exp->words[exp->offset] == '_')
		{
			w_addchar(&env, exp->words[exp->offset]);
			++exp->offset;
		}
	}
	else if (ft_isdigit(exp->words[exp->offset]))
	{
		while (ft_isdigit(exp->words[exp->offset]))
		{
			w_addchar(&env, exp->words[exp->offset]);
			++exp->offset;
		}
	}
	else if (CHAR_IN_SET(exp->words[exp->offset], "*@$?!"))
	{
		w_addchar(&env, exp->words[exp->offset]);
		++exp->offset;
	}
	else
		return (env.str);
	if (!((brace && exp->words[exp->offset] == '}')))
		--exp->offset;
	return (env.str);
}

/*
**	poised after $
*/

int		parse_param(t_expand *exp, int quoted)
{
	char	*value;
	int		seen_hash;
	char	*env_var;

	value = NULL;
	seen_hash = 0;
	env_var = get_param(exp, &seen_hash);
	if (env_var == NULL)
	{
		if (seen_hash)
		{
			value = ft_strdup(var_get_value(singleton_env(), "#"));
			seen_hash = 0;
		}
		else
		{
			exp->offset--;
			w_addchar(&exp->word, '$');
			w_addchar(&exp->g_word, '$');
			return (0);
		}
	}
	else
		value = var_get_value(singleton_env(), env_var);
	free(env_var);
	if (seen_hash)
	{
		char param_length[20];
		w_addstr(&exp->word, ft_itoa_word((value ? ft_strlen(value) : 0), param_length));
		return (0);
	}
	handle_fieldsplitting(value, exp, quoted);
	return (0);
}

static int		is_arith_expansion(t_expand *exp)
{
	int i;
	int depth;

	depth = 0;
	i = 3 + exp->offset;
	while (exp->words[i] && !(depth == 0 && exp->words[i] == ')'))
	{
		if (exp->words[i] == '(')
			++depth;
		else if (exp->words[i] == ')')
			--depth;
		++i;
	}
	if (exp->words[i] == ')' && exp->words[i + 1] == ')')
	{
		return (1);
	}
	return (0);
}

int		parse_dollars(t_expand *exp, int quoted)
{
	if (exp->words[1 + exp->offset] == '"' ||
	exp->words[1 + exp->offset] == '\\' || exp->words[1 + exp->offset] == '\0')
	{
		w_addchar(&exp->word, '$');
		return (0);
	}
	if (exp->words[1 + exp->offset] == '(')
	{
		if (exp->words[2 + exp->offset] == '(')
		{
			if (is_arith_expansion(exp))
			{
				(exp->offset) += 3;
				return (parse_arith(exp));
			}
		}
		(exp->offset) += 2;
		return (parse_comm(exp, quoted));
	}
	else
	{
		++(exp->offset);
		return (parse_param(exp, quoted));
	}
}
