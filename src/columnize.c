#include <stdlib.h>
#include "ft_ls.h"

enum {
	Ncol_limit = 20
};

typedef struct	s_iw{
	int	index;
	int	width;
}				t_iw;

static void	swap_pointers(t_iw **mem1_ptr, t_iw **mem2_ptr) {
	t_iw	*tmp;
	tmp = *mem1_ptr;
	*mem1_ptr = *mem2_ptr;
	*mem2_ptr = tmp;
}

static int	in_range(int test_me, int start, int stop) {
	return (start <= test_me && test_me < stop);
}

static t_iw	max_iwidth(struct s_finfo **infos, int start, int stop) {
	t_iw	max_iw;

	max_iw.index = start;
	max_iw.width = infos[start]->namelen;
	start++;
	while (start < stop)
	{
		if (infos[start]->namelen > max_iw.width)
		{
			max_iw.index = start;
			max_iw.width = infos[start]->namelen;
		}
		start++;
	}
	return max_iw;
}

static inline int my_min(int a, int b)
{
	return a < b ? a : b;
}

void	columnize(int **column_widths, int *ncol, struct s_finfo **items,
		int item_count, int separator_width, int width_limit) {
	t_iw	mem1[Ncol_limit];
	t_iw	mem2[Ncol_limit];
	t_iw	*colwidths = mem1;
	t_iw	*new_widths = mem2;

	colwidths[0] = max_iwidth(items, 0, item_count);
	*ncol = 1;
	while (*ncol < Ncol_limit && *ncol < item_count)
	{
		int	lookat = 0;
		int	new_at = 0;
		int	new_height = item_count / (*ncol + 1) + (item_count % (*ncol + 1) > 0);
		int	new_width_sum = 0;
		int	col = 0;
		while (col < *ncol + 1 && col * new_height < item_count)
		{
			int	start = col * new_height;
			int	stop = my_min((col + 1) * new_height, item_count);
			t_iw	new_iw;
			if (lookat >= *ncol - 1 || !in_range(colwidths[lookat].index, start, stop))
			{
				new_iw = max_iwidth(items, start, stop);
				//leave lookat unchanged
			}
			else if (!in_range(colwidths[lookat + 1].index, start, stop))
			{
				int	prev_bound = col * (item_count / *ncol);
				if (prev_bound < stop)
				{
					if (colwidths[lookat].index < prev_bound)
						new_iw = max_iwidth(items, prev_bound, stop);
					else
						new_iw = max_iwidth(items, start, prev_bound);
					if (new_iw.width < colwidths[lookat].width)
						new_iw = colwidths[lookat];
				}
				else
				{
					new_iw = colwidths[lookat];
				}
				lookat += 1;
			}
			else
			{
				if (colwidths[lookat].width > colwidths[lookat + 1].width)
					new_iw = colwidths[lookat];
				else
					new_iw = colwidths[lookat + 1];
				lookat += 2;
			}
			new_widths[new_at++] = new_iw;
			new_width_sum += new_iw.width;
			col++;
		}
		new_width_sum += *ncol * separator_width;
		if (new_width_sum > width_limit)
		//new_widths are over the limit.
		//stop the loop and use current colwidths and ncol.
		{
			break;
		}
		else if (new_width_sum == width_limit)
		// new_widths fit exactly within the limit.
		// stop the loop but increment ncol and use the new_widths for output.
		{
			*ncol += 1;
			swap_pointers(&colwidths, &new_widths);
			break;
		}
		else
		// new_width_sum < width_limit. have not reached the limit yet.
		{
			*ncol += 1;
			swap_pointers(&colwidths, &new_widths);
		}
	}
	*column_widths = malloc(*ncol * sizeof(**column_widths));
	int	i = 0;
	while (i < *ncol)
	{
		(*column_widths)[i] = colwidths[i].width;
		i++;
	}
}
