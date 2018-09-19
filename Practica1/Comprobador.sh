#!/bin/sh

rm PorcentajeActual.txt
touch PorcentajeActual.txt
echo 'Mapa 30: ' >> PorcentajeActual.txt 
./BelkanSG ./mapas/mapa30.map >> PorcentajeActual.txt
echo '\nMapa 50: ' >> PorcentajeActual.txt 
./BelkanSG ./mapas/mapa50.map >> PorcentajeActual.txt
echo '\nMapa 100: ' >> PorcentajeActual.txt 
./BelkanSG ./mapas/mapa100.map >> PorcentajeActual.txt
echo '\nMapa 100n: ' >> PorcentajeActual.txt 
./BelkanSG ./mapas/mapa100n.map >> PorcentajeActual.txt
echo '\nMapa 100n2: ' >> PorcentajeActual.txt 
./BelkanSG ./mapas/mapa100n2.map >> PorcentajeActual.txt
