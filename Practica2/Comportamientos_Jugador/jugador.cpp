#include "../Comportamientos_Jugador/jugador.hpp"
#include "personaje.hpp"
#include "motorlib/entidad.hpp"
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

void ComportamientoJugador::Reiniciar() {
    fil = col = 99;
    ultimaAccion = accionRecordar = actIDLE;
    bien_situado = false;
    brujula = 0;
    estoy_ejecutando_plan = false;
    error_plan = false;
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
    estoy_ejecutando_plan = false;
    regalo = botas = baniador = llave = todos_tesoros = false;
    contador_plan = 0;
    recuerda = false;
    cojo_objeto = false;
    plan.clear();
}

bool ComportamientoJugador::Puedo_Avanzar(Sensores sensores) {
    if (sensores.terreno[2] == 'B')
        if (botas && sensores.terreno[0] != 'A')
            return true;
        else
            return false;
    if (sensores.terreno[2] == 'A')
        if (baniador && sensores.terreno[0] != 'B')
            return true;
        else
            return false;
    if (sensores.terreno[2] == 'D')
        if (llaves)
            return true;
        else
            return false;

    if (sensores.superficie[2] == 'l' || sensores.superficie[2] == 'a' || sensores.superficie[2] == 'r' || sensores.terreno[2] == 'M' || sensores.terreno[2] == 'P')
        return false;
    else
        return true;
}

void ComportamientoJugador::Brujula(Action ultima, Sensores sensores) {
    if (!sensores.colision)
        switch (ultima) {
            case actFORWARD:
                switch (brujula) {
                    case 0: // Norte
                        if (bien_situado)
                            fil--;
                        else
                            ori_fil_aux--;
                        break;
                    case 1: // Este
                        if (bien_situado)
                            col++;
                        else
                            ori_col_aux++;
                        break;
                    case 2: // Sur
                        if (bien_situado)
                            fil++;
                        else
                            ori_fil_aux++;
                        break;
                    case 3: // Oeste
                        if (bien_situado)
                            col--;
                        else
                            ori_col_aux--;
                        break;
                }
                break;
            case actTURN_L:
                brujula = (brujula + 3) % 4;
                break;
            case actTURN_R:
                brujula = (brujula + 1) % 4;
                break;
        }
}

int ComportamientoJugador::SeePkOrObject(Sensores sensores, int fila = -1, int columna = -1) { // SOLO RECOGE OBJETOS SI NOS LO TIENE YA
    if (sensores.vida < 100 || sensores.colision || sensores.terreno[2] == 'M' || sensores.terreno[2] == 'P' || (sensores.terreno[2] == 'B' && !botas) || (sensores.terreno[2] == 'A' && !baniador) || (sensores.terreno[2] == 'D' && !llave))
        return -1;

    if (sensores.terreno[2] == 'T' || sensores.terreno[2] == 'S' || (sensores.terreno[2] == 'B' && sensores.objetoActivo == '2') || (sensores.terreno[2] == 'A' && sensores.objetoActivo == '1'))
        for (int i = 0; i < 16; i++)
            if ((sensores.superficie[i] == 'r' && regalo) || (sensores.superficie[i] == 'd' && llave) || (sensores.terreno[i] == 'K' && !bien_situado) || (sensores.superficie[i] == '1' && !baniador) || (sensores.superficie[i] == '2' && !botas) || (sensores.superficie[i] == '4' && !regalo && bien_situado) || (sensores.superficie[i] == '3' && !llave)) { // 4 = tesoro               
                //cout << i << endl;
                return i;
            }

    return -1;
}

Action ComportamientoJugador::DireccionPK(int i, Sensores sensores) { // VA HACIA PKS Y OBJETOS
    recuerda = true;

    if ((i > 8 && i < 12) || (i > 3 && i < 6) || i == 1) {
        if (sensores.terreno[1] == 'M') {
            return actFORWARD;
        }

        if (sensores.superficie[1] == 'd') {
            accionRecordar = actTURN_L;
            return actFORWARD;
        }

        accionRecordar = actTURN_R;
        return actTURN_L;
    } else if ((i > 12 && i < 16) || (i > 6 && i < 9) || i == 3) {
        if (sensores.terreno[3] == 'M') {
            return actFORWARD;
        }

        if (sensores.superficie[3] == 'd') {
            accionRecordar = actTURN_R;
            return actFORWARD;
        }

        accionRecordar = actTURN_L;
        return actTURN_R;
    }

    //cout << "Aqui" << endl;
    if (sensores.terreno[2] != 'P')
        return actFORWARD;
    else
        return actTURN_L;
}

