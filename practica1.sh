#!/bin/bash

# Verificar que se hayan pasado 5 parámetros
if [ "$#" -ne 5 ]; then
    echo "Uso: $0 <semilla> <nivel> <fila> <columna> <orientacion>"
    exit 1
fi

# Asignar los parámetros a variables
semilla=$1
nivel=$2
fila=$3
columna=$4
orientacion=$5

# Lista de mapas a ejecutar
mapas=("cuadricula.map" "mapa100.map" "mapa50.map" "medieval.map" "picto.map" "luminaliaZA1.map" "mapa30.map" "mapa75.map" "paldea1.map" "vertigo.map")

# Archivo de log donde se guardarán los resultados
log_file="resultados_log.txt"

# Asegurarse de que el archivo de log esté vacío al comenzar
> $log_file

# Compilar el juego
echo "Compilando el juego..."
sudo make

# Ejecutar el juego para cada mapa con los parámetros especificados
for mapa in "${mapas[@]}"; do
    echo "Ejecutando el juego con el mapa $mapa" | tee -a $log_file
    ./practica1SG "./mapas/$mapa" $semilla $nivel $fila $columna $orientacion 2>&1 | tee -a $log_file
    echo "----------------------------------------------------" | tee -a $log_file
done

echo "Simulaciones completadas. Resultados guardados en $log_file y mostrados en pantalla."
