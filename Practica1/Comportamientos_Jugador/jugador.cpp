#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

Action ComportamientoJugador::Reset(Sensores sensores) { // ARREGLAR
    bien_situado = false;

    ultimaAccion = actIDLE;
    brujula = 0;
}

void ComportamientoJugador::Brujula(Action ultima, Sensores sensores) {
    if (!sensores.colision)
        switch (ultima) {
            case actFORWARD:
                switch (brujula) {
                    case 0: // Norte
                        fil--;
                        break;
                    case 1: // Este
                        col++;
                        break;
                    case 2: // Sur
                        fil++;
                        break;
                    case 3: // Oeste
                        col--;
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

int ComportamientoJugador::SeePkOrObject(Sensores sensores) {
    for (int i = 0; i < 16; i++)
        if (sensores.terreno[i] == 'K' || sensores.superficie[i] == '1' || sensores.superficie[i] == '2')
            return i;

    return -1;
}

Action ComportamientoJugador::Puertas(Sensores sensores) {
    Action devolver = actIDLE;

    if (puerta_L) {
        puerta_L = false;
        puerta = true;
        devolver = actTURN_L;
    } else if (puerta_R) {
        puerta_R = false;
        puerta = true;
        devolver = actTURN_R;
    } else if (sensores.objetoActivo == '_' && sensores.mochila == '3' && sensores.superficie[2] == 'd') {
        puerta = true;
        devolver = actPOP;
    } else if (sensores.superficie[1] == 'd') { // ARREGLAR
        puerta = true;
        puerta_L = true;
        devolver = actFORWARD;
    } else if (sensores.superficie[3] == 'd') {
        puerta = true;
        puerta_R = true;
        devolver = actFORWARD;
    } else if (sensores.superficie[5] == 'd' || sensores.superficie[7] == 'd') {
        puerta = true;
        devolver = actFORWARD;
    } else if (sensores.superficie[2] == 'd' && sensores.mochila == '3') {
        puerta = true;
        devolver = actTHROW;
    } else if (sensores.superficie[2] == 'd' && sensores.objetoActivo == '3') {
        explora_sala = dentro_sala = true;
        puerta = true;
        devolver = actGIVE;
    }
    
    return devolver;
}

Action ComportamientoJugador::DireccionPK(int i, Sensores sensores) {
    recuerda = true;

    if ((i > 8 && i < 12) || (i > 3 && i < 6) || i == 1) {
        accionRecordar = actTURN_R;
        return actTURN_L;
    } else if ((i > 12 && i < 16) || (i > 6 && i < 9) || i == 3) {
        accionRecordar = actTURN_L;
        return actTURN_R;
    }

    if (sensores.superficie[2] == '1' || sensores.superficie[2] == '2')
        return CogerObjeto(sensores);

    return actFORWARD;
}

Action ComportamientoJugador::CogerObjeto(Sensores sensores) {
    if (objeto_delante || (sensores.superficie[2] == '0' || sensores.superficie[2] == '1' || sensores.superficie[2] == '2' || sensores.superficie[2] == '3') && !sensores.colision && ultimaAccion != actPICKUP && ultimaAccion != actPUSH) {
        mutex_objeto = true;

        if (sensores.objetoActivo != '_') {
            objeto_delante = true;
            return actPUSH;
        } else {
            objeto_delante = false;
            return actPICKUP;
        }

        objetos_mochila++;

        if (objetos_mochila >= 4)
            objetos_mochila++;

        if (objetos_mochila >= 4)
            return actTHROW;
    } else
        return actIDLE;
}

Action ComportamientoJugador::UsoObjeto(Sensores sensores) {

    if (sensores.terreno[2] == 'B')
        if (sensores.objetoActivo == '2')
            return actFORWARD;
        else if (sensores.objetoActivo != '2' && sensores.objetoActivo != '_' && sensores.mochila == '2' && sensores.terreno[0] != 'A')
            return actTHROW;
        else if (sensores.mochila == '2' && sensores.terreno[0] != 'A')
            return actPOP;
        else
            return actIDLE;
    else if (sensores.terreno[2] == 'A')
        if (sensores.objetoActivo == '1')
            return actFORWARD;
        else if (sensores.objetoActivo != '1' && sensores.objetoActivo != '_' && sensores.mochila == '1' && sensores.terreno[0] != 'B')
            return actTHROW;
        else if (sensores.mochila == '1' && sensores.terreno[0] != 'B')
            return actPOP;
        else
            return actIDLE;
    else if (sensores.superficie[2] == 'l')
        if (sensores.objetoActivo == '0')
            return actGIVE;
        else if (sensores.objetoActivo != '0' && sensores.objetoActivo != '_' && sensores.mochila == '0')
            return actTHROW;
        else if (sensores.mochila == '0')
            return actPOP;
        else
            return actIDLE;
    else
        return actIDLE;
}

void ComportamientoJugador::DescubreMapa(Sensores sensores) {
    int aux_f;
    int aux_c;

    switch (brujula) {
        case 0: // NORTE
            aux_f = -1;
            aux_c = -1;

            for (int i = 1; i < 16; i++) {
                if (mapaResultado[fil + aux_f][col + aux_c] == '?')
                    mapaResultado[fil + aux_f][col + aux_c] = sensores.terreno[i];

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
                if (mapaResultado[fil + aux_f][col + aux_c] == '?')
                    mapaResultado[fil + aux_f][col + aux_c] = sensores.terreno[i];

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
                if (mapaResultado[fil + aux_f][col + aux_c] == '?')
                    mapaResultado[fil + aux_f][col + aux_c] = sensores.terreno[i];

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
                if (mapaResultado[fil + aux_f][col + aux_c] == '?')
                    mapaResultado[fil + aux_f][col + aux_c] = sensores.terreno[i];

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
}

void ComportamientoJugador::Precipicios() {
    precipicios = true;

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
}

Action ComportamientoJugador::Movimiento(Sensores sensores) {
    static int aleatorio;
    static int contador = 0;

    aleatorio = rand() % 2;

    if (sensores.colision || sensores.terreno[2] == 'P') {
        contador == 0;
        return actTURN_R;
    } else if ((sensores.terreno[2] == 'T' || sensores.terreno[2] == 'S' || sensores.terreno[2] == 'K') && sensores.superficie[2] == '_' && contador < 10) {
        contador++;
        return actFORWARD;
    }
    if ((sensores.colision || sensores.terreno[2] == 'P') && ultimaAccion != actTURN_L) {
        contador == 0;
        return actTURN_R;
    } else if ((sensores.colision || sensores.superficie[2] == 'l' || sensores.superficie[2] == 'a') && ultimaAccion != actTURN_R)
        return actTURN_L;
    else
        if (aleatorio == 0) {
        if ((sensores.terreno[2] == 'T' || sensores.terreno[2] == 'S' || sensores.terreno[2] == 'K') && sensores.superficie[2] == '_')
            contador = 0;

        return actTURN_L;
    } else
        return actTURN_R;

}

Action ComportamientoJugador::think(Sensores sensores) {

    Action accion = actIDLE;

    if (sensores.reset) {
        Reset(sensores);
        return actIDLE;
    } else if (!precipicios) // DESCUBRE LOS PRECIPIOS EN MAPA DE 100x100, SOLO SE ENTRA UNA VEZ EN AQUÍ
        Precipicios();

    Brujula(ultimaAccion, sensores);

    if (sensores.mochila == '3' || sensores.objetoActivo == '3')
        accion = Puertas(sensores);

    if (!explora_sala && pasos_sala < 5 && dentro_sala) {
        if (pasos_sala == 3 || pasos_sala == 4)
            accion = actTURN_L;
        pasos_sala--;
        if (pasos_sala < 2)
            accion = actFORWARD;
        if (pasos_sala == -8)
            dentro_sala = false;
    }

    if (explora_sala && pasos_sala < 5 && accion == actIDLE) {
        dentro_sala = true;
        pasos_sala++;
        if (pasos_sala == 4)
            explora_sala = false;
        accion = actFORWARD;
    }

    if (accion == actIDLE && !puerta)
        accion = CogerObjeto(sensores);

    if (!mutex_objeto && !puerta && !dentro_sala && accion == actIDLE)
        accion = UsoObjeto(sensores);

    if (accion == actIDLE && !puerta) {
        if (sensores.terreno[0] == 'K' and !bien_situado) {
            fil = sensores.mensajeF;
            col = sensores.mensajeC;
            bien_situado = true;
            mapaResultado[fil][col] = sensores.terreno[0];
        }

        accion = Movimiento(sensores);
    }

    if (!sensores.colision && bien_situado) {
        DescubreMapa(sensores);
    } else if (recuerda && ultimaAccion == actFORWARD) {
        recuerda = false;
        accion = accionRecordar;
        accionRecordar = actFORWARD;
    } else if (SeePkOrObject(sensores) != -1 && !recuerda) // Ha visto el PK o un objeto
        accion = DireccionPK(SeePkOrObject(sensores), sensores);

    ultimaAccion = accion;
    mutex_objeto = false;
    puerta = false;

    return accion;
}

int ComportamientoJugador::interact(Action accion, int valor) {
    return false;
}
