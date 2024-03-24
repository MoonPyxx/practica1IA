#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
#include <queue>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{	
	Action accion = actIDLE;
	añadirObjeto(sensores);
	estaAtrapado(sensores);
	detectarObjetos(sensores);
	movimiento(accion, sensores);	
	// Si no está bien situado, actualiza la posición
	detectarPosicionamiento(sensores);
	if (sensores.reset){
		reinicio(sensores);
	}
	if (recargar(sensores)){
		acciones_pendientes.push(actIDLE);
	}
	if (!acciones_pendientes.empty()){
		accion = acciones_pendientes.front();
		acciones_pendientes.pop();
	} else{
		if (hayObstaculo(sensores) || hayEntidades(sensores)){
			if (rand()%2 == 0){
				accion = actTURN_L;
			} else {
				accion = actTURN_SR;
			}
		} else{
			if (rand()%10==0){
				accion = actTURN_L;
			} else if(rand()%12==0) {
				accion = actTURN_SR;
			}else {
				accion = actWALK;

		}
		}
		// MAPAS (DEBUG)
		/*
		cout << "MAPA AUXILIAR: " << endl;
		for (int i = 0; i < tam_mapa*2; ++i) {
        for (int j = 0; j < tam_mapa*2; ++j) {
            std::cout << mapaAuxiliar[i][j] << " ";
        }
        cout << endl;
		    }
		cout << endl;

		cout << "MAPA RESULTADO: " << endl;
		for (int i = 0; i < tam_mapa; ++i) {
		for (int j = 0; j < tam_mapa; ++j) {
			std::cout << mapaResultado[i][j] << " ";
		}
		cout << endl;
		
		    }
			*/
		if (sensores.nivel==0){
			current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
			mapTerreno(sensores.terreno, mapaResultado);
		} else {
	if (!bien_situado && usar_mapa){
		mapTerreno(sensores.terreno, mapaAuxiliar);
		
	} 
	if (bien_situado){
		mapTerreno(sensores.terreno, mapaResultado);
	
	}
	}
	}

	// Recordar la ultima accion
	last_action = accion;
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

// Funciones mias

// Reiniciar
void ComportamientoJugador::limpiarCola(){
	while (!acciones_pendientes.empty()){
		acciones_pendientes.pop();
	}
}
void ComportamientoJugador::borrarMapaAuxiliar(){
	for (int i = 0; i < tam_mapa*2; i++) { 
		for (int j = 0; j < tam_mapa*2; j++) {
			mapaAuxiliar[i][j] = '?';
		}
	}
}

void ComportamientoJugador::reinicio(Sensores &sensores){
	cout << "Se ha producido un reinicio " << endl;
	current_state.fil =  current_state.col = tam_mapa;
	current_state.brujula = norte;
	bien_situado = false;
	tiene_bikini = tiene_zapatillas = false;
	last_action = actIDLE;
	limpiarCola();
	if (usar_mapa)
	borrarMapaAuxiliar();
}
bool ComportamientoJugador::recargar(Sensores &sensores){
	return (sensores.terreno[0] == 'X' && sensores.bateria < sensores.vida);
}
// Comprobar si delante hay un obstaculo (muro o precipicio)
bool ComportamientoJugador::hayObstaculo(Sensores &sensores){
	char terreno_frente = sensores.terreno[2];
	bool pocaBateria = sensores.bateria < 3000;
if (terreno_frente == 'M' || terreno_frente == 'P') {
    return true; 
} else if ((terreno_frente == 'B' && !tiene_zapatillas) || (terreno_frente == 'A' && !tiene_bikini)) {
    return !pocaBateria; 
} else {
    return false; 
}
}
bool ComportamientoJugador::hayEntidades(Sensores &sensores){
	for (int i = 1; i <= 3; i++) {
        if (sensores.agentes[i] == 'a' || sensores.agentes[i] == 'l') {
            return true;
        }
    }
    return false;
}
	void ComportamientoJugador::estaAtrapado(Sensores &sensores){
		if (sensores.terreno[0] == 'S' && sensores.terreno[1] == 'S' &&sensores.terreno[2] == 'S' && sensores.terreno[3] == 'M' &&
		sensores.terreno[5] == 'S' && sensores.terreno[6] == 'S' && 
		sensores.terreno[7] == 'S' && sensores.terreno[11] == 'S' && sensores.terreno[12] == 'S' &&
		sensores.terreno[13] == 'M'){
		acciones_pendientes.push(actWALK); 
  		acciones_pendientes.push(actWALK); 
  		acciones_pendientes.push(actTURN_SR); 
  		acciones_pendientes.push(actTURN_SR); 
		acciones_pendientes.push(actWALK);
		}
	}

