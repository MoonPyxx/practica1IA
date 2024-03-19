#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{	
	Action accion = actIDLE;
	if (current_state.tiene_bikini){
		cout << "Tiene bikini" << endl;
	} else if(current_state.tiene_zapatillas){
		cout << "Tiene zapatillas" << endl;
	
	} 
	cout << "Terreno frente: " << sensores.terreno[2] << endl;
	movimiento(accion);	
	// Si no está bien situado, actualiza la posición
	if (sensores.terreno[0] == 'G'){
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
	}
	if (!bien_situado){
		if (hayObstaculo(sensores)){
			if (rand()%2 == 0){
				accion = actTURN_L;
			} else {
				accion = actTURN_SR;
			}
		} else{

			accion = actWALK;
		}
			
	} else{
		mapTerreno(sensores.terreno, mapaResultado);
			// Comprobar que está dentro de mapa y que puede andar
	if (canWalk(sensores) && dentroMapa()){
 		accion = actWALK;
 		} else if (!girar_derecha){
			accion = actTURN_L;
			girar_derecha = (rand()%2 == 0);

		} else {
 		accion = actTURN_SR;
		girar_derecha = (rand()%2 == 0);
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

void ComportamientoJugador::reinicio(Sensores &sensores){

	// current_state.fil =  current_state.col = 1;
	current_state.brujula = norte;
	bien_situado = false;
	last_action = actIDLE;
}


// Comprobar si delante hay un obstaculo (muro o precipicio)
bool ComportamientoJugador::hayObstaculo(Sensores &sensores){
	char terreno_frente = sensores.terreno[2];
	return (terreno_frente == 'M' || terreno_frente == 'P');
}

// Comprobar si delante es transitable o tienes bikini/zapatillas
bool ComportamientoJugador::canWalk(Sensores &sensores){
	char terreno_frente = sensores.terreno[2];
    bool esTransitable = (terreno_frente == 'T' || terreno_frente == 'S' || terreno_frente == 'G' || terreno_frente == 'A' || terreno_frente == 'B');

	/* if (terreno_frente == 'A' && current_state.tiene_bikini) {
        esTransitable = true;
    } else if (terreno_frente == 'B' && current_state.tiene_zapatillas) {
        esTransitable = true;
    }
	*/

    return esTransitable && sensores.agentes[2] == '_';

}

// Detectar si hay un objeto (bikini, zapatillas o recarga)
bool ComportamientoJugador::detectarObjeto(Sensores &sensores){
	return (sensores.terreno[2] == 'D' || sensores.terreno[2] == 'X' || sensores.terreno[2] == 'K');
}

// Comprobar si está dentro del mapa para evitar segmentation error

bool ComportamientoJugador::dentroMapa(){
	return (current_state.fil >= 0 && current_state.fil < mapaResultado.size() &&
    current_state.col >= 0 && current_state.col < mapaResultado[0].size());
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
        		matriz[current_state.fil - i][current_state.col + j] = terreno[index++];
    	}
		}
		break;

		case sur:
		 index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
				 if (index == 9 || index == 11 || index == 12 || index == 13){
					if (terreno[index != '?']){
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
				 if (index == 9 || index == 11 || index == 12 || index == 13){
					if (terreno[index != '?']){
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
				 if (index == 9 || index == 11 || index == 12 || index == 13){
					if (terreno[index != '?']){
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
		matriz[current_state.fil - 2][current_state.col + 2] = terreno[6]; 
		matriz[current_state.fil - 1][current_state.col + 2] = terreno[7]; 
		matriz[current_state.fil][current_state.col + 2] = terreno[8];
		if (terreno[9]!= '?'){
			matriz[current_state.fil - 3][current_state.col] = terreno[9]; 
		}
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
		matriz[current_state.fil - 2][current_state.col - 2] = terreno[6]; 
		matriz[current_state.fil - 2][current_state.col - 1] = terreno[7]; 
		matriz[current_state.fil-2][current_state.col] = terreno[8]; 
		if (terreno[9]!= '?'){
		matriz[current_state.fil][current_state.col-3] = terreno[9]; 
		}
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
    	matriz[current_state.fil + 2][current_state.col + 2] = terreno[6]; 
    	matriz[current_state.fil + 2][current_state.col + 1] = terreno[7]; 
    	matriz[current_state.fil+2][current_state.col] = terreno[8]; 
		if (terreno[9]!= '?'){
    	matriz[current_state.fil][current_state.col+3] = terreno[9]; 
		}
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
		matriz[current_state.fil + 2][current_state.col - 2] = terreno[6]; 
		matriz[current_state.fil + 1][current_state.col - 2] = terreno[7]; 
		matriz[current_state.fil][current_state.col - 2] = terreno[8]; 
		if (terreno[9]!= '?'){
		matriz[current_state.fil + 3][current_state.col] = terreno[9]; 
		}
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
