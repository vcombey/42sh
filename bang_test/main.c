#include <stdio.h>
#include "libft.h"
#include "bang.h"
#include <readline/readline.h>

int		main(void)
{
	const char	*input;
	char		*expanded_input;

	input = readline("$> ");
	if (!(expanded_input = bang_expand(input, NULL)))
	{
		dprintf(2, "ret is null\n");
		expanded_input = ft_strdup(input);
	}
	dprintf(2, "#%s#\n", expanded_input);
	free(expanded_input);
	while (1);
	return (0);
}
