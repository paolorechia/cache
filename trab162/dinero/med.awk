BEGIN { x = 1.0 ; y = 0.0 ; z = 0.0; }
//{ x = x * $1 ; printf "%.4f %.8f\n", $1, x ; }
END { y = log(x) / 6.0; z = exp(y) ; printf "%.4f\n", z ; }
