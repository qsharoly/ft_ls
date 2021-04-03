/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/03 20:43:32 by qsharoly          #+#    #+#             */
/*   Updated: 2021/03/20 12:57:57 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

char	*ft_strdup(char const *src)
{
	size_t	i;
	char	*out;

	out = (char *)malloc(sizeof(*out) * (ft_strlen(src) + 1));
	if (out)
	{
		i = 0;
		while (src[i])
		{
			out[i] = src[i];
			i++;
		}
		out[i] = '\0';
	}
	return (out);
}