Action ComportamientoJugador::CogerObjeto(Sensores sensores) {
    static bool tira = false;
    static bool tira_hueso = false;

    if (sensores.vida > 100) {
        if (tira_hueso) {
            tira_hueso = false;
            return actTHROW;
        }

        if (sensores.superficie[2] == '4' && !bien_situado)
            return actIDLE;

        if (botas && baniador && llave && regalo)
            todos_tesoros = true;

        if (sensores.terreno[0] != 'B' && sensores.terreno[0] != 'A' && !todos_tesoros && (sensores.superficie[2] == '0' || (sensores.superficie[2] == '1' && !baniador) || (sensores.superficie[2] == '2' && !botas) || (sensores.superficie[2] == '3' && !llave) || (sensores.superficie[2] == '4' && !regalo))) {
            cojo_objeto = true;

            if (sensores.superficie[2] == '0')
                tira_hueso = true;

            if (sensores.objetoActivo != '_') {
                objetos_mochila++;

                return actPUSH;
            } else {

                if (sensores.superficie[2] == '1')
                    baniador = true;
                else if (sensores.superficie[2] == '2')
                    botas = true;
                else if (sensores.superficie[2] == '3')
                    llave = true;
                else if (sensores.superficie[2] == '4')
                    regalo = true;

                if (sensores.superficie[2] == '4')
                    estoy_ejecutando_plan = false;

                return actPICKUP;
            }
        } else if (sensores.colision && todos_tesoros && sensores.terreno[0] != 'B' && sensores.terreno[0] != 'A') {
            if (sensores.objetoActivo == '1')
                baniador = false;
            else if (sensores.objetoActivo == '2')
                botas = false;
            else if (sensores.objetoActivo == '3')
                llave = false;
            else if (sensores.objetoActivo == '4')
                regalo = false;

            tira = true;

            return actTHROW;
        }

        if (sensores.terreno[0] != 'B' && sensores.terreno[0] != 'A' && !tira && sensores.colision && ((sensores.superficie[2] == '1' && baniador) || (sensores.superficie[2] == '2' && botas) || (sensores.superficie[2] == '3' && llave) || (sensores.superficie[2] == '4' && regalo))) {
            cojo_objeto = true;
            //cout << "Entra" << endl;
            tira = true;
            return actPUSH;
        } else if (tira && ultimaAccion != actPICKUP) {
            cojo_objeto = true;
            if (sensores.superficie[2] == '1')
                baniador = true;
            else if (sensores.superficie[2] == '2')
                botas = true;
            else if (sensores.superficie[2] == '3')
                llave = true;
            else if (sensores.superficie[2] == '4')
                regalo = true;
            return actPICKUP;
        } else if (tira) {
            tira = false;
            cojo_objeto = true;
            return actTHROW;
        }
    } else {
        if (sensores.objetoActivo != '_' && sensores.superficie[2] == '_' && sensores.superficie[6] == '_' && sensores.superficie[12] == '_' && (sensores.terreno[12] != 'P' && sensores.terreno[12] != 'M' && sensores.terreno[12] != 'B' && sensores.terreno[12] != 'A')) {
            cojo_objeto = true;
            if (sensores.objetoActivo == '1')
                baniador = false;
            else if (sensores.objetoActivo == '2')
                botas = false;
            else if (sensores.objetoActivo == '3')
                llave = false;
            else if (sensores.objetoActivo == '4')
                regalo = false;

            return actTHROW;
        } else if (sensores.objetoActivo == '_' && (llave || regalo)) {
            cojo_objeto = true;
            return actPOP;
        } else
            return actIDLE;
    }
}

Action ComportamientoJugador::UsoObjeto(Sensores sensores) {
    cojo_objeto = false;

    if (sensores.terreno[2] == 'B' && botas) {
        if (sensores.terreno[0] != 'A' && sensores.objetoActivo == '_') {
            usoobjeto = true;
            return actPOP;
        } else if (sensores.objetoActivo != '2' && sensores.terreno[0] != 'A') {
            usoobjeto = true;
            return actPUSH;
        }
    } else if (sensores.terreno[2] == 'A' && baniador) {
        if (sensores.terreno[0] != 'B' && sensores.objetoActivo == '_') {
            usoobjeto = true;
            return actPOP;
        } else if (sensores.objetoActivo != '1' && sensores.terreno[0] != 'B') {
            usoobjeto = true;
            return actPUSH;
        }
    } else if (sensores.superficie[2] == 'r' && regalo) {
        if (sensores.objetoActivo == '4') {
            usoobjeto = true;
            regalo = false;
            todos_tesoros = false;
            estoy_ejecutando_plan = false;
            return actGIVE;
        } else if (sensores.objetoActivo == '_') {
            usoobjeto = true;
            return actPOP;
        } else if (sensores.objetoActivo != '_' && sensores.objetoActivo != '4') {
            usoobjeto = true;
            return actPUSH;
        }
    } else if (sensores.superficie[2] == 'd' && llave) {
        if (sensores.objetoActivo == '3') {
            usoobjeto = true;
            if (puerta == 10)
                puerta = 0;
            else if (puerta == 0)
                puerta = 10;
            return actGIVE;
        } else if (sensores.objetoActivo == '_') {
            usoobjeto = true;
            return actPOP;
        } else if (sensores.objetoActivo != '_' && sensores.objetoActivo != '3') {
            usoobjeto = true;
            return actPUSH;
        }
    }

    if (sensores.superficie[2] == 'l')
        usoobjeto = false;
}

