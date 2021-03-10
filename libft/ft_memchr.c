/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/06 20:33:43 by qsharoly          #+#    #+#             */
/*   Updated: 2019/09/09 13:46:22 by qsharoly         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	void *after;

	after = (void *)s + n;
	while (s < after)
	{
		if (*(char unsigned *)s == (char unsigned)c)
			return ((void *)s);
		s++;
	}
	return (NULL);
}
