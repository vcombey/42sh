/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-sede <adrien.de.sede@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/01 18:23:00 by ade-sede          #+#    #+#             */
/*   Updated: 2017/06/02 12:43:07 by ade-sede         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "libft.h"

t_lexer		init_lexer(const char *line)
{
	t_lexer		lex;

	lex.line = line;
	lex.index = 0;
	lex.stack = NULL;
	lex.state = DEFAULT;
	return (lex);
}

t_token		*create_token(char *value)
{
	t_token		*token;

	token = palloc(sizeof(*token));
	token->value = value;
	token->id = 0;
	token->size = ft_strlen(value);
	return (token);
}
