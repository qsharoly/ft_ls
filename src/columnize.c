#include <stdlib.h>
#include "ft_ls.h"

//#define DEBUGLOG

#ifdef DEBUGLOG
#include <stdio.h>
#endif
__attribute__((__format__(__printf__, 1, 2)))
static void	dbglog(const char *format, ...)
{
#ifdef DEBUGLOG
	va_list	ap;

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
#else
	(void)format;
#endif
}

static inline int min(int a, int b)
{
	return a < b ? a : b;
}

static int	max_width_between(struct s_finfo **items, int start, int stop)
{
	int	max_w;

	max_w = items[start]->namelen;
	for (int i = start + 1; i < stop; i++)
	{
		if (items[i]->namelen > max_w)
			max_w = items[i]->namelen;
	}
	dbglog("items [%d, %d): ", start, stop);
	dbglog("max_width: %d\n", max_w);
	return max_w;
}

static int	column_width_sum(struct s_finfo **items, int item_count, int stride)
{
	int	sum = 0;

	for (int start = 0; start < item_count; start += stride)
	{
		int stop = min(start + stride, item_count);
		sum += max_width_between(items, start, stop);
	}
	return sum;
}

int	columnize(int **column_widths, struct s_finfo **items, int item_count,
		int separator_width, int width_limit)
{
	dbglog("width_limit: %d\n", width_limit);
	int stride_small_bound = 1;
	int	stride_big_bound = item_count;
	int stride_estimate = 0;
	while (stride_small_bound != stride_big_bound)
	{
		dbglog("\n");
		int	new_estimate = (stride_small_bound + stride_big_bound) / 2;
		dbglog("new estimate: %d\n", new_estimate);
		if (new_estimate == stride_estimate)
		{
			if (stride_estimate == stride_small_bound)
			{
				stride_estimate += 1;
				dbglog("repeated on small bound, stop at %d\n", stride_estimate);
			}
			else
				dbglog("repeated, stop at %d\n", stride_estimate);
			break;
		}
		stride_estimate = new_estimate;
		int ncol_estimate = item_count / stride_estimate + (item_count % stride_estimate > 0);
		dbglog("%d < stride: %d < %d\ncolumns: %d\n", stride_small_bound, stride_estimate, stride_big_bound, ncol_estimate);
		//here 1 is added to mimic ls behavior:
		//leave at least one space between text and right edge of terminal
		int	table_width = separator_width * (ncol_estimate - 1) + 1 
						+ column_width_sum(items, item_count, stride_estimate);
		dbglog("table_width: %d\n", table_width);
		if (table_width < width_limit)
		{
			dbglog("too narrow. try more columns (smaller stride)\n");
			stride_big_bound = stride_estimate;
		}
		else if (table_width > width_limit)
		{
			dbglog("too wide. try less columns (bigger stride)\n");
			stride_small_bound = stride_estimate;
		}
		else
		{
			dbglog("exact.\n");
			break;
		}
	}
	int stride = stride_estimate;
	int ncol = item_count / stride + (item_count % stride > 0);
	dbglog("\n");
	dbglog("ncol: %d\n", ncol);
	dbglog("stride: %d\n", stride);
	*column_widths = malloc(ncol * sizeof(**column_widths));
	int i = 0;
	int start = 0;
	while (i < ncol)
	{
		int	stop = min(start + stride, item_count);
		(*column_widths)[i] = max_width_between(items, start, stop);
		start += stride;
		i++;
	}
	return stride;
}