void ComportamientoJugador::detectarPosicionamiento(Sensores &sensores){
	if (sensores.terreno[0] == 'G' && !bien_situado){
		int fil = current_state.fil - sensores.posF;
		int col = current_state.col - sensores.posC;
		if (usar_mapa){
			if (sensores.nivel == 3)
		{
			reorientarMapa(sensores);
		}
		actualizarMapaConAuxiliar(fil, col);
		}
		
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		
	bien_situado = true;
	limpiarCola();
	}
}

void ComportamientoJugador::rotarMapa(vector<vector<unsigned char>>& map) {
    int N = map.size();
	// Trasponer matriz
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; ++j) {
            swap(map[i][j], map[j][i]);
        }
    }

	// Invertir filas
	for (int i = 0; i< N/2 ; i++){
		for (int j = 0; j<N; j++){
			swap (map[i][j], map[N-i-1][j]);
		}
	}


}
void ComportamientoJugador::reorientarMapa(Sensores &sensores){
	// Calcular la orientación real basada en el cambio de orientación
	int deltaOrientacion = (current_state.brujula - sensores.sentido + 8 ) % 4;

	cout << "Delta orientacion: " << deltaOrientacion << endl;
	Orientacion orientacionReal = static_cast<Orientacion>(deltaOrientacion);


	switch (orientacionReal){
		case oeste:
			rotarMapa(mapaAuxiliar);
			break;
		case sur:
			rotarMapa(mapaAuxiliar);
			rotarMapa(mapaAuxiliar);
			break;
		case este:	
		cout << "Este" << endl;
			rotarMapa(mapaAuxiliar);
			rotarMapa(mapaAuxiliar);
			rotarMapa(mapaAuxiliar);
			cout << "Rotado al este" << endl;
			break;

			default:
			break;
	}
	
}

// Comprobar si delante es transitable o tienes bikini/zapatillas

void ComportamientoJugador::añadirObjeto(Sensores &sensores){
	char terreno_actual = sensores.terreno[0];
	if (terreno_actual == 'D'){
		tiene_zapatillas = true;
	} else if(terreno_actual == 'K'){
		tiene_bikini = true;
	}
}


