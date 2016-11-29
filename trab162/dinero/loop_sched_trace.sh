#/bin/bash

sched_trace=../sched/test_sched.trc.gz
output=~/cache/trab161/sched_traces/sched_trc
    for as in 1 2 ; do \
      for bl in 8 16; do \
        gzip -dc ${sched_trace} | \
        dinero -a${as} -b${bl} -i8k -d4k -u0 -wc -Aw -O${output}_${as}_${bl}
      done
    done
