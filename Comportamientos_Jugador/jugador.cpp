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
    detectarPosicionamiento(sensores);

    if (sensores.reset) {
        reinicio(sensores);
    }

    if (recargar(sensores)) {
        acciones_pendientes.push(actIDLE);
    }

    if (!acciones_pendientes.empty()) {
        accion = acciones_pendientes.front();
        if (accion == actWALK && sensores.terreno[2] == 'P') {
            accion = actTURN_SR;
        }
		
        acciones_pendientes.pop();
    } else {
        if (acciones_pendientes.empty()) {
            if (hayObstaculo(sensores) || hayEntidades(sensores)) {
                if (rand() % 2 == 0) {
                    accion = actTURN_L;
                } else {
                    accion = actTURN_SR;
                }
            } else {
				if (rand () %15 == 0){
					accion = actTURN_SR;
				} else {
						accion = actWALK;
					}	
				}
            }
        }
		if (sensores.nivel == 0) {
		bien_situado = true;
        current_state.fil = sensores.posF;
        current_state.col = sensores.posC;
        current_state.brujula = sensores.sentido;
        mapTerreno(sensores.terreno, mapaResultado);
    } else {
        if (!bien_situado) {
            mapTerreno(sensores.terreno, mapaAuxiliar);
        }
        if (bien_situado) {
            mapTerreno(sensores.terreno, mapaAuxiliar);
            mapTerreno(sensores.terreno, mapaResultado);
        }
    }
	last_action = accion;
	return accion;
}

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

bool ComportamientoJugador::puedeCorrer(Sensores &sensores){
    if (sensores.terreno[6] == TERRENO_MURO || sensores.terreno[6] == TERRENO_PRECIPICIO ||
        (sensores.terreno[6] == TERRENO_BOSQUE && !tiene_zapatillas) ||
        (sensores.terreno[6] == TERRENO_AGUA && !tiene_bikini) ||
        sensores.agentes[6] == 'a' || sensores.agentes[6] == 'l') {
        return false;
    }
    return true;
}

void ComportamientoJugador::reinicio(Sensores &sensores){
	cout << "Se ha producido un reinicio " << endl;
	current_state.fil =  current_state.col = tam_mapa;
	current_state.brujula = norte;
	bien_situado = false;
	tiene_bikini = tiene_zapatillas = false;
	last_action = actIDLE;
	limpiarCola();
	borrarMapaAuxiliar();
}
bool ComportamientoJugador::recargar(Sensores &sensores){
	return (sensores.terreno[0] == OBJETO_RECARGA && sensores.bateria < sensores.vida);
}
// Comprobar si delante hay un obstaculo (muro o precipicio)
bool ComportamientoJugador::hayObstaculo(Sensores &sensores){
	char terreno_frente = sensores.terreno[2];
	bool pocaBateria = sensores.bateria < 3000;
		if (terreno_frente == TERRENO_MURO || terreno_frente == TERRENO_PRECIPICIO) {
    		return true; 
		} else if ((terreno_frente == TERRENO_BOSQUE && !tiene_zapatillas) || (terreno_frente == TERRENO_AGUA && !tiene_bikini)) {
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
		if (sensores.terreno[0] == 'S' && sensores.terreno[1] == 'S' &&sensores.terreno[2] == 'S' && sensores.terreno[3] == TERRENO_MURO &&
		sensores.terreno[5] == 'S' && sensores.terreno[6] == 'S' && 
		sensores.terreno[7] == 'S' && sensores.terreno[11] == 'S' && sensores.terreno[12] == 'S' &&
		sensores.terreno[13] == TERRENO_MURO){
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
			if (sensores.nivel == 3)
			{
				reorientarMapa(sensores);
			}
		actualizarMapaConAuxiliar(fil, col);

		
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		
	bien_situado = true;
	limpiarCola();
	}
}

template <typename T>
void ComportamientoJugador::rotarMapa(vector<vector<T>>& map) {
    int N = map.size();
    // Trasponer matriz
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; ++j) {
            swap(map[i][j], map[j][i]);
        }
    }

    // Invertir filas
    for (int i = 0; i < N / 2; i++) {
        for (int j = 0; j < N; j++) {
            swap(map[i][j], map[N-i-1][j]);
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
	if (terreno_actual == OBJETO_ZAPATILLAS){
		tiene_zapatillas = true;
	} else if(terreno_actual == OBJETO_BIKINI){
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
 		// Actualizacion en caso de girar a la derecha (45 grados)
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
	for (int i = 0; i<= casillas_sensor; i++){
		if (sensores.terreno[i]== TERRENO_MURO){
			muro = true;
		}
	}
	if (acciones_pendientes.empty() && !muro){
		for (int j= 0; j<=casillas_sensor; j++){
			if (((sensores.terreno[j] == OBJETO_RECARGA && sensores.bateria < nivel_bateria) || 
    		(sensores.terreno[j] == OBJETO_BIKINI && !tiene_bikini) || 
    		(sensores.terreno[j] == OBJETO_ZAPATILLAS && !tiene_zapatillas) || 
    		(sensores.terreno[j] == 'G' && !bien_situado))) {
				accionPorCasilla(sensores, j);
			}
		}
	}
}
void ComportamientoJugador::orientarJugador(Sensores &sensores, Orientacion orientacion){
	switch (orientacion){
		case noreste:
		acciones_pendientes.push(actTURN_SR);
		acciones_pendientes.push(actTURN_L);
		break;
		case sureste:
		acciones_pendientes.push(actTURN_SR);
		acciones_pendientes.push(actTURN_L);
		break;
		case noroeste:
		acciones_pendientes.push(actTURN_SR);
		break;
		case suroeste:
		acciones_pendientes.push(actTURN_SR);
		break;
	}
}
void ComportamientoJugador::accionPorCasilla(Sensores &sensores, int casilla){
	switch (casilla) {
            case 1:
                acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
                acciones_pendientes.push(actTURN_L);
                acciones_pendientes.push(actWALK);
                break;
            case 2:
                acciones_pendientes.push(actWALK);
                break;
            case 3:
                acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
                acciones_pendientes.push(actTURN_SR);
                acciones_pendientes.push(actTURN_SR);
                acciones_pendientes.push(actWALK);
                break;
			case 4:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_L);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				break;
			case 5:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_L);
				acciones_pendientes.push(actWALK);
				break;
			case 6:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				break;
			case 7:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actWALK);
				break;
			case 8:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				break;
			case 9:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_L);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				break;
			case 10:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_L);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				break;
			case 11:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_L);
				acciones_pendientes.push(actWALK);
				break;
			case 12:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				break;
			case 13:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actWALK);
				break;
			case 14:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				break;
			case 15:
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				orientarJugador(sensores, current_state.brujula);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actTURN_SR);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				acciones_pendientes.push(actWALK);
				break;

            default:
                break;
        }
}



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