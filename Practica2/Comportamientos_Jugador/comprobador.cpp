/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   comprobador.cpp
 * Author: Adrián Jesús Peña Rodríguez
 *
 * Created on April 6, 2017, 4:35 PM
 */

#include <omp.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <iostream>

using namespace std;

unsigned int colSize, filSize;

void setMapa(char * file, char * filea) {
    std::ifstream ifile;
    ifile.open(file, ios::in);
    std::ifstream ifilea;
    ifilea.open(filea, ios::in);
    bool fallo = false;
    
    ifile >> colSize;
    ifile >> filSize;

    char mapa[100][100];
    char mapa_aux[100][100];

    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
            ifile >> mapa[i][j];
    
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
            ifilea >> mapa_aux[i][j];

    for (int i = 0; i < 100 && !fallo; i++) 
        for (int j = 0; j < 100 && !fallo; j++)
            if(mapa[i][j] != mapa_aux[i][j] && mapa_aux[i][j] != '?'){
                fallo = true;
                cout << "\nFallo\n" << endl;
            }
    
    if(!fallo)
        cout << "\nEs correcto\n" << endl;
}

int main(int argc, char ** argv) {
    if (argc != 3) {
        cout << "Primer argumento mapa profesor, segundo el mapa a comprobar\n";
    } else {
        setMapa(argv[1], argv[2]);
    }
    exit(EXIT_SUCCESS);
}