#include "env.h"
#include "exec.h"
#include "libft.h"
#include "printf.h"
#include "line_editing.h"
#include "history.h"
#include "completion.h"

#include "color.h"
#include <stdio.h>

int	main(void)
{
	extern const char	**environ;

#ifdef PID
	dprintf(2, BLU"%d\n"RESET, getpid());
#endif
	env_load_base_env(singleton_env(), environ);
#ifndef NO_TERMCAPS
	create_ternary_tree(singleton_env());
	history_load(singleton_hist(), singleton_env());
	conf_term_in();
#endif
	main_loop(singleton_env());
	env_free_env(singleton_env());
	return (0);
}
