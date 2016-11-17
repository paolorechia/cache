#!/bin/bash

# -i 0=frag 1=jpeg_enc 2=rtr 3=zip_dec 4=zip_enc

# set -x

if [ $# = "0" ] ; then
   echo -e "\n\t$0 -p|-f|-l|-x|-j -t title -i idx apl\n"
   echo -e "\t$0 -eps -t jpeg_enc_csz_blk jpeg_enc\n\n"
   exit 1
fi

declare -a inp lbl dataSet
declare -i idx=0

ptSZ="1"
where="top right"

for P in "$@" ; do    # while true ; do

    case "$1" in
	-p|-e|-ps|-eps|-postscript)  # postscript
	    outTerm='postscript eps enh color blacktext "Times-Roman" 18'
            ptSZ="1.4"
            ptSZ="1.2"
	    outSfx="eps"
	    xkeypos="3000"
            # lmargin="at screen -1"
	    sz="0.625,0.65"
	    sz="0.75,0.625"
	    ;;
	
	-f|-fig|-xfig)            # xfig  
	    outTerm="fig color landscape metric fontsize 11"
	    outSfx="fig"
	    xkeypos="2500"
	    sz="1.0,1.0"
	    ;;
	
	-l|-lat|-latex)           # latex
	    outTerm="latex 10 size 6cm, 5cm"
	    outSfx="tex"
	    xkeypos="2500"
	    sz="1.0,1.0"
	    ;;
	
	-x|-x11|-terminal)        # na tela, X11
	    outTerm='x11 enhanced font "terminal-14" persist raise'
	    outSfx="nil"
	    xkeypos="2500"
	    sz="1.0,1.0"
	    ;;
	
	-j|-jpg|-jpeg)            # jpg
	    outTerm='jpeg large enhanced'
	    outSfx="jpg"
	    xkeypos="2500"
	    sz="1.0,1.0"
	    ;;

	-t|-tit|-title)
            outTitle="$2"
            shift;
            ;;

	-i|-ind|-index)
            dataSet=$2
            shift;
            ;;

        *)  # input data file
	    input="$1"
            # inp[$idx]="$1"
            # label="$(echo ${1%.d} | sed -e 's:_:-:g')"
            # lbl[$idx]="$label"
            # idx=$((idx+1))
	    break
            ;;
    esac
    
    shift; # echo $1 $input $out
    
done

apl=${input}

TITLE=$(echo $outTitle | sed -e 's:_:-:g')

if [ "$outSfx" = "null" ] ; then
  outFile="/dev/null"
else
  outName=$(echo $outTitle | sed -e 's: :_:g')
  outFile="${outName}.${outSfx}"
  echo $outFile
fi

if [ "x$YRANGE" = "x" ] ; then
  YRANGE="[0:0.1]"        ## ELSE usa variavel do ambiente
fi

if [ "x$XRANGE" = "x" ] ; then
  # XRANGE="[1.5:80]"        ## ELSE usa variavel do ambiente
  XRANGE="[3:20]"        ## ELSE usa variavel do ambiente
fi

idx0=$((dataSet))
idx1=$((dataSet+1))
idx2=$((dataSet+2))
idx3=$((dataSet+3))
idx4=$((dataSet+4))


cat <<EOF0 | gnuplot
set encoding utf8
set title ""  # "$TITLE"
set xlabel "capacity"
set ylabel "miss rate"
set lmargin $lmargin # set rmargin $rmargin
set yrange $YRANGE
set logscale x
set xrange $XRANGE
# set xtics ("1k" 1, "2k" 2, "4k" 4, "8k" 8, "16k" 16, "32k" 32, "64k" 64)
set xtics ("4k" 4, "8k" 8, "16k" 16)
set tics in ; set grid noxtics ytics;
set key samplen 2
set border 3 lw 0 
set pointsize $ptSZ
set size $sz
set term $outTerm
set output "$outFile"
plot \
 "${apl}_A1B32WcAw.dat" us 1:2 tit "8 wds" w lp lt 1 pt 9 lc rgb "#660088",\
 "${apl}_A1B64WcAw.dat" us 1:2 tit "16 wds" w lp lt 1 pt 9 lc rgb "#008888",\
 "${apl}_A1B128WcAw.dat" us 1:2 tit "32 wds" w lp lt 1 pt 9 lc rgb "#0000ff"
EOF0
# echo $outFile

# -i 0=frag 1=jpeg_enc 2=rtr 3=zip_dec 4=zip_enc

# "${apl}_A1B4WwAn.dat" us 1:2 tit "1 wd" w lp lt 1 pt 1 lc rgb "#000000",\
# "${apl}_A1B8WwAn.dat" us 1:2 tit "2 wds" w lp lt 1 pt 2 lc rgb "#ff0000",\
# "${apl}_A1B16WwAn.dat" us 1:2 tit "4 wds" w lp lt 1 pt 7 lc rgb "#444444",\