/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/04 16:08:51 by qsharoly          #+#    #+#             */
/*   Updated: 2021/02/24 09:25:57 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stdlib.h>

# define ITOA_BUF_SIZE 65

typedef struct	s_split_info
{
	size_t			source_len;
	unsigned int	elem_count;
}				t_split_info;

typedef struct	s_list
{
	void			*content;
	size_t			content_size;
	struct s_list	*next;
}				t_list;

typedef struct	s_btree
{
	void			*content;
	size_t			content_size;
	struct s_btree	*left;
	struct s_btree	*right;
}				t_btree;

void			*ft_memset(void *b, int c, size_t len);
void			*ft_memcpy(void *dest, const void *src, size_t n);
void			*ft_memmove(void *dest, const void *src, size_t len);
void			*ft_memccpy(void *dest, const void *src, int c, size_t n);
void			*ft_memchr(const void *s, int c, size_t n);
int				ft_memcmp(const void *s1, const void *s2, size_t n);
void			ft_bzero(void *s, size_t n);
void			ft_putbyte(char unsigned c);
void			ft_putmem(const void *s, size_t n);
void			ft_putmem_ascii(const void *s, size_t n);
int				ft_isalpha(int c);
int				ft_isdigit(int c);
int				ft_isalnum(int c);
int				ft_isascii(int c);
int				ft_isprint(int c);
int				ft_toupper(int c);
int				ft_tolower(int c);
void			ft_putchar(char c);
void			ft_putchar_fd(int fd, char c);
void			ft_putstr(char const *s);
void			ft_putstr_fd(int fd, char const *s);
void			ft_putendl(char const *s);
void			ft_putendl_fd(int fd, char const *s);
void			ft_putnbr(int n);
void			ft_putnbr_endl(int n);
void			ft_putnbr_fd(int fd, int n);
size_t			ft_strlen(char const *s);
int				ft_strcmp(char const *s1, char const *s2);
int				ft_strncmp(char const *s1, char const *s2, size_t n);
char			*ft_strdup(char const *s1);
char			*ft_strcpy(char *dst, char const *src);
char			*ft_strncpy(char *dst, char const *src, size_t n);
size_t			ft_strlcpy(char *dst, char const *src, size_t size);
char			*ft_strcat(char *s1, char const *s2);
char			*ft_strncat(char *s1, char const *s2, size_t n);
size_t			ft_strlcat(char *dst, char const *src, size_t size);
char			*ft_strchr(char const *s, int c);
char			*ft_strhas(char const *s, int c);
char			*ft_strrchr(char const *s, int c);
char			*ft_strstr(char const *haystack, char const *needle);
char			*ft_strnstr(char const *haystack,
					char const *needle, size_t len);
int				ft_sqrt(int	nb);
int				ft_atoi(char const *str);
void			*ft_memalloc(size_t size);
void			ft_memdel(void **ap);
char			*ft_strnew(size_t size);
void			ft_strdel(char **as);
void			ft_strclr(char *s);
void			ft_striter(char *s, void (*f)(char *));
void			ft_striteri(char *s, void (*f)(unsigned int, char *));
char			*ft_strmap(char const *s, char (*f)(char));
char			*ft_strmapi(char const *s, char (*f)(unsigned int, char));
int				ft_strequ(char const *s1, char const *s2);
int				ft_strnequ(char const *s1, char const *s2, size_t n);
char			*ft_strsub(char const *s, unsigned int start, size_t len);
char			*ft_strjoin(char const *s1, char const *s2);
char			*ft_strtrim(char const *s);
char			**ft_strsplit(char const *s, char c);
char			**ft_strsplit_n(char const *s, char c, int *count);
char			*ft_itoa(int n);
t_list			*ft_lstnew(void const *content, size_t content_size);
void			ft_lstadd(t_list **alst, t_list *new_node);
void			ft_lstdelone(t_list **alst, void (*del)(void *, size_t));
void			ft_lstdel(t_list **alst, void (*del)(void *, size_t));
void			ft_lstiter(t_list *lst, void (*f)(t_list *elem));
t_list			*ft_lstmap(t_list *lst, t_list *(*f)(t_list *elem));
t_list			*ft_lst_push_tail(t_list **alst, t_list *new_node);
t_btree			*ft_btree_new_node(void const *content, size_t content_size);
t_btree			*ft_btree_add_unique(t_btree **tree, t_btree *new_node,
					int (*cmp)(void *, void *));
t_btree			*ft_btree_get(t_btree *tree, void *needle,
					int (*cmp)(void *, void *));
void			ft_btree_free_node(t_btree **tree, void (*del)(void *, size_t));
void			ft_btree_delete(t_btree **tree, void (*del)(void *, size_t));
void			ft_del_bzero(void *data, size_t size);
void			ft_simple_del(void *data, size_t size);
long long		ft_abs(long long i);
int				ft_max(int a, int b);
int				ft_min(int a, int b);

#endif
