#!/bin/bash

# set -x

if [ $# -lt 2 ] ; then
   echo -e "\n\t$0 -p|-f|-l|-x|-j -t TITULO arquivo.dat\n\tARGS: $*\n"
   exit 1
fi

declare -a inp lbl
declare -i idx=0
ptSZ="1"

for P in "$@" ; do      # while true 

    case "$1" in
        -p|-e|-ps|-eps|-postscript)  # postscript
            outTerm='postscript eps enhanced color blacktext "Times-Roman" 18'
            ptSZ="1.4"
            outSfx="eps"
            lmargin="at screen -1"
            sz="0.625,0.650"
            ;;

        -f|-fig|-xfig)            # xfig  
            outTerm="fig color landscape metric fontsize 11"
            outSfx="fig"
            sz="1.0,1.0"
            ;;

       -l|-lat|-latex)           # latex
            outTerm="latex 10 size 6cm, 5cm"
            outSfx="tex"
            sz="1.0,1.0"
            ;;

#        -x|-x11|-terminal)        # na tela, X11
#            outTerm='x11 enhanced font "terminal-14" persist raise'
#            outSfx="null"
#            sz="1.0,1.0"
#            ;;

        -j|-jpg|-jpeg)            # jpg
            outTerm='jpeg large enhanced'
            outSfx="jpg"
            sz="1.0,1.0"
            ;;

        -t|-tit|-title)
            outTitle=$2
            shift;
            ;;

       *)  # input files
            inp[$idx]="$1"
            label="$(echo $1 | sed -e 's:_:-:g')"
            lbl[$idx]="$label"
            idx=$((idx+1))
            ;;
    esac

    shift;

done

TITLE=$(echo $outTitle | sed -e 's:_:-:g')

if [ "$outSfx" = "null" ] ; then
  outFile="/dev/null"
else
  outName=$(echo $outTitle | sed -e 's: :_:g')
  outFile="${outName}.${outSfx}"
  echo $outFile
fi

if [ "x$XRANGE" = "x" ] ; then
  XRANGE="[8:512]"        ## ELSE usa variavel do ambiente
fi


cat <<EOF0 | gnuplot
set encoding utf8
set xlabel "tamanho do bloco [bytes]"; set ylabel ""
set title "$TITLE"
#set xtics ("64" 64, "128" 128, "256" 256, "512" 512, "1k" 1024, "2k" 2048, "4k" 4096, "8k" 8192, "16k" 16384, "32k" 32768)
# set nozero
set logscale x
set xtics nomirror ("16" 16, "32" 32, "64" 64, "128" 128, "256" 256)
set ytics nomirror ; set tics in ; set nogrid
set xrange $XRANGE
set yrange [0:5]
set ylabel "% de todas as referencias"
set lmargin $lmargin 
set border 3 lw 0  ; set ytics nomirror
set pointsize $ptSZ
set size $sz
set term $outTerm
set output "$outFile"
set key top right      # {top,mid,bottom} {left,center,right}
set size 1.0,1.0
plot "blocs.dat" using 1:2 title "1K" with linesp lt 1 pt 3,\
     "blocs.dat" using 1:3 title "4K" with linesp lt 1 pt 2,\
     "blocs.dat" using 1:4 title "16K" with linesp lt 1 pt 3,\
     "blocs.dat" using 1:5 title "64K" with linesp lt 1 pt 4,\
     "blocs.dat" using 1:6 title "256K" with linesp lt 1 pt 5
EOF0


# plot \
# "${inp[0]}-4-1.cr" us 7:8 tit "${lbl[0]}" w li lt 2 lw 2 lc rgb "red",\
# "${inp[1]}-4-1.cr" us 7:8 tit "${lbl[1]}" w li lt 3 lw 2 lc rgb "black",\
# "${inp[2]}-4-1.cr" us 7:8 tit "${lbl[2]}" w li lt 2 lw 2 lc rgb "blue",\
# "${inp[3]}-4-1.cr" us 7:8 tit "${lbl[3]}" w li lt 1 lw 2 lc rgb "black",\
# "${inp[4]}-4-1.cr" us 7:8 tit "${lbl[4]}" w li lt 5,\
# "${inp[5]}-4-1.cr" us 7:8 tit "${lbl[5]}" w li lt 4,\
# "${inp[6]}-4-1.cr" us 7:8 tit "${lbl[6]}" w li lt 9

