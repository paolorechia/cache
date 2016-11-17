#/bin/bash

# set -x

for TRC in all ; do
  for csz in 4k 8k 16k ; do # 1k 2k 4k 8k 16k 32k 64k 128k 256k ; do
    # associativity
    for as in 1 2 ; do # 1 2 4 8 ; do \
      # cache sub-block size
      for bl in 32 64 128 ; do # 4 8 16 32 64 128 256 ; do \
        # write policy [c=copy-back | w=writethrough]
        for wp in c ; do 
          # write-miss policy [w=allocate | n=no-allocate]
          if [ ${wp} = "w" ] ; then ap=n; else ap=w ; fi
	  if [ ! -s ${TRC}_${csz}A${as}B${bl}W${wp}A${ap} ] ; then
            echo ${TRC}_${csz}A${as}B${bl}WP${wp}AP${ap}
	    sched -p'gzip -dc TRC/frag.trc.gz'     \
		  -p'gzip -dc TRC/jpeg_enc.trc.gz' \
		  -p'gzip -dc TRC/zip_dec.trc.gz'  \
                  -p'gzip -dc TRC/zip_enc.trc.gz'  \
                  -q10000 -m999000000 -c -r -f -v |\
	    dinero -a${as} -b${bl} -i8k -d${csz} -u0 -w${wp} -A${ap} \
                   -z999999999 -O${TRC}_${csz}A${as}B${bl}W${wp}A${ap}
          fi
        done
      done
    done 
  done
done
