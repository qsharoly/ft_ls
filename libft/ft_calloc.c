/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/27 13:11:43 by debby             #+#    #+#             */
/*   Updated: 2021/06/27 13:25:46 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

void	*ft_calloc(size_t num, size_t size)
{
	void	*memory;

	if (!num || !size)
		return (NULL);
	memory = malloc(num * size);
	if (!memory)
		return (NULL);
	ft_bzero(memory, num * size);
	return (memory);
}
