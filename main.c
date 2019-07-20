/*
TODO:
  1) Распарсить строку
  2) Создать дерево операторов
  3) Решить и удалить дерево

Алгоритм:
  1) Удаляем внешние скобки
  2) Ищем наименее приоритетный оператор
  Если есть
    3.1) Записываем оператор в текущий узел
    3.2) Создаем два потомка
    3.3) Запускаем рекурсию от двух потомков и подстрок \
        разделенных этим оператором
  Если нет
    3.1) Записываем число в текущий узел
*/

#include <libc.h>
#include <stdio.h>

typedef struct    s_node
{
  struct s_node   *left;
  struct s_node   *right;
  void            *content;
  size_t          size;
}                 t_node;

typedef struct    s_tree
{
  t_node          *root;
}                 t_tree;

typedef struct    s_opp
{
  char            opp;
  int             (*f)(int, int);
}                 t_opp;

typedef struct    s_ogs
{
  char            opp;
  int             n;
}                 t_ogs;

int sum(int a, int b)
{
  return (a + b);
}

int mul(int a, int b)
{
  return (a * b);
}

int sub(int a, int b)
{
  return (a - b);
}

int div(int a, int b)
{
  return (a / b);
}

int mod(int a, int b)
{
  return (a % b);
}

t_opp g_opps[] = 
{
  {'+', &sum},
  {'-', &sub},
  {'*', &mul},
  {'/', &div},
  {'%', &mod},
  {0, NULL}
}

t_tree  *new_tree()
{
  t_tree  *tmp;

  if ((tmp = malloc(sizeof(*tmp))))
    tmp->root = NULL;
  return (tmp);
}

t_node  *new_node(void *content, size_t size, char copy)
{
  t_node  *tmp;

  if ((tmp = malloc(sizeof(*tmp))))
  {
    if (size)
    {
      tmp->size = size;
      if (copy)
        tmp->content = content;
      else if ((tmp->content = malloc(size)))
        memcpy(content, tmp->content, size);
      else
        free(tmp);
    }
    else
      tmp->content = NULL;
  }
  return (tmp);
}

void  node_del(t_node *node, vodi (*del)(void *))
{
  if (!node)
    return ;
  node_del(node->left);
  node_del(node->right);
  (*del)(node->content);
  free(node);
}

void  tree_del(t_tree *tree, void (*del)(void *))
{
  node_del(tree->root, del);
  free(tree);
}

t_ogs *new_ogs(char c, int n)
{
  t_ogs *tmp;

  if ((tmp = malloc(sizeof(*tmp))))
  {
    tmp->c = c;
    tmp->n = n;
  }
  return (tmp);
}

void  ogs_del(void *content)
{
  free(content);
}

char  is_whitespace(char c)
{
  return (c == '\f' || c == '\n' || c == '\r' || c == '\t' \
      || c == '\v' || c == ' ');
}

char *parse(char *str)
{
  int i;
  int len;
  char *res;
  
  len = 0;
  i = -1;
  while (str[++i])
    if (!is_whitespace(str[i]))
      ++len;
  if ((res = malloc((len + 1) * sizeof(*res))))
  {
    res[len] = 0;
    i = -1;
    len = -1;
    while (str[++i])
      if (!is_whitespace(str[i]))
        res[++len] = str[i];
  }
  return (res);
}

char  *bracket_trim(char *str)
{
  int   i;
  int   balance;
  int   len;
  char  *buf;
  int   brs;

  len = strlen(str);
  if (*str != '(')
    return (str);
  i = -1;
  while (str[++i] == '(' && str[len - i - 1] == ')')
    ++brs;
  while (i < len - brs)
  {
    if (str[i] == '(')
      ++balance;
    else if (str[i] == ')')
      --balance;
    if (balance < 0)
    {
      ++balance;
      --brs;
    }
  }
  buf = str[brs];
  *str[len - brs] = 0;
}

char is_opp(char c)
{
  int i;
 
  i = -1;
  while (g_opps[++i].opp)
    if (c == g_opps[i].opp)
      return(1);
  return (0);
}

char  *find_opp(char *str)
{
  char *opp1;
  char *opp2;
  
  opp1 = NULL;
  opp2 = NULL;
  while(*++str)
  {
    if (!is_opp(*(str - 1)))
    {
      if (!opp2 && (*str == '+' || *str == '-'))
      {
        opp2 = str;
        return (opp2);
      }
      else if (!opp1 && (*str == '*' || *str == '/' || *str == '%'))
        opp1 = str;
    }
  }
  return (opp1);
}

void fill_node(t_node *node, char *str)
{
  char  *opp;

  str = bracket_trim(str);
  opp = find_opp(str);
  if (opp)
    ((t_ogs *)node->content)->c = opp;
  else
    ((t_ogs *)node->content)->n = atoi(str);
  *opp = 0;
  tmp = new_ogs(0,0);
  node->left = new_node(tmp, sizeof(*tmp), 1);
  tmp = new_ogs(0,0);
  node->right = new_node(tmp, sizeof(*tmp), 1);
  fill_node(node->left, str);
  fill_node(node->right, opp + 1);
}

int  count_tree(t_node *node)
{
  int i;

  if (node->c)
  {
    i = -1;
    while (g_opps[++i].opp)
      if (node->c == g_opps[i].opp)
        return (*(g_opps[i].f))(count_tree(node->left), \
            count_tree(node->right));
  }
  else
    return (node->n)
}

int main(int ac, char *av[])
{
  t_tree  *tree;
  char    *str;
  t_ogs   *tmp;
  int     res;

  tree = new_tree();
  if (ac == 2)
  {
    if ((str = parse(av[1])))
    {
      tmp = new_ogs(0, 0);
      tree->root = new_node(tmp, sizeof(*tmp), 1);
      fill_node(tree->root, str);
      res = count_tree(tree->root);
      printf("%d\n", res);
      tree_del(tree, &ogs_del);
      free(str);
    }
  }
  return 0;
}