void ComportamientoJugador::DescubreMapa(Sensores sensores, int fila, int columna) {
    int aux_f;
    int aux_c;
    static bool precipicios = false;

    if (!precipicios) {
        for (int i = 0; i < 3; i++) // Precipicios norte
            for (int j = 0; j < 100; j++)
                mapaResultado[i][j] = 'P';

        for (int i = 97; i < 100; i++) // Precipicios sur
            for (int j = 0; j < 100; j++)
                mapaResultado[i][j] = 'P';

        for (int i = 0; i < 3; i++) // Precipicios oeste
            for (int j = 0; j < 100; j++)
                mapaResultado[j][i] = 'P';

        for (int i = 97; i < 100; i++) // Precipicios este
            for (int j = 0; j < 100; j++)
                mapaResultado[j][i] = 'P';
        precipicios = true;
    }

    switch (brujula) {
        case 0: // NORTE
            aux_f = -1;
            aux_c = -1;

            for (int i = 1; i < 16; i++) {
                if (bien_situado) {
                    if (mapaResultado[fila + aux_f][columna + aux_c] == '?') {
                        mapaResultado[fila + aux_f][columna + aux_c] = sensores.terreno[i];
                    }

                    if (sensores.superficie[i] == 'r') { // GUARDAMOS LA POSICION DE TODOS LOS REYES QUE VEAMOS
                        pair<int, int> rey(fila + aux_f, columna + aux_c);
                        reyes.push_front(rey);
                    }
                } else {
                    if (aux_vision[fila + aux_f][columna + aux_c] == '?')
                        aux_vision[fila + aux_f][columna + aux_c] = sensores.terreno[i];
                }

                if (limit_col_der < ori_col_aux + aux_c)
                    limit_col_der = ori_col_aux + aux_c;

                if (limit_fil_top > ori_fil_aux + aux_f)
                    limit_fil_top = ori_fil_aux + aux_f;

                if (limit_col_izq > ori_col_aux + aux_c)
                    limit_col_izq = ori_col_aux + aux_c;

                if (i == 3) {
                    aux_c = -2;
                    aux_f--;
                } else if (i == 8) {
                    aux_c = -3;
                    aux_f--;
                } else
                    aux_c++;

            }

            break;
        case 1: // ESTE
            aux_f = -1;
            aux_c = 1;

            for (int i = 1; i < 16; i++) {
                if (bien_situado) {
                    if (mapaResultado[fila + aux_f][columna + aux_c] == '?')
                        mapaResultado[fila + aux_f][columna + aux_c] = sensores.terreno[i];

                    if (sensores.superficie[i] == 'r') {// GUARDAMOS LA POSICION DE TODOS LOS REYES QUE VEAMOS
                        pair<int, int> rey(fila + aux_f, columna + aux_c);
                        reyes.push_front(rey);
                    }
                } else {
                    if (aux_vision[fila + aux_f][columna + aux_c] == '?')
                        aux_vision[fila + aux_f][columna + aux_c] = sensores.terreno[i];
                }

                if (limit_col_der < ori_col_aux + aux_c)
                    limit_col_der = ori_col_aux + aux_c;

                if (limit_fil_top > ori_fil_aux + aux_f)
                    limit_fil_top = ori_fil_aux + aux_f;

                if (limit_fil_bot < ori_fil_aux + aux_f)
                    limit_fil_bot = ori_fil_aux + aux_f;

                if (i == 3) {
                    aux_c++;
                    aux_f = -2;
                } else if (i == 8) {
                    aux_c++;
                    aux_f = -3;
                } else
                    aux_f++;

            }
            break;
        case 2: // SUR
            aux_f = 1;
            aux_c = 1;

            for (int i = 1; i < 16; i++) {
                if (bien_situado) {
                    if (mapaResultado[fila + aux_f][columna + aux_c] == '?')
                        mapaResultado[fila + aux_f][columna + aux_c] = sensores.terreno[i];

                    if (sensores.superficie[i] == 'r') {// GUARDAMOS LA POSICION DE TODOS LOS REYES QUE VEAMOS
                        pair<int, int> rey(fila + aux_f, columna + aux_c);
                        reyes.push_front(rey);
                    }
                } else {
                    if (aux_vision[fila + aux_f][columna + aux_c] == '?')
                        aux_vision[fila + aux_f][columna + aux_c] = sensores.terreno[i];
                }

                if (limit_fil_bot < ori_fil_aux + aux_f)
                    limit_fil_bot = ori_fil_aux + aux_f;

                if (limit_col_izq > ori_col_aux + aux_c)
                    limit_col_izq = ori_col_aux + aux_c;

                if (limit_col_der < ori_col_aux + aux_c)
                    limit_col_der = ori_col_aux + aux_c;

                if (i == 3) {
                    aux_c = 2;
                    aux_f++;
                } else if (i == 8) {
                    aux_c = 3;
                    aux_f++;
                } else
                    aux_c--;

            }
            break;
        case 3: // OESTE
            aux_f = 1;
            aux_c = -1;

            for (int i = 1; i < 16; i++) {
                if (bien_situado) {
                    if (mapaResultado[fila + aux_f][columna + aux_c] == '?')
                        mapaResultado[fila + aux_f][columna + aux_c] = sensores.terreno[i];

                    if (sensores.superficie[i] == 'r') {// GUARDAMOS LA POSICION DE TODOS LOS REYES QUE VEAMOS
                        pair<int, int> rey(fila + aux_f, columna + aux_c);
                        reyes.push_front(rey);
                    }
                } else {
                    if (aux_vision[fila + aux_f][columna + aux_c] == '?')
                        aux_vision[fila + aux_f][columna + aux_c] = sensores.terreno[i];
                }

                if (limit_col_izq > ori_col_aux + aux_c)
                    limit_col_izq = ori_col_aux + aux_c;

                if (limit_fil_top > ori_fil_aux + aux_f)
                    limit_fil_top = ori_fil_aux + aux_f;

                if (limit_fil_bot < ori_fil_aux + aux_f)
                    limit_fil_bot = ori_fil_aux + aux_f;

                if (i == 3) {
                    aux_c--;
                    aux_f = 2;
                } else if (i == 8) {
                    aux_c--;
                    aux_f = 3;
                } else
                    aux_f--;
            }
            break;
    }

    //    for (int i = limit_fil_top; i < limit_fil_bot; i++) {
    //        for (int j = limit_col_izq; j < limit_col_der; j++)
    //            //if(aux_vision[i][j] != '?')
    //            cout << aux_vision[i][j] << " ";
    //        cout << endl;
    //    }
    //    cout << endl;
}

