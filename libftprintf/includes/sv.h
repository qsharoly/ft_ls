/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sv.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: debby <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 21:37:35 by debby             #+#    #+#             */
/*   Updated: 2022/03/23 21:44:06 by debby            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SV_H
# define SV_H

typedef struct	s_sv
{
	const char	*start;
	int			length;
}				t_sv;

t_sv			sv_from_cstr(const char *cstring);

#endif
