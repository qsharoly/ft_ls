#!/usr/bin/bash

EXEC="$1"
BASE=$(basename $EXEC)

set -v #echo commands
perf record -F 10000 -g $EXEC
perf script | ~/clones/FlameGraph/stackcollapse-perf.pl > out.perf-folded
~/clones/FlameGraph/flamegraph.pl out.perf-folded > "perf-$BASE.svg"
mv --backup=numbered "perf-$BASE.svg" /mnt/d/it