void ComportamientoJugador::Translada() {
    int fil_ori = fil;
    int col_ori = col;

    //    for (int i = limit_fil_top; i <= limit_fil_bot; i++) {
    //        for (int j = limit_col_izq; j <= limit_col_der; j++)
    //            cout << aux_vision[i][j];
    //        cout << endl;
    //    }

    for (int i = coord_k_aux.first; i > limit_fil_top; i--, fil_ori--)
        for (int j = limit_col_izq; j <= limit_col_der; j++)
            if (mapaResultado[fil_ori][col_ori + (j - coord_k_aux.second)] == '?' && aux_vision[i][j] != '?')
                mapaResultado[fil_ori][col_ori + (j - coord_k_aux.second)] = aux_vision[i][j];


    fil_ori = fil;

    for (int i = coord_k_aux.first; i <= limit_fil_bot; i++, fil_ori++)
        for (int j = limit_col_izq; j <= limit_col_der; j++)
            if (mapaResultado[fil_ori][col_ori + (j - coord_k_aux.second)] == '?' && aux_vision[i][j] != '?')
                mapaResultado[fil_ori][col_ori + (j - coord_k_aux.second)] = aux_vision[i][j];
}

Action ComportamientoJugador::MovimientoPostPK(Sensores sensores) {
    static int aleatorio;
    static int contador = 0;

    aleatorio = rand() % 2;

    if (!Puedo_Avanzar(sensores) || contador > 15) {
        contador = 0;
        if (aleatorio == 0)
            return actTURN_R;
        else
            return actTURN_L;
    } else {
        contador++;
        //cout << "Entra" << endl;
        return actFORWARD;
    }
}

Action ComportamientoJugador::MovimientoPrePK(Sensores sensores) {
    static int contador = 0;

    switch (brujula) {
        case 0: // Norte
            if (Puedo_Avanzar(sensores))
                return actFORWARD;
            else if (derecha)
                return actTURN_R;
            else
                return actTURN_L;

        case 1:
            if (sensores.terreno[2] == 'P' && (sensores.terreno[1] == 'P' || sensores.terreno[3] == 'P'))
                if (derecha)
                    derecha = false;
                else
                    derecha = true;

            if (contador == 4) {
                contador = 0;

                if (sensores.terreno[1] == 'P')
                    return actTURN_R;
                else
                    return actTURN_L;
            } else if (Puedo_Avanzar(sensores)) {
                contador++;
                return actFORWARD;
            } else
                return actTURN_R;

        case 2: // Sur
            if (Puedo_Avanzar(sensores))
                return actFORWARD;
            else if (derecha)
                return actTURN_L;
            else
                return actTURN_R;

        case 3:
            if (sensores.terreno[2] == 'P' && (sensores.terreno[1] == 'P' || sensores.terreno[3] == 'P'))
                if (derecha)
                    derecha = false;
                else
                    derecha = true;

            if (contador == 4) {
                contador = 0;

                if (sensores.terreno[3] == 'P')
                    return actTURN_L;
                else
                    return actTURN_R;
            } else if (Puedo_Avanzar(sensores)) {
                contador++;
                return actFORWARD;
            } else
                return actTURN_L;

    }

    //cout << "Salgo" << endl;

    return actIDLE;
}

void PintaPlan(list<Action> plan) {
    auto it = plan.begin();
    while (it != plan.end()) {
        if (*it == actFORWARD) {
            cout << "A ";
        } else if (*it == actTURN_R) {
            cout << "D ";
        } else if (*it == actTURN_L) {
            cout << "I ";
        } else {
            cout << "- ";
        }
        it++;
    }
    cout << endl;
}

void ComportamientoJugador::PorcentajeDescubierto() {
    cout << "\t" << itera << " iteraciones realizadas." << endl;
    //cout << "\t" << ((itera / 20000) * 100) << "% iteraciones realizadas." << endl;
}

class ComportamientoJugador::Nodo {
    pair<int, int> posicion; // Posicion actual
    int esfuerzo; // Distancia total que le queda hasta alcanzar el nodo
    int prioridad; // prioridad = distancia + distancia_estimada_queda --- Menor = Mayor prioridad
    ComportamientoJugador::Nodo* padre;
public:

