/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/05 15:19:30 by qsharoly          #+#    #+#             */
/*   Updated: 2019/09/09 23:02:55 by qsharoly         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int		ft_strcmp(char const *s1, char const *s2)
{
	int		diff;

	diff = (unsigned char)*s1 - (unsigned char)*s2;
	while (*s1 && *s2)
	{
		if (diff != 0)
			break ;
		else
		{
			s1++;
			s2++;
		}
		diff = (unsigned char)*s1 - (unsigned char)*s2;
	}
	return (diff);
}
