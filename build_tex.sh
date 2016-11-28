#/bin/bash

cat << DOCUMENT0
\documentclass{article}
\usepackage[utf8]{inputenc}
\usepackage{graphicx}
\usepackage{geometry}
 \geometry{
 a4paper,
 total={170mm,257mm},
 left=20mm,
 top=20mm,
 }
\usepackage[export]{adjustbox}


\title{Graficos - cache}
\author{paolorechia }
\date{November 2016}

\begin{document}

\setlength{\parindent}{0em}
DOCUMENT0
for each in $(ls | grep txf); do
    printf "\includegraphics[scale=0.5]{${each}.png}\n"
done

echo "\\end{document}"