    Nodo() {
        posicion.first = -1;
        posicion.second = -1;
        esfuerzo = 0;
        prioridad = 0;
        padre = NULL;
    }

    Nodo(int xp, int yp, int e, int filaF, int columnaF, ComportamientoJugador::Nodo* f = NULL) {
        posicion.first = xp;
        posicion.second = yp;
        esfuerzo = e;
        padre = f;
        actualizaPrioridad(filaF, columnaF);
    }

    pair<int, int> getPosicion() const {
        return posicion;
    }

    int getEsfuerzo() const {
        return esfuerzo;
    }

    int getPrioridad() const {
        return prioridad;
    }

    ComportamientoJugador::Nodo* getPadre() {
        return padre;
    }

    void actualizaPrioridad(const int & xDest, const int & yDest) {
        prioridad = esfuerzo + estimate(xDest, yDest); // A*, esta es nuestra función f = g + h
    }

    void siguienteEsfuerzo() { // No hay movimientos diagonales 
        esfuerzo++;
    }

    const int & estimate(const int & xDest, const int & yDest) const { // HEURÍSTICA, estimación de la distancia que queda hasta llegar al punto
        static int xd, yd, d;
        xd = xDest - posicion.first;
        yd = yDest - posicion.second;

        // Distancia Euclídea
        //d = static_cast<int> (sqrt(xd * xd + yd * yd));

        // Distancia Manhattan
        d = abs(xd) + abs(yd);

        // Distancia Chebyshev
        //d=max(abs(xd), abs(yd));

        return d;
    }
};

class ComportamientoJugador::AEstrella {
public:
    list<ComportamientoJugador::Nodo*> abierta;
    list<ComportamientoJugador::Nodo*> cerrada;
    list<ComportamientoJugador::Nodo*> candidatos;
    ComportamientoJugador::Nodo* inicio;
    ComportamientoJugador::Nodo* select;
    ComportamientoJugador::Nodo* fin;
    list<pair<int, int> > camino_coord;
    int brujula_aux;
    int contador = 0;

    AEstrella(const ComportamientoJugador::estado &origen, const ComportamientoJugador::estado &destino, int brujula, vector< std::vector< unsigned char> > mR, Sensores sensores, bool llave, bool baniador, bool botas) {
        inicio = new ComportamientoJugador::Nodo(origen.fila, origen.columna, 0, destino.fila, destino.columna);
        fin = new ComportamientoJugador::Nodo(destino.fila, destino.columna, 0, destino.fila, destino.columna);
        cerrada.push_back(inicio);
        abierta.merge(vecinos(inicio, mR, sensores, llave, baniador, botas));
        brujula_aux = brujula;

        while (!objetivo() && contador < 150) { // CON CONTADOR CONTROLAMOS QUE NO EXISTA UN CAMINO POSIBLE SI EXPLORA MÁS DE 150 NODOS
            buscar(mR, sensores, llave, baniador, botas);
            contador++;
        }

        if (contador < 150)
            camino_coord = camino();
    }

    list<Action> creaPlan() {
        int fil_aux = inicio->getPosicion().first;
        int col_aux = inicio->getPosicion().second;
        list<Action> plan;

        if (!camino_coord.empty()) {
            camino_coord.reverse();

            //            for (auto it = camino_coord.begin(); it != camino_coord.end(); it++)
            //                cout << (*it).first << " " << (*it).second << endl;

            for (auto it = camino_coord.begin(); it != camino_coord.end(); it++) {
                //cout << fil_aux << " " << col_aux << endl;
                switch (brujula_aux) {
                    case 0: // Norte                    
                        if (fil_aux > (*it).first) {
                            plan.push_back(actFORWARD);
                            fil_aux--;
                            brujula_aux = 0;
                        } else if (fil_aux < (*it).first) {
                            //cout << "Entra" << endl;
                            plan.push_back(actTURN_L);
                            plan.push_back(actTURN_L);
                            plan.push_back(actFORWARD);
                            fil_aux++;
                            brujula_aux = 2;
                        } else if (col_aux > (*it).second) {
                            plan.push_back(actTURN_L);
                            plan.push_back(actFORWARD);
                            col_aux--;
                            brujula_aux = 3;
                        } else if (col_aux < (*it).second) {
                            plan.push_back(actTURN_R);
                            plan.push_back(actFORWARD);
                            col_aux++;
                            brujula_aux = 1;
                        }
                        break;
                    case 1: // Este
                        if (fil_aux > (*it).first) {
                            plan.push_back(actTURN_L);
                            plan.push_back(actFORWARD);
                            fil_aux--;
                            brujula_aux = 0;
                        } else if (fil_aux < (*it).first) {
                            plan.push_back(actTURN_R);
                            plan.push_back(actFORWARD);
                            fil_aux++;
                            brujula_aux = 2;
                        } else if (col_aux > (*it).second) {
                            plan.push_back(actTURN_L);
                            plan.push_back(actTURN_L);
                            plan.push_back(actFORWARD);
                            col_aux--;
                            brujula_aux = 3;
                        } else if (col_aux < (*it).second) {
                            plan.push_back(actFORWARD);
                            col_aux++;
                            brujula_aux = 1;
                        }
                        break;
                    case 2: // Sur
                        if (fil_aux > (*it).first) {
                            plan.push_back(actTURN_L);
                            plan.push_back(actTURN_L);
                            plan.push_back(actFORWARD);
                            fil_aux--;
                            brujula_aux = 0;
                        } else if (fil_aux < (*it).first) {
                            plan.push_back(actFORWARD);
                            fil_aux++;
                            brujula_aux = 2;
                        } else if (col_aux > (*it).second) {
                            plan.push_back(actTURN_R);
                            plan.push_back(actFORWARD);
                            col_aux--;
                            brujula_aux = 3;
                        } else if (col_aux < (*it).second) {
                            plan.push_back(actTURN_L);
                            plan.push_back(actFORWARD);
                            col_aux++;
                            brujula_aux = 1;
                        }
                        break;
                    case 3: // Oeste
                        if (fil_aux > (*it).first) {
                            plan.push_back(actTURN_R);
                            plan.push_back(actFORWARD);
                            fil_aux--;
                            brujula_aux = 0;
                        } else if (fil_aux < (*it).first) {
                            plan.push_back(actTURN_L);
                            plan.push_back(actFORWARD);
                            fil_aux++;
                            brujula_aux = 2;
                        } else if (col_aux > (*it).second) {
                            plan.push_back(actFORWARD);
                            col_aux--;
                            brujula_aux = 3;
                        } else if (col_aux < (*it).second) {
                            plan.push_back(actTURN_L);
                            plan.push_back(actTURN_L);
                            plan.push_back(actFORWARD);
                            col_aux++;
                            brujula_aux = 1;
                        }
                        break;
                }

                //cout << (*it).first << " " << (*it).second << endl;
            }

            camino_coord.clear();
        }

        return plan;
    }

