/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmpi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/05 15:19:30 by qsharoly          #+#    #+#             */
/*   Updated: 2021/03/10 21:46:52 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Case-insensitive strcmp
*/

int		ft_strcmpi(char const *s1, char const *s2)
{
	int		diff;

	diff = ft_tolower((unsigned char)*s1) - ft_tolower((unsigned char)*s2);
	while (*s1 && *s2)
	{
		if (diff != 0)
			break ;
		else
		{
			s1++;
			s2++;
		}
		diff = ft_tolower((unsigned char)*s1) - ft_tolower((unsigned char)*s2);
	}
	return (diff);
}
