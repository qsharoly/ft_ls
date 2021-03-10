/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/03 20:43:32 by qsharoly          #+#    #+#             */
/*   Updated: 2019/09/11 13:23:56 by qsharoly         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

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
