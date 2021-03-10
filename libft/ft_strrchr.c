/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/09 13:15:44 by qsharoly          #+#    #+#             */
/*   Updated: 2019/09/09 13:47:02 by qsharoly         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	*ft_memrchr(const void *s, int c, size_t n)
{
	void *end;

	end = (void *)s + n - 1;
	while (end >= s)
	{
		if (*(char unsigned *)end == (char unsigned)c)
			return ((void *)end);
		end--;
	}
	return (NULL);
}

char		*ft_strrchr(const char *s, int c)
{
	return (ft_memrchr(s, c, ft_strlen(s) + 1));
}
