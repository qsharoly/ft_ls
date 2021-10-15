#include "ft_ls.h"

enum {
	Ncol_limit = 20;
};

typedef struct	s_iw{
	int	index;
	int	width;
}				t_iw;

static int	in_range(int test_me, int start, int stop) {
	return (start <= test_me && test_me < stop);
}

static t_iw	max_iwidth(struct f_info **infos, int start, int stop) {
	t_iw	max_iw;

	max_iw.index = start;
	max_iw.width = infos[start]->name_length;
	start++;
	while (start < stop)
	{
		if (infos[start]->name_length > max_iw.width)
		{
			max_iw.index = start;
			max_iw.width = infos[start]->name_length;
		}
		start++;
	}
	return max_iw;
}

void	columnize(int *column_widths, int *ncol, struct f_info **items,
		int item_count, int width_limit) {
	t_iw	mem1[Ncol_limit];
	t_iw	mem2[Ncol_limit];
	t_iw	*colwidths = mem1;
	t_iw	*new_widths = mem2;
	int		new_width_sum;

	colwidths[0] = max_iwidth(items, 0, item_count);
	*ncol = 1;
	while (1 && *ncol < Ncol_limit)
	{
		int	lookat = 0;
		int	new_at = 0;
		int	new_height = info_count / (*ncol + 1);
		int	new_width_sum = 0;
		int	col = 0;
		while (col < *ncol + 1)
		{
			int	start = col * new_height;
			int	stop = (col + 1) * new_height;
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
		if (new_width_sum > width_limit)
		{
			break;
		}
		*ncol += 1;
		t_iw	*tmp = colwidths;
		colwidths = new_widths;
		new_widths = tmp;
		if (new_width_sum == width_limit)
		{
			break;
		}
	}
	column_widths = malloc(*ncol * (*column_widths));
	int	i = 0;
	while (i < *ncol)
	{
		column_widths[i] = colwidths[i].width;
		i++;
	}
}
