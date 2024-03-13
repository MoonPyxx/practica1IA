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

	if (current_state.brujula==norte){
		cout << current_state.brujula << endl;
		int index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
        		matriz[st.fil - i][st.col + j] = sensores.terreno[index++];
    	}
}

	} else if (current_state.brujula == sur){
		int index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
        		matriz[st.fil + i][st.col - j] = sensores.terreno[index++];
    			}
			}
	} else if(current_state.brujula==este){
		int index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) { 
        		matriz[st.fil + j][st.col + i] = sensores.terreno[index++];

    }
}
	} else if(current_state.brujula==oeste){
		int index = 1; 
		for(int i = 1; i <= 3; ++i) { 
    		for(int j = -i; j <= i; ++j) {
       			matriz[st.fil - j][st.col - i] = sensores.terreno[index++];

    }
}
	}
}