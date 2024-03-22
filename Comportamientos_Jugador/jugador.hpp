#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include <queue>
#include "comportamientos/comportamiento.hpp"
using namespace std;
struct state{
  int fil;
  int col;
  Orientacion brujula;
};

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      current_state.brujula = norte;
      current_state.fil = size;
      current_state.col = size;
      tiene_bikini = false;
      tiene_zapatillas = false;
      last_action = actIDLE;
      bien_situado = false;


    tam_mapa = size;
    mapaAuxiliar = vector<vector<unsigned char>>(size *2, vector<unsigned char>(size*2, '?'));

    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    bool hayObstaculo(Sensores &sensores);
    void añadirObjeto(Sensores &sensores);
    void movimiento(Action accion);
    void mapTerreno(const vector<unsigned char> &terreno, vector < vector < unsigned char > > &matriz);
    void reinicio(Sensores &sensores);
    void estaAtrapado(Sensores &sensores);
    void detectarPosicionamiento(Sensores &sensores);
    bool recargar(Sensores &sensores);
    void actualizarMapaConAuxiliar(int fil, int col);
    void detectarObjetos(Sensores &sensores);
    bool hayEntidades(Sensores &sensores);

  private:

  // Declarar aquí las variables de estado

  Action last_action;
  Orientacion brujula;
  state current_state;
  bool bien_situado;
  int tam_mapa;
  queue<Action> acciones_pendientes;

  // variables objetos
  bool tiene_bikini;
  bool tiene_zapatillas;

  // mapa
  vector<vector<unsigned char>> mapaAuxiliar;

};
#endif