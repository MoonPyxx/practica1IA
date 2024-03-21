#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{	
	Action accion = actIDLE;

	añadirObjeto(sensores);
	if(estaAtrapado(sensores)){
		atrapado = true;
		salirAtrapado(sensores);
	}
	movimiento(accion);	
	// Si no está bien situado, actualiza la posición
	detectarPosicionamiento(sensores);
	if (sensores.reset){
		reinicio(sensores);
	}
	if (recargar(sensores)){
		accion = actIDLE;
	}
	if (atrapado){
		accion = salirAtrapado(sensores);
	} else{
		if (hayObstaculo(sensores)){
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
			
	} if(bien_situado)
		mapTerreno(sensores.terreno, mapaResultado);

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

void ComportamientoJugador::reinicio(Sensores &sensores){

	current_state.fil =  current_state.col = tam_mapa;
	current_state.brujula = norte;
	bien_situado = false;
	tiene_bikini = tiene_zapatillas = false;
	last_action = actIDLE;
}
bool ComportamientoJugador::recargar(Sensores &sensores){
	return (sensores.terreno[0] == 'X' && sensores.bateria < 4000);
}
// Comprobar si delante hay un obstaculo (muro o precipicio)
bool ComportamientoJugador::hayObstaculo(Sensores &sensores){
	char terreno_frente = sensores.terreno[2];
	return (terreno_frente == 'M' || terreno_frente == 'P'|| (terreno_frente == 'B' && !tiene_zapatillas) || (terreno_frente == 'A' && !tiene_bikini));
}
Action ComportamientoJugador::salirAtrapado(Sensores &sensores){	
	if (primer_paso){
		primer_paso = false;
		segundo_paso = true;
		cout << "primer paso detectado" << endl;
		return actWALK;
	} else if(segundo_paso){
		cout << "segundo paso detectado" << endl;
		segundo_paso = false;
		tercer_paso = true;
		return actWALK;
	} else if(tercer_paso){
		cout << "tercer paso detectado" << endl;
		tercer_paso = false;
		cuarto_paso = true;
		return actWALK;
	} else if(cuarto_paso){
		cout << "cuarto paso detectado" << endl;
		cuarto_paso = false;
		quinto_paso = true;
		return actTURN_SR;
	} else if(quinto_paso){
		cout << "quinto paso detectado" << endl;
		quinto_paso = false;
		primer_paso = true;
		atrapado = false;
		return actTURN_SR;
	} else {
		cout << "Error en la salida del atrapado" << endl;
		return actIDLE;
	}

}

bool ComportamientoJugador::estaAtrapado(Sensores &sensores){
	if (sensores.terreno[0] == 'S' && sensores.terreno[1] == 'S' &&sensores.terreno[2] == 'S' && sensores.terreno[3] == 'M' &&
	 sensores.terreno[5] == 'S' && sensores.terreno[6] == 'S' && 
	 sensores.terreno[7] == 'S' && sensores.terreno[11] == 'S' && sensores.terreno[12] == 'S' &&
	   sensores.terreno[13] == 'M'){
		return true;
	} else {
		return false;
	}
}
void ComportamientoJugador::detectarPosicionamiento(Sensores &sensores){
	if (sensores.terreno[0] == 'G'){
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
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


// Calcular movimiento (actWALK, actRUN)
void ComportamientoJugador::movimiento(Action accion){
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
// Mapear el terreno en maparesultado (a medias)
void ComportamientoJugador::mapTerreno(const vector<unsigned char> &terreno, vector < vector < unsigned char > > &matriz){
	matriz[current_state.fil][current_state.col] = terreno[0];
	int index = 1;
	switch(current_state.brujula){
		case norte:
		 index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
				if (index == 6 || index == 11 || index == 12 || index == 13){
					if(terreno[index] != '?'){
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
				 if (index == 6 || index == 11 || index == 12 || index == 13){
					if (terreno[index] != '?'){
						matriz[current_state.fil + i][current_state.col - j] = terreno[index++];
					}
				 } else {
						matriz[current_state.fil + i][current_state.col - j] = terreno[index++];
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
