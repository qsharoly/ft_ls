#!/usr/bin/bash

EXEC="$@"
BASE=$(basename $1)

set -x #echo expanded commands
perf record -F 10000 -g $EXEC
perf script | ~/clones/FlameGraph/stackcollapse-perf.pl > out.perf-folded
~/clones/FlameGraph/flamegraph.pl out.perf-folded > "perf-$BASE.svg"
mv --backup=numbered "perf-$BASE.svg" /mnt/d/it
