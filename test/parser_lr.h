#ifndef PARSER_LR_H
# define PARSER_LR_H

#define NB_SYMBOLS 3
#define NB_TOKEN 4
#define FIRST_SYMBOL 0
#define FIRST_TOKEN 10000

#define NB_RULES 4

#define IS_TOKEN(i) i >= FIRST_TOKEN && i <= FIRST_TOKEN + NB_TOKEN

#define IS_SYMBOL(i) i >= FIRST_SYMBOL && i <= FIRST_SYMBOL + NB_SYMBOLS

enum e_symbol {
	start_symbol = FIRST_SYMBOL,
	s,
	c,
};

enum e_token {
	DOLLAR = FIRST_TOKEN,
	E,
	F,
	EPSILON,
};

/*
**	a morpheme is a token or a symbol
*/

typedef int t_morpheme;

struct s_morpheme_lst {
	struct s_morpheme_lst	*next;
	t_morpheme				m;
};

struct s_static_grammar_rule {
	t_morpheme		node;
	t_morpheme		childs[7];
};

struct s_grammar_rule {
	t_morpheme				node;
	struct s_morpheme_lst		*childs;
};


/*
**	an item is a [S->ß.Eµ, a]
**	whit a gramar rule, a point and a look ahead symbol
*/

struct s_item {
	struct item			*next_item;
	int					gramar_rule;
	int					point;
	enum e_token		look_ahead;
};

struct s_parser_lr {
	struct s_grammar_rule		grammar_rules[NB_RULES];
	struct s_morpheme_lst		*firsts[NB_SYMBOLS + 2];
};

/*
** in firsts, the indice match the symbol
*/

struct s_line {
	struct s_line			*next;
	int 					state;
	struct s_item			*item_list;
	struct s_item			*closure;
	struct s_item			*kernel;
	int						action_table[NB_TOKEN];
	int				 		goto_table[NB_SYMBOLS];
};

void	init_grammar_rules(struct s_parser_lr *lr);
int		get_first_grammar_rule(struct s_grammar_rule *grammar_rule, enum e_symbol symbol) ;
int	search_morpheme_lst(struct s_morpheme_lst *a, t_morpheme m);
struct s_morpheme_lst *add_unique_morpheme_lst(struct s_morpheme_lst **a, t_morpheme m);
struct s_morpheme_lst *union_morpheme_lst(struct s_morpheme_lst **a, struct s_morpheme_lst *b);
struct s_morpheme_lst *new_morpheme_lst(t_morpheme m);
struct s_item *create_item(struct s_grammar_rule *rule, int point, t_morpheme token);
struct s_line *create_line(int state);

void	debug_token(int m);
void	debug_symbol(int m);
void	debug_grammar_rule(struct s_parser_lr *lr, int i);
void	debug_gramar(struct s_parser_lr *lr);
void	init_firsts(struct s_parser_lr *lr);
void	debug_firsts(struct s_parser_lr *lr);

#endif