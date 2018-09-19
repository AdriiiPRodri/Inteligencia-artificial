#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H
#include <list>
#include <stdlib.h>
#include <utility>
//#include <cstdlib>
#include "comportamientos/comportamiento.hpp"
using namespace std;

class ComportamientoJugador : public Comportamiento {
public:

    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
        // Inicializar Variables de Estado
        fil = col = 99;
        for (int i = 0; i < 200; i++)
            for (int j = 0; j < 200; j++)
                aux_vision[i][j] = '?';
        ori_col_aux = 99;
        ori_fil_aux = 99;
        coord_k_aux.first = -1;
        coord_k_aux.second = -1;
        fil_pk = col_pk = -1;
        limit_col_izq = 99 - 3;
        limit_col_der = 99 + 4;
        limit_fil_bot = 99;
        limit_fil_top = 99 - 3;
        for (int i = 0; i < 100; i++)
            for (int j = 0; j < 100; j++)
                aux_pesos[i][j] = 0;
        brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
        ultimaAccion = accionRecordar = actIDLE;
        girar_derecha = false;
        bien_situado = false;
        botas = llave = baniador = regalo = todos_tesoros = false;
        recuerda = cojo_objeto = explora_sala = dentro_sala = derecha = misiones = usoobjeto = false;
        objetos_mochila = pasos_sala = 0;
        itera = contador_plan = 0;
        puerta = 0;

        estoy_ejecutando_plan = false;
        error_plan = false;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport) {
    }

    ~ComportamientoJugador() {
    }

    void Reiniciar();

    bool Puedo_Avanzar(Sensores sensores);

    void Brujula(Action ultima, Sensores sensores);

    int SeePkOrObject(Sensores sensores, int fila, int columna);

    Action Puertas(Sensores Sensores);

    Action DireccionPK(int i, Sensores sensores);

    Action CogerObjeto(Sensores sensores);

    Action UsoObjeto(Sensores sensores);

    void DescubreMapa(Sensores sensores, int fil, int col);

    void Translada();

    Action MovimientoPostPK(Sensores sensores);

    Action MovimientoPrePK(Sensores sensores);

    Action CaminoPesos(Sensores sensores);

    void PorcentajeDescubierto();

    Action think(Sensores sensores);

    int interact(Action accion, int valor);

    ComportamientoJugador * clone() {
        return new ComportamientoJugador(*this);
    }

    bool heAvanzado();

    struct estado {
        int fila;
        int columna;
    };

    class Nodo;

    class AEstrella;

private:

    // Declarar Variables de Estado
    int fil, col, brujula;
    int ori_fil_aux, ori_col_aux, limit_fil_top, limit_fil_bot, limit_col_izq, limit_col_der;
    int fil_pk, col_pk;
    pair<int, int> coord_k_aux;
    char aux_vision[200][200];
    int aux_pesos[100][100];
    Action ultimaAccion, accionRecordar;
    bool girar_derecha;
    bool bien_situado;
    bool recuerda, cojo_objeto, explora_sala, dentro_sala, derecha;
    int objetos_mochila;
    int pasos_sala;
    double itera;
    bool botas, baniador, llave, todos_tesoros, regalo;
    
    // Para el plan
    bool estoy_ejecutando_plan;
    bool error_plan;
    list<Action> plan;
    bool misiones;
    list<pair<int, int> > reyes;
    list<pair<int, int> > pks;
    pair<int, int> objetivo;
    int fila_anterior;
    int columna_anterior;
    int contador_plan;
    int puerta;
    bool usoobjeto;

    friend class ComportamientoJugador::AEstrella;

    bool pathFinding(const estado &origen, const estado &destino, list<Action> &plan);

};

#endif
