#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
using namespace std;
struct state{
  int fil;
  int col;
  Orientacion brujula;
  bool tiene_bikini;
  bool tiene_zapatillas;
};

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      current_state.brujula = norte;
      current_state.fil = 99;
      current_state.col = 99;
      current_state.tiene_bikini = false;
      current_state.tiene_zapatillas = false;
      last_action = actIDLE;
      bien_situado = false;
      // tam_mapa = size/2;
    //  mapaAuxiliar = vector<vector<unsigned char>>(size * 2, vector<unsigned char>(size*2, '?'));

    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    bool canWalk(Sensores &sensores);
    bool hayObstaculo(Sensores &sensores);
    bool detectarObjeto(Sensores &sensores);
    bool dentroMapa();
    void movimiento(Action accion);
    void mapTerreno(const vector<unsigned char> &terreno, vector < vector < unsigned char > > &matriz);
    void reinicio(Sensores &sensores);
  private:

  // Declarar aquí las variables de estado

  bool girar_derecha;
  Action last_action;
  Orientacion brujula;
  state current_state;
  bool bien_situado;
  int tam_mapa;

  vector<vector<unsigned char>> mapaAuxiliar;

};
#endif