void ComportamientoJugador::actualizarMapaConAuxiliar(int fil, int col){

	 for (int i = 0; i < mapaResultado.size(); i++) {
        for (int j = 0; j < mapaResultado.size(); j++) {
			if (mapaResultado[i][j] == '?' && dentroDeMapa(fil+i, col+j, tam_mapa*2, tam_mapa*2)){
				mapaResultado[i][j] = mapaAuxiliar[fil+i][col+j];
			}
        }
    }
	borrarMapaAuxiliar();
	
}
bool ComportamientoJugador::dentroDeMapa(int fil, int col, int filasMax, int columnasMax){
	return (fil >= 0 && fil < filasMax && col >= 0 && col < columnasMax);
}
// Calcular movimiento (actWALK, actRUN)
void ComportamientoJugador::movimiento(Action accion, Sensores &sensores){
	if (!sensores.colision){
	int a = current_state.brujula;
	switch(last_action){
		case actWALK:
		   switch(current_state.brujula){
				case norte: current_state.fil--; break;
				case noreste: current_state.fil--; current_state.col++; break;
				case este: current_state.col++; break;
				case sureste: current_state.fil++; current_state.col++; break;
				case sur: current_state.fil++; break;
				case suroeste: current_state.fil++; current_state.col--; break;
				case oeste: current_state.col--; break;
				case noroeste: current_state.fil--; current_state.col--; break;
		   }
		break;
	case actRUN:
	switch(current_state.brujula){
				case norte: current_state.fil-=2; break;
				case noreste: current_state.fil-=2; current_state.col+=2; break;
				case este: current_state.col+=2; break;
				case sureste: current_state.fil+=2; current_state.col+=2; break;
				case sur: current_state.fil+=2; break;
				case suroeste: current_state.fil+=2; current_state.col-=2; break;
				case oeste: current_state.col-=2; break;
				case noroeste: current_state.fil-=2; current_state.col-=2; break;
		   }
		   break;
 		// Actualizacion en caso de correr (45 grados)
		case actTURN_SR:
			a = (a+1)%8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
		case actTURN_L:
		// Actualizacion de girar 90 a la izquierda
		a = (a+6)%8;
		current_state.brujula = static_cast<Orientacion>(a);
		break;
	}
	}
}
void ComportamientoJugador::detectarObjetos(Sensores &sensores){
	int casillas_sensor = 15;
	int nivel_bateria = sensores.vida;
	bool muro = false;
	for (int i = 0; i<= 15; i++){
		if (sensores.terreno[i]== 'M'){
			muro = true;
		}
	}
	if (acciones_pendientes.empty() && !muro){
		
		if (current_state.brujula == norte || current_state.brujula == sur || current_state.brujula == este || current_state.brujula == oeste){ 
		  if ((sensores.terreno[1] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[1] == 'K' && !tiene_bikini) || (sensores.terreno[1] == 'D' && !tiene_zapatillas) || (sensores.terreno[1] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 1" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_L);
			acciones_pendientes.push(actWALK);
		  } else if((sensores.terreno[2] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[2] == 'K' && !tiene_bikini) || (sensores.terreno[2] == 'D' && !tiene_zapatillas) || (sensores.terreno[2] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 2" << endl;
			acciones_pendientes.push(actWALK);
		} 	else if((sensores.terreno[3] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[3] == 'K' && !tiene_bikini) || (sensores.terreno[3] == 'D' && !tiene_zapatillas) || (sensores.terreno[3] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 3" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actWALK);
		} else if((sensores.terreno[4] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[4] == 'K' && !tiene_bikini) || (sensores.terreno[4] == 'D' && !tiene_zapatillas) || (sensores.terreno[4] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 4" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_L);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
		} else if((sensores.terreno[5] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[5] == 'K' && !tiene_bikini) || (sensores.terreno[5] == 'D' && !tiene_zapatillas) || (sensores.terreno[5] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 5" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_L);
			acciones_pendientes.push(actWALK);
		}
		else if((sensores.terreno[6] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[6] == 'K' && !tiene_bikini) || (sensores.terreno[6] == 'D' && !tiene_zapatillas) || (sensores.terreno[6] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 6" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
		}
		else if((sensores.terreno[7] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[7] == 'K' && !tiene_bikini) || (sensores.terreno[7] == 'D' && !tiene_zapatillas) || (sensores.terreno[7] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 7" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actWALK);
		}
		else if((sensores.terreno[8] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[8] == 'K' && !tiene_bikini) || (sensores.terreno[8] == 'D' && !tiene_zapatillas) || (sensores.terreno[8] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 8" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
		} else if((sensores.terreno[9] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[9] == 'K' && !tiene_bikini) || (sensores.terreno[9] == 'D' && !tiene_zapatillas) || (sensores.terreno[9] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 9" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_L);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
		} else if((sensores.terreno[10] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[10] == 'K' && !tiene_bikini) || (sensores.terreno[10] == 'D' && !tiene_zapatillas) || (sensores.terreno[10] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 10" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_L);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
		}  else if((sensores.terreno[11] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[11] == 'K' && !tiene_bikini) || (sensores.terreno[11] == 'D' && !tiene_zapatillas) || (sensores.terreno[11] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 11" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_L);
			acciones_pendientes.push(actWALK);
		} else if((sensores.terreno[12] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[12] == 'K' && !tiene_bikini) || (sensores.terreno[12] == 'D' && !tiene_zapatillas) || (sensores.terreno[12] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 12" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
		} else if((sensores.terreno[13] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[13] == 'K' && !tiene_bikini) || (sensores.terreno[13] == 'D' && !tiene_zapatillas) || (sensores.terreno[13] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 13" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actWALK);
		} else if((sensores.terreno[14] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[14] == 'K' && !tiene_bikini) || (sensores.terreno[14] == 'D' && !tiene_zapatillas) || (sensores.terreno[14] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 14" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
		} else if((sensores.terreno[15] == 'X' && sensores.bateria < nivel_bateria)|| (sensores.terreno[15] == 'K' && !tiene_bikini) || (sensores.terreno[15] == 'D' && !tiene_zapatillas) || (sensores.terreno[15] == 'G' && !bien_situado)) {
			cout << "Objeto detectado en la casilla 15" << endl;
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actTURN_SR);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
			acciones_pendientes.push(actWALK);
		} 
	}
}
}
// Mapear el terreno en maparesultado (a medias)
void ComportamientoJugador::mapTerreno(const vector<unsigned char> &terreno, vector < vector < unsigned char > > &matriz){
	matriz[current_state.fil][current_state.col] = terreno[0];
	int index = 1;
	switch(current_state.brujula){
		case norte:
		 index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
				 if (dentroDeMapa(current_state.fil-i, current_state.col + j, matriz.size(), matriz[0].size())) {
				if (index == 6 || index == 	11 || index == 12 || index == 13){
					if(terreno[index] != '?'){
						matriz[current_state.fil - i][current_state.col + j] = terreno[index++];
					}
					} else{
						matriz[current_state.fil - i][current_state.col + j] = terreno[index++];
					}
				}
			}
    	}
		break;

		case sur:
		 index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
				 if (dentroDeMapa(current_state.fil+i, current_state.col - j, matriz.size(), matriz[0].size())) {
				if (index == 6 || index == 	11 || index == 12 || index == 13){
					if(terreno[index] != '?'){
						matriz[current_state.fil + i][current_state.col - j] = terreno[index++];
					}
					} else{
						matriz[current_state.fil + i][current_state.col - j] = terreno[index++];
					}
				}
			}
    	}
		break;

		case este:
			index = 1; 
			for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
				 if (index == 6 || index == 11 || index == 12 || index == 13){
					if (terreno[index]!= '?'){
						matriz[current_state.fil + j][current_state.col + i] = terreno[index++];
					}
				 } else {
						matriz[current_state.fil + j][current_state.col + i] = terreno[index++];
		 			}
    			}
			}
		break;

		case oeste:
		index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
				 if (index == 6 || index == 11 || index == 12 || index == 13){
					if (terreno[index] != '?'){
						matriz[current_state.fil - j][current_state.col - i] = terreno[index++];
					}
				 } else {
						matriz[current_state.fil - j][current_state.col - i] = terreno[index++];
		 			}
    			}
			}
		break;

		case noreste:
		matriz[current_state.fil - 1][current_state.col] = terreno[1];
		matriz[current_state.fil - 1][current_state.col + 1] = terreno[2];
		matriz[current_state.fil][current_state.col + 1] = terreno[3]; 
		matriz[current_state.fil - 2][current_state.col] = terreno[4]; 
		matriz[current_state.fil - 2][current_state.col + 1] = terreno[5]; 
		if (terreno[6]!= '?'){
		matriz[current_state.fil - 2][current_state.col + 2] = terreno[6]; 
		}
		matriz[current_state.fil - 1][current_state.col + 2] = terreno[7]; 
		matriz[current_state.fil][current_state.col + 2] = terreno[8];
		matriz[current_state.fil - 3][current_state.col] = terreno[9]; 
		matriz[current_state.fil - 3][current_state.col + 1] = terreno[10]; 
		if (terreno[11]!= '?'){
			matriz[current_state.fil - 3][current_state.col + 2] = terreno[11]; 
		}
		if (terreno[12]!= '?'){
			matriz[current_state.fil - 3][current_state.col + 3] = terreno[12]; 
		}
		if (terreno[13]!= '?'){
			matriz[current_state.fil - 2][current_state.col + 3] = terreno[13]; 
		}
		
		matriz[current_state.fil - 1][current_state.col + 3] = terreno[14]; 
		matriz[current_state.fil][current_state.col + 3] = terreno[15];
		break;
		
		 case noroeste: 
		matriz[current_state.fil][current_state.col-1] = terreno[1];
		matriz[current_state.fil - 1][current_state.col - 1] = terreno[2];
		matriz[current_state.fil-1][current_state.col] = terreno[3]; 
		matriz[current_state.fil ][current_state.col-2] = terreno[4]; 
		matriz[current_state.fil - 1][current_state.col - 2] = terreno[5];
		if(terreno[6]!= '?'){ 
		matriz[current_state.fil - 2][current_state.col - 2] = terreno[6]; 
		}
		matriz[current_state.fil - 2][current_state.col - 1] = terreno[7]; 
		matriz[current_state.fil-2][current_state.col] = terreno[8]; 
		matriz[current_state.fil][current_state.col-3] = terreno[9]; 
		matriz[current_state.fil - 1][current_state.col - 3] = terreno[10]; 
		if (terreno[11]!= '?'){
		matriz[current_state.fil - 2][current_state.col - 3] = terreno[11]; 
		}
		if (terreno[12]!= '?'){
		matriz[current_state.fil - 3][current_state.col - 3] = terreno[12]; 
		}
		if (terreno[13]!= '?'){
		matriz[current_state.fil - 3][current_state.col - 2] = terreno[13]; 
		}
		matriz[current_state.fil - 3][current_state.col - 1] = terreno[14]; 
		matriz[current_state.fil-3][current_state.col] = terreno[15];
		break;
		
		case sureste:
		matriz[current_state.fil][current_state.col+1] = terreno[1];
    	matriz[current_state.fil + 1][current_state.col + 1] = terreno[2];
    	matriz[current_state.fil+1][current_state.col ] = terreno[3]; 
    	matriz[current_state.fil][current_state.col+2] = terreno[4]; 
    	matriz[current_state.fil + 1][current_state.col + 2] = terreno[5]; 
		if (terreno[6]!= '?'){
    	matriz[current_state.fil + 2][current_state.col + 2] = terreno[6]; 
		}
    	matriz[current_state.fil + 2][current_state.col + 1] = terreno[7]; 
    	matriz[current_state.fil+2][current_state.col] = terreno[8]; 
    	matriz[current_state.fil][current_state.col+3] = terreno[9]; 
    	matriz[current_state.fil + 1][current_state.col + 3] = terreno[10];
		if (terreno[11]!= '?'){ 
    	matriz[current_state.fil + 2][current_state.col + 3] = terreno[11];
		} 
		if (terreno[12]!= '?'){
    	matriz[current_state.fil + 3][current_state.col + 3] = terreno[12];
		}
		if (terreno[13]!= '?'){ 
    	matriz[current_state.fil + 3][current_state.col + 2] = terreno[13]; 
		}
    	matriz[current_state.fil + 3][current_state.col + 1] = terreno[14]; 
    	matriz[current_state.fil+3][current_state.col] = terreno[15];
    break;
		
		 case suroeste:
		matriz[current_state.fil+1][current_state.col] = terreno[1];
		matriz[current_state.fil + 1][current_state.col - 1] = terreno[2];
		matriz[current_state.fil][current_state.col - 1] = terreno[3]; 
		matriz[current_state.fil + 2][current_state.col] = terreno[4]; 
		matriz[current_state.fil + 2][current_state.col - 1] = terreno[5];
		if (terreno[6]!= '?'){ 
		matriz[current_state.fil + 2][current_state.col - 2] = terreno[6]; 
		}
		matriz[current_state.fil + 1][current_state.col - 2] = terreno[7]; 
		matriz[current_state.fil][current_state.col - 2] = terreno[8]; 
		matriz[current_state.fil + 3][current_state.col] = terreno[9]; 
		matriz[current_state.fil + 3][current_state.col - 1] = terreno[10]; 
		if (terreno[11]!= '?'){
		matriz[current_state.fil + 3][current_state.col - 2] = terreno[11]; 
		}
		if (terreno[12]!= '?'){
		matriz[current_state.fil + 3][current_state.col - 3] = terreno[12]; 
		}
		if (terreno[13]!= '?'){
		matriz[current_state.fil + 2][current_state.col - 3] = terreno[13]; 
		}
		matriz[current_state.fil + 1][current_state.col - 3] = terreno[14]; 
		matriz[current_state.fil][current_state.col - 3] = terreno[15];
		break;
}
}