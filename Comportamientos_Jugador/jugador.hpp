#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include <queue>
#include <limits>
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
      usar_mapa = true;
      terreno_actual = sensores.terreno[0];
      terreno_frente = sensores.terreno[2];


    tam_mapa = size;
    mapaAuxiliar = vector<vector<unsigned char>>(size *2, vector<unsigned char>(size*2, '?'));
    mapaTiempos = vector<vector<double>>(size *2, vector<double>(size*2, 0));

    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int hayObstaculo(Sensores &sensores);
    void añadirObjeto(Sensores &sensores);
    void movimiento(Action accion, Sensores &sensores);
    void mapTerreno(const vector<unsigned char> &terreno, vector < vector < unsigned char > > &matriz);
    void reinicio(Sensores &sensores);
    void estaAtrapado(Sensores &sensores);
    void detectarPosicionamiento(Sensores &sensores);
    bool recargar(Sensores &sensores);
    void actualizarMapaConAuxiliar(int fil, int col);
    void detectarObjetos(Sensores &sensores);
    int hayEntidades(Sensores &sensores);
    void limpiarCola();
    void reorientarMapa(Sensores &sensores);

    void borrarMapaAuxiliar();
    bool dentroDeMapa(int fil, int col, int filasMax, int columnasMax);
    void accionPorCasilla(int casilla);
    void comprobarMapaTiempos(Sensores &sensores);
    void actualizarMapaTiempos(int fil, int col);
    template <typename T>
    void rotarMapa(vector<vector<T>>& map);

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
  bool usar_mapa;


  // mapa de tiempos
  vector<vector<double>> mapaTiempos;


const unsigned char OBJETO_BIKINI = 'K';
const unsigned char OBJETO_ZAPATILLAS = 'D';
const unsigned char OBJETO_RECARGA = 'X';
const unsigned char TERRENO_BOSQUE = 'B';
const unsigned char TERRENO_PRECIPICIO = 'P';
const unsigned char TERRENO_MURO = 'M';
const unsigned char TERRENO_AGUA = 'A';
char terreno_actual;
char terreno_frente;


};
#endif