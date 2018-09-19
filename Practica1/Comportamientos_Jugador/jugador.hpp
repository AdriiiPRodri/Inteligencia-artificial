#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
using namespace std;

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
        fil = col = 99;
        brujula = objetos_mochila = pasos_sala = 0;
        brujula = objetos_mochila = 0;
        ultimaAccion = accionRecordar = actIDLE;
        precipicios = bien_situado = objeto_delante = recuerda = mutex_objeto = muerte = puerta = puerta_L = puerta_R = explora_sala = dentro_sala = false;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}
    
    Action Reset(Sensores sensores);
    
    void Brujula(Action ultima, Sensores sensores);
    
    int SeePkOrObject(Sensores sensores);
    
    Action Puertas(Sensores Sensores);
    
    Action DireccionPK(int i, Sensores sensores);
    
    Action CogerObjeto(Sensores sensores);
    
    Action UsoObjeto(Sensores sensores);
    
    void DescubreMapa(Sensores sensores);
    
    void Precipicios();
    
    Action Movimiento(Sensores sensores);

    Action think(Sensores sensores);

    int interact(Action accion, int valor);

    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
      int fil, col, objetos_mochila, pasos_sala;
      int aux[200][200] = 0;
      int brujula;      // 0 = NORTE, 1 = ESTE, 2 = SUR, 3 = OESTE
      Action ultimaAccion, accionRecordar;
      bool precipicios, bien_situado, recuerda, objeto_delante, mutex_objeto, muerte, puerta, puerta_L, puerta_R, explora_sala, dentro_sala;
};


#endif
