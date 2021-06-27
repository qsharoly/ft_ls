/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_stricmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsharoly <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/05 15:19:30 by qsharoly          #+#    #+#             */
/*   Updated: 2021/06/27 09:32:13 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Case-insensitive strcmp
*/

int		ft_stricmp(char const *s1, char const *s2)
{
	int		diff;

	while (1)
	{
		diff = ft_tolower((unsigned char)*s1) - ft_tolower((unsigned char)*s2);
		if (diff != 0 || !*s1 || !*s2)
			break ;
		s1++;
		s2++;
	}
	return (diff);
}
