#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{
	int a = current_state.brujula;
	Action accion = actIDLE;
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
		// Actualizacion en caso de avanzar
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
	
	if (sensores.posF!=-1 && !bien_situado){
		current_state.fil = sensores.posF;
		current_state.col = sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
	}
	if (bien_situado){
		mapTerreno(sensores, sensores.terreno, current_state, mapaResultado);
	}

	// Decidir la nueva accion
	// toca muro
	if (sensores.terreno[2]== 'M'){
		accion = actTURN_L;
	}
	if ((sensores.terreno[2]=='T' || sensores.terreno[2]=='S'
	 || sensores.terreno[2]=='G') && sensores.agentes[2] == '_'){
 		accion = actWALK;
 		} else if (!girar_derecha){
			accion = actTURN_L;
			girar_derecha = (rand()%2 == 0);

		} else {
 		accion = actTURN_SR;
		girar_derecha = (rand()%2 == 0);
		}
	// Recordar la ultima accion
	last_action = accion;
	return accion;
	
	

	// Mostrar el valor de los sensores
	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC;
	switch (sensores.sentido)
	{
		case norte:	  cout << " Norte\n";	break;
		case noreste: cout << " Noreste\n";	break;
		case este:    cout << " Este\n";	break;
		case sureste: cout << " Sureste\n";	break;
		case sur:     cout << " Sur\n";	break;
		case suroeste:cout << " Suroeste\n";	break;
		case oeste:   cout << " Oeste\n";	break;
		case noroeste:cout << " Noroeste\n";	break;
	}
	cout << "Terreno: ";
	for (int i=0; i<sensores.terreno.size(); i++)
		cout << sensores.terreno[i];

	cout << "  Agentes: ";
	for (int i=0; i<sensores.agentes.size(); i++)
		cout << sensores.agentes[i];

	cout << "\nColision: " << sensores.colision;
	cout << "  Reset: " << sensores.reset;
	cout << "  Vida: " << sensores.vida << endl<< endl;


	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
void ComportamientoJugador::mapTerreno(Sensores &sensores, const vector<unsigned char> &terreno, const state &st, vector < vector < unsigned char > > &matriz){
	matriz[st.fil][st.col] = terreno[0];
	int index = 1;
	switch(sensores.sentido){
		case norte:
		cout << current_state.brujula << endl;
		 index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
        		matriz[st.fil - i][st.col + j] = sensores.terreno[index++];
    	}
		}
		break;

		case sur:
		 index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
        		matriz[st.fil + i][st.col - j] = sensores.terreno[index++];
    			}
			}
		break;

		case este:
			index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
        		matriz[st.fil + j][st.col + i] = sensores.terreno[index++];

    		}
		}
		break;

		case oeste:
		index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) {
       			matriz[st.fil - j][st.col - i] = sensores.terreno[index++];

    }
}
		break;
		case noreste:
		int index = 1;
int n = 3; // Suponiendo que el '3' representa el nivel máximo de expansión en tu patrón.

for(int i = 1; i <= n; ++i) {
    // Primer bucle: maneja la expansión hacia arriba y a la derecha.
    for(int j = -i; j <= i; ++j) {
        matriz[st.fil - i][st.col + j] = sensores.terreno[index++];
    }
    // Segundo bucle: maneja la expansión hacia la derecha.
    if(i < n) { // Para evitar sobreescritura en la última iteración de 'i'.
        for(int j = 1; j <= i; ++j) {
            matriz[st.fil - i + j][st.col + i] = sensores.terreno[index++];
        }
    }
}

// Tercer bucle: maneja la última columna hacia abajo, si es necesario.
// Nota: Este paso se ajusta si necesitas expandirte más allá de la estructura dada.
for(int i = -n; i <= 0; ++i) {
    if(-i > 1) { // Ajusta según necesidad para evitar sobreescrituras.
        matriz[st.fil + i][st.col + n] = sensores.terreno[index++];
    }
}

	/*1 = matriz[-1][0]
	2 = matriz [-1][+1]
	3 = matriz [0][+1]
	4 = matriz [-2][0]
	5 = matriz [-2][+1]
	6 = matriz [-2][+2]
	7 = matriz [-1][+2]
	8 = matriz [0][+2]
	9 = matriz [-3][0]
	10 = matriz [-3][+1]
	11 = matriz [-3][+2]
	12 = matriz [-3][+3]
	13 = matriz [-2][+3]
	14 = matriz [-1][+3]
	15 = matriz [0][+3]
	*/

}
}
