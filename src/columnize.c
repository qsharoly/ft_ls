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

#ifdef DEBUGLOG
#include <stdio.h>
void	dumpling(struct s_finfo **items, int item_count, t_iw *colwidths, int ncol)
{
	int height = item_count / ncol + (item_count % ncol > 0);
	int bo = height;
	int lk = 0;
	for (int a = 0; a < item_count; ++a)
	{
		if (a == bo) {
			printf("|");
			bo+=height;
		}
		else
			printf(" ");
		printf("%2d", items[a]->namelen);
		if (colwidths[lk].index == a)
			lk++;
	}
	printf("\n");
	bo = height;
	lk = 0;
	for (int a = 0; a < item_count; ++a)
	{
		if (a == bo) {
			printf("|");
			bo+=height;
		}
		else
			printf(" ");
		if (colwidths[lk].index == a) {
			lk++;
			printf(" x");
		}
		else
			printf("%2d",a);
	}
	printf("\n");
}
#endif

void	columnize(int **column_widths, int *ncol, struct s_finfo **items,
		int item_count, int separator_width, int width_limit) {
	t_iw	mem1[Ncol_limit];
	t_iw	mem2[Ncol_limit];
	t_iw	*colwidths = mem1;
	t_iw	*new_widths = mem2;

	colwidths[0] = max_iwidth(items, 0, item_count);
	*ncol = 1;
#ifdef DEBUGLOG
	printf("initial:\n");
	dumpling(items, item_count, colwidths, *ncol);
#endif
	int skipped_over = 0;
	while (*ncol < Ncol_limit && *ncol < item_count)
	{
		int	lookat = 0;
		int	old_height = item_count / *ncol + (item_count % *ncol > 0);
		int	new_at = 0;
		int	new_height = item_count / (*ncol + 1) + (item_count % (*ncol + 1) > 0);
#ifdef DEBUGLOG
		printf("splitting in %d groups of %d or less.\n", *ncol+1, new_height);
		dumpling(items, item_count, colwidths, *ncol+1);
#endif
		if (old_height == new_height)
		{
#ifdef DEBUGLOG
			printf("not enough elements for groups of %d or less. skipping over.\n", new_height);
#endif
			skipped_over += 1;
			*ncol += 1;
			continue;
		}
		int	new_width_sum = 0;
		int	col = 0;
		while (col < *ncol + 1 && col * new_height < item_count)
		{
			int	start = col * new_height;
			int	stop = my_min((col + 1) * new_height, item_count);
			t_iw	new_iw;
			if (!in_range(colwidths[lookat].index, start, stop))// || lookat + 1 >= *ncol)
			{
				//leave lookat unchanged
				new_iw = max_iwidth(items, start, stop);
#ifdef DEBUGLOG
				printf("[%d, %d] => %d\n", start, stop-1, new_iw.index);
#endif
			}
			else if (lookat + 1 >= *ncol || !in_range(colwidths[lookat + 1].index, start, stop))
			{
				int	prev_bound = col * old_height;
				if (prev_bound > 0 && prev_bound < stop)
				{
					if (colwidths[lookat].index < prev_bound)
					{
						new_iw = max_iwidth(items, prev_bound, stop);
#ifdef DEBUGLOG
						printf("<%d vs [%d, %d)", colwidths[lookat].index, prev_bound, stop);
#endif
					}
					else if (colwidths[lookat].index > prev_bound)
					{
						new_iw = max_iwidth(items, start, prev_bound);
#ifdef DEBUGLOG
						printf(">[%d, %d) vs %d", start, prev_bound, colwidths[lookat].index);
#endif
					}
					else
					{
						new_iw = max_iwidth(items, start, prev_bound);
#ifdef DEBUGLOG
						printf("=[%d, %d) vs %d", start, prev_bound, colwidths[lookat].index);
#endif
					}
					if (new_iw.width < colwidths[lookat].width)
						new_iw = colwidths[lookat];
#ifdef DEBUGLOG
					printf(" => %d\n", new_iw.index);
#endif
				}
				else
				{
					new_iw = colwidths[lookat];
#ifdef DEBUGLOG
					printf("just %d\n", new_iw.index);
#endif
				}
				lookat += 1;
			}
			else
			{
#ifdef DEBUGLOG
				printf("%d vs %d", colwidths[lookat].index, colwidths[lookat+1].index);
#endif
				if (colwidths[lookat].width > colwidths[lookat + 1].width)
					new_iw = colwidths[lookat];
				else
					new_iw = colwidths[lookat + 1];
				lookat += 2;
#ifdef DEBUGLOG
				printf(" => %d\n", new_iw.index);
#endif
			}
			new_widths[new_at++] = new_iw;
			new_width_sum += new_iw.width;
			col++;
		}
#ifdef DEBUGLOG
		printf("result:\n");
		dumpling(items, item_count, new_widths, *ncol+1);
#endif
		new_width_sum += *ncol * separator_width;
		if (new_width_sum > width_limit)
		//new_widths are over the limit.
		//stop the loop and use current colwidths and ncol.
		{
			*ncol -= skipped_over;
#ifdef DEBUGLOG
			printf("bad length sum. fallback.\n");
#endif
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
#ifdef DEBUGLOG
	printf("final:\n");
	dumpling(items, item_count, colwidths, *ncol);
#endif
	*column_widths = malloc(*ncol * sizeof(**column_widths));
	int	i = 0;
	while (i < *ncol)
	{
		(*column_widths)[i] = colwidths[i].width;
		i++;
	}
}
