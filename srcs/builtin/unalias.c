#include "env.h"
#include "builtin.h"
#include "libft.h"
#include "printf.h"
#include "failure.h"

int			builtin_unalias(t_env *env, const char **argv)
{
	int		argc;
	int		i;
	t_list	*node;
	int		ret;

	i = 0;
	ret = EXIT_SUCCESS;
	argc = ft_arraylen(argv);
	if (argc == 1)
		return (investigate_error(NULL, "unalias : Argument required", EXIT_FAILURE));
	while (++i < argc)
	{
		node = find_alias(env->alias, argv[i], ft_strlen(argv[i]));
		if (node)
		{
			ft_simple_lst_del_one(&env->alias, node, ft_free);
			ret = EXIT_SUCCESS;
		}
		else
			ret = investigate_error(NULL, "alias not found : ", EXIT_FAILURE);
	}
	return (ret);
}