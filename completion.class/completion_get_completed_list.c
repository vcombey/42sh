/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   completion_get_completed_list.c                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmickael <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/10 05:14:11 by bmickael          #+#    #+#             */
/*   Updated: 2017/09/10 05:14:13 by bmickael         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private_completion.h"

static char		**ft_up_realloc(char **origin, size_t o_size, size_t n_size)
{
	char **new_str;

	if (!(new_str = (char **)ft_memalloc(sizeof(char *) * (n_size + 1))))
		return (NULL);
	ft_memcpy(new_str, origin, o_size * sizeof(char *));
	free(origin);
	return (new_str);
}

static void		comple_bin_matches_extract_tree(t_ternary_tree *node,
				char ***matches, size_t *i, size_t *size)
{
	if (node == NULL)
		return ;
	if (node->match)
	{
		if (*i >= *size)
		{
			if (!(*matches = ft_up_realloc(*matches, *size,
			(*size + MALLOC_UNIT))))
				exit(EXIT_FAILURE);
			*size += MALLOC_UNIT;
		}
		if (!((*matches)[*i] = ft_strdup(node->match)))
			exit(EXIT_FAILURE);
		*i = *i + 1;
	}
	comple_bin_matches_extract_tree(node->left, matches, i, size);
	comple_bin_matches_extract_tree(node->down, matches, i, size);
	comple_bin_matches_extract_tree(node->right, matches, i, size);
}

static void		layer(t_ternary_tree *node,
				size_t *i, size_t *size, t_fourretout *s)
{
	if (node->left || node->right)
	{
			comple_bin_matches_extract_tree(node, s->matches, i, size);
			*(s->cur) = realloc(*(s->cur), (s->i + 1) * sizeof(char));
			(*(s->cur))[s->i] = '\0';
	}
	else if (node->down)
	{
		(s->i)++;
		layer(node->down, i, size, s);
		(s->i)--;
		(*(s->cur))[s->i] = node->c;
	}
	else
	{
		*(s->cur) = realloc(*(s->cur), (s->i + 2) * sizeof(char));
		(*(s->cur))[s->i + 1] = '\0';
		(*(s->cur))[s->i] = node->c;
	}
}

static void		comple_bin_match_trip_tree(t_ternary_tree *node,
				char ***matches, t_fourretout *s, size_t size)
{
	size_t	i;

	i = 0;
	if ((*(s->cur))[s->i] == '\0' || !node)
		return (layer(node, &i, &size, s));
	if ((*(s->cur))[s->i] < node->c && (!node->left || (node->left && (*(s->cur))[s->i] > node->left->c)))
		return ;
	else if ((*(s->cur))[s->i] > node->c
			&& (!node->right || (node->right && (*(s->cur))[s->i] < node->right->c)))
		return ;
	else if ((*(s->cur))[s->i] < node->c)
		return (comple_bin_match_trip_tree(node->left, matches, s, size));
	else if ((*(s->cur))[s->i] > node->c)
		return (comple_bin_match_trip_tree(node->right, matches, s, size));
	else if ((*(s->cur))[s->i] == node->c && node->down)
	{
		s->i += 1;
		return (comple_bin_match_trip_tree(node->down, matches, s, size));
	}
	else if ((*(s->cur))[s->i] == node->c && !node->down)
		return ;
}

void			comple_bin_match_trip_layer(t_ternary_tree *node,
				char ***matches, char **cur, size_t size)
{
	t_fourretout s;

	s.matches = matches;
	s.cur = cur;
	s.i = 0;
	comple_bin_match_trip_tree(node, matches, &s, size);
}