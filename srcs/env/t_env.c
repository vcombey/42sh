#include "libft.h"
#include "env.h"

int	env_copy_env(t_env *new_env, t_env *old_env)
{
	if (!new_env || !old_env)
		return (0);
	new_env->environ = NULL;
	new_env->previous_exit = old_env->previous_exit;
	new_env->environ = env_create_environ((const char **)old_env->environ, \
			&(new_env)->environ_size);
	return (1);
}

int	env_free_env(t_env *env)
{
	if (!env)
		return (0);
	ft_arraydel(&(env->environ));
	return (1);
}