    list<ComportamientoJugador::Nodo*> vecinos(ComportamientoJugador::Nodo* nodo, vector< std::vector< unsigned char> > mR, Sensores sensores, bool llave, bool baniador, bool botas) {
        list<ComportamientoJugador::Nodo*> devolver;

        if (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] != '?' && mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] != 'M' && mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] != 'P')
            if (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'K' || mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'T' || mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'S' || (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'B' && botas) || (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'A' && baniador) || (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'D' && llave))
                devolver.push_back(new ComportamientoJugador::Nodo(nodo->getPosicion().first - 1, nodo->getPosicion().second, nodo->getEsfuerzo() + 1, fin->getPosicion().first, fin->getPosicion().second, nodo));

        if (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] != '?' && mR[nodo->getPosicion().first][nodo->getPosicion().second + 1] != 'M' && mR[nodo->getPosicion().first][nodo->getPosicion().second + 1] != 'P')
            if (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'K' || mR[nodo->getPosicion().first][nodo->getPosicion().second + 1] == 'T' || mR[nodo->getPosicion().first][nodo->getPosicion().second + 1] == 'S' || (mR[nodo->getPosicion().first][nodo->getPosicion().second + 1] == 'B' && botas) || (mR[nodo->getPosicion().first][nodo->getPosicion().second + 1] == 'A' && baniador) || (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'D' && llave))
                devolver.push_back(new ComportamientoJugador::Nodo(nodo->getPosicion().first, nodo->getPosicion().second + 1, nodo->getEsfuerzo() + 1, fin->getPosicion().first, fin->getPosicion().second, nodo));

        if (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] != '?' && mR[nodo->getPosicion().first][nodo->getPosicion().second - 1] != 'M' && mR[nodo->getPosicion().first][nodo->getPosicion().second - 1] != 'P')
            if (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'K' || mR[nodo->getPosicion().first][nodo->getPosicion().second - 1] == 'T' || mR[nodo->getPosicion().first][nodo->getPosicion().second - 1] == 'S' || (mR[nodo->getPosicion().first][nodo->getPosicion().second - 1] == 'B' && botas) || (mR[nodo->getPosicion().first][nodo->getPosicion().second - 1] == 'A' && baniador) || (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'D' && llave))
                devolver.push_back(new ComportamientoJugador::Nodo(nodo->getPosicion().first, nodo->getPosicion().second - 1, nodo->getEsfuerzo() + 1, fin->getPosicion().first, fin->getPosicion().second, nodo));

        if (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] != '?' && mR[nodo->getPosicion().first + 1][nodo->getPosicion().second] != 'M' && mR[nodo->getPosicion().first + 1][nodo->getPosicion().second] != 'P')
            if (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'K' || mR[nodo->getPosicion().first + 1][nodo->getPosicion().second] == 'T' || mR[nodo->getPosicion().first + 1][nodo->getPosicion().second] == 'S' || (mR[nodo->getPosicion().first + 1][nodo->getPosicion().second] == 'B' && botas) || (mR[nodo->getPosicion().first + 1][nodo->getPosicion().second] == 'A' && baniador) || (mR[nodo->getPosicion().first - 1][nodo->getPosicion().second] == 'D' && llave))
                devolver.push_back(new ComportamientoJugador::Nodo(nodo->getPosicion().first + 1, nodo->getPosicion().second, nodo->getEsfuerzo() + 1, fin->getPosicion().first, fin->getPosicion().second, nodo));


        return devolver;
    }

    void f_menor() { // PROBABLE FALLO AL ELIMINAR DE ABIERTA
        list<ComportamientoJugador::Nodo*>::iterator p = abierta.begin();

        for (list<ComportamientoJugador::Nodo*>::iterator it = abierta.begin(); it != abierta.end(); it++)
            if ((*it)->getPrioridad() < (*abierta.begin())->getPrioridad()) {
                p = it;
            }

        cerrada.push_back((*p));
        abierta.erase(p);
    }

    bool en_lista(ComportamientoJugador::Nodo* nodo, list<ComportamientoJugador::Nodo*> lista) {
        for (list<ComportamientoJugador::Nodo*>::iterator it = lista.begin(); it != lista.end(); it++)
            if (nodo->getPosicion() == (*it)->getPosicion())
                return true;

        return false;
    }

    void ruta() {
        for (list<ComportamientoJugador::Nodo*>::iterator it = candidatos.begin(); it != candidatos.end(); it++)
            if (en_lista((*it), cerrada))
                continue;
            else if (!en_lista((*it), abierta))
                abierta.push_back(*it);
            else {
                if (select->getEsfuerzo() + 1 < (*it)->getEsfuerzo())
                    for (list<ComportamientoJugador::Nodo*>::iterator at = abierta.begin(); at != abierta.end(); at++)
                        if ((*it)->getPosicion() == (*at)->getPosicion()) {
                            abierta.erase(at);
                            abierta.push_back(*it);
                            break;
                        }
            }
    }

    void buscar(vector< std::vector< unsigned char> > mR, Sensores sensores, bool llave, bool baniador, bool botas) {
        f_menor();
        select = cerrada.back();
        candidatos = vecinos(select, mR, sensores, llave, baniador, botas);
        ruta();
    }

    bool objetivo() {
        for (list<ComportamientoJugador::Nodo*>::iterator at = abierta.begin(); at != abierta.end(); at++)
            if (fin->getPosicion() == (*at)->getPosicion())
                return true;
        //cout << abierta.size() << endl;
        if (abierta.empty())
            return true;

        return false;
    }

    list<pair<int, int> > camino() {
        ComportamientoJugador::Nodo* p = NULL;
        list<pair<int, int> > devolver;

        for (list<ComportamientoJugador::Nodo*>::iterator at = abierta.begin(); at != abierta.end(); at++)
            if (fin->getPosicion() == (*at)->getPosicion()) {
                p = *at;
            }

        while (p->getPadre() != NULL && p != NULL) {
            devolver.push_back(p->getPosicion());
            p = p->getPadre();
        }

        return devolver;
    }
};

