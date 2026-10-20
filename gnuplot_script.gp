set terminal pdfcairo
set output 'E_data.pdf'
set title 'Energy over distance'
set xlabel 'Depth [um]'
set ylabel 'Energy [eV]'
plot 'E_data.txt' using 1:2 with lines title 'Data'