bool ComportamientoJugador::heAvanzado() {
    if (fila_anterior == fil && columna_anterior == col)
        return false;

    return true;
}

Action ComportamientoJugador::think(Sensores sensores) { // DESPUES DE MORIR NO SE SITUA BIEN, ARREGLAR

    Action accion = actIDLE;

    fila_anterior = fil;
    columna_anterior = col;

    if (itera == 12000) // COMENZAMOS A HACER MISIONES A PARTIR DE LA ITERACION 13000
        misiones = true;

    if (sensores.reset) {
        Reiniciar();
        PorcentajeDescubierto();
    } else {
        Brujula(ultimaAccion, sensores);

        if (puerta > 0 && !sensores.colision && sensores.terreno[2] != 'P') {
            recuerda = false;
            accion = actFORWARD;
            puerta--;
        } else {
            if (!todos_tesoros) // COMPROBAMOS SI PODEMOS COGER ALGUN OBJETO DE DELANTE
                accion = CogerObjeto(sensores);

            if (!cojo_objeto) // COMPROBAMOS SI PODEMOS USAR UN OBJETO
                accion = UsoObjeto(sensores);

            if (!usoobjeto && !cojo_objeto) { // SI NO HEMOS NI COGIDO NI USADO OBJETO, ENTRAMOS
                if (sensores.vida < 200 || (misiones && regalo && reyes.empty()) || (!estoy_ejecutando_plan && (!misiones || error_plan || !bien_situado))) { // SI AUN ESTAMOS SIN MISION O TENEMOS UN FALLO EN EL PLAN
                    //cout << "Me muevo sin plan" << endl;
                    if (sensores.terreno[0] == 'K' && !bien_situado) { // NOS COLOCAMOS ENCIMA DE UN PK
                        fil = sensores.mensajeF;
                        col = sensores.mensajeC;
                        bien_situado = true;
                        coord_k_aux.first = ori_fil_aux;
                        coord_k_aux.second = ori_col_aux;
                        Translada(); // TRASLADAMOS LO QUE HEMOS VISTO ANTES DE ESTAR BIEN SITUADOS
                        mapaResultado[fil][col] = sensores.terreno[0]; // METEMOS EN MAPARESULTADO NUESTROS PIES QUE ES K
                    }

                    if (!bien_situado) {// Aqui estan los dos tipos de movimiento de nuestro personaje
                        //cout << "Llega" << endl;
                        accion = MovimientoPrePK(sensores);
                    } else {
                        //cout << "Aqui" << endl;
                        accion = MovimientoPostPK(sensores);
                    }

                    if (error_plan) {
                        contador_plan++;

                        if (contador_plan == 100) {
                            contador_plan = 0;
                            error_plan = false;
                        }
                    }

                    /**********************************************************************************/
                    // SI TENEMOS OBJETIVO COMO PUEDE SER UN PK CON ESTO NOS DIRIGIREMOS HACIA EL (REACTIVO)
                    if (sensores.terreno[2] != 'P') {
                        if (!recuerda && ((SeePkOrObject(sensores) != -1) || (bien_situado && SeePkOrObject(sensores, fil, col) != -1))) { // Estamos en proceso de alcanzar un objetivo(reactivo)
                            accion = DireccionPK(SeePkOrObject(sensores), sensores);
                            //cout << " Entra" << endl;
                        } else if (recuerda && ultimaAccion == actFORWARD) {
                            recuerda = false;
                            accion = accionRecordar;
                            accionRecordar = actFORWARD;
                        }
                    }
                    /**********************************************************************************/
                } else {
                    if (bien_situado && !estoy_ejecutando_plan) {
                        bool pasa = false;
                        if (!regalo) { // BUSCAMOS EL REGALO MÁS PRÓXIMO
                            int distancia_manhattan = -1;
                            int mas_cercano = 99;

                            if (sensores.regalos.empty())
                                pasa = true;

                            if (!pasa) {
                                for (int i = 0; i < sensores.regalos.size(); i++) { // Aqui tenemos el regalo más cercano
                                    distancia_manhattan = abs(sensores.regalos[i].first - fil) + abs(sensores.regalos[i].second - col);
                                    if (distancia_manhattan < mas_cercano) {
                                        mas_cercano = distancia_manhattan;
                                        objetivo.first = sensores.regalos[i].first;
                                        objetivo.second = sensores.regalos[i].second;
                                    }
                                }
                            }
                        } else if (!reyes.empty()) { // AQUÍ COMENZAMOS LA PLANIFICACION HACIA EL REY MÁS CERCANO
                            int distancia_manhattan = -1;
                            int mas_cercano = 99;

                            if (reyes.empty())
                                pasa = true;

                            if (!pasa)
                                for (auto it = reyes.begin(); it != reyes.end(); it++) { // AQUI SACAMOS EL REY MAS CERCANO
                                    distancia_manhattan = abs((*it).first - fil) + abs((*it).second - col);
                                    if (distancia_manhattan < mas_cercano) {
                                        mas_cercano = distancia_manhattan;
                                        objetivo.first = abs((*it).first);
                                        objetivo.second = abs((*it).second);
                                    }
                                }
                        }

                        if (!pasa) {// AHORA CALCULAMOS LA RUTA HASTA EL REGALO O REY MÁS PROXIMO A NOSOTROS  
                            estado obj;
                            obj.fila = objetivo.first;
                            obj.columna = objetivo.second;
                            estado actual;
                            actual.fila = fil;
                            actual.columna = col;
                            //cout << obj.fila << " " << obj.columna << endl;
                            //cout << actual.fila << " " << actual.columna << endl;
                            if (mapaResultado[obj.fila][obj.columna] != '?') {
                                ComportamientoJugador::AEstrella c(actual, obj, brujula, mapaResultado, sensores, llave, baniador, botas);
                                plan = c.creaPlan();
                                if (!plan.empty()) {
                                    //plan.pop_front();
                                    plan.pop_back();
                                    PintaPlan(plan);
                                    estoy_ejecutando_plan = true;
                                } else {
                                    //cout << "Vacio" << endl;
                                    estoy_ejecutando_plan = false;
                                    error_plan = true;
                                }
                            } else
                                error_plan = true;
                        }
                    }

                    if (estoy_ejecutando_plan && bien_situado && !error_plan) {
                        //cout << "Entra" << endl;
                        if (sensores.colision || sensores.superficie[2] == 'r') { // Hay un fallo en el plan
                            error_plan = true;
                            estoy_ejecutando_plan = false;
                            plan.clear();
                        }

                        if (!plan.empty()) {
                            //cout << "Entra ahora" << endl;
                            accion = plan.front();
                            if(plan.front() == actFORWARD && ((sensores.terreno[0] == 'B' && sensores.terreno[2] == 'A') || (sensores.terreno[0] == 'A' && sensores.terreno[2] == 'B'))){
                                error_plan = true;
                                estoy_ejecutando_plan = false;
                                accion = actIDLE;
                            }
                            plan.pop_front();
                        }

                        if (plan.empty()) {
                            //cout << "Vacio" << endl;
                            estoy_ejecutando_plan = false;
                        }
                    }
                }
            }
        }
        /**********************************************************************************/
        // RELLENAMOS LOS MAPAS
        if (!sensores.colision && bien_situado) { // Aquí rellenaremos el mapaResultado
            DescubreMapa(sensores, fil, col);
            aux_pesos[fil][col]++;
        } else if (!sensores.colision && !bien_situado) // Aquí rellenamos el mapaAuxiliar que luego trasladaremos
            DescubreMapa(sensores, ori_fil_aux, ori_col_aux);
        /**********************************************************************************/
    }

    ultimaAccion = accion;
    cojo_objeto = false;
    itera++;
    usoobjeto = false;

    return accion;
}

int ComportamientoJugador::interact(Action accion, int valor) {
    return false;
}
