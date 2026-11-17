/*  Arquitectura de Computadores - Ingeniería Informática (Facultad de Informática - EHU)
    Laboratorio de OpenMP - PROYECTO

    defineth.h
    Constantes y estructuras de datos utilizadas en los archivos multzopal y funth
**********************************************************************************/


// Constantes

#define EMAX        170000  // número máximo de elementos (palabras)
#define MAX_GRUPOS 200    // número máximo de grupos
#define DIMENSIONES     50      // número de variables de cada palabra
#define DELTA1      0.005   // convergencia: cambio mínimo de los centroides
#define DELTA2      0.0000001 // convergencia: clasificación (CVI)
#define ITMAX       10000   // convergencia: número máximo de iteraciones
#define TEMAS      24
#define MEDKNN      10

extern int  num_clusters;      // número de grupos en cada iteración de la simulación


// Estructuras de datos

struct cluster          // información sobre los grupos
{
  int  elem[EMAX];     // elementos
  int  num;                // número de elementos
};


struct analisis            // resultados del análisis de las areas
{
  float  mmax, mmin;       // mediana máxima y mínima para cada área
  int    grupo_max, grupo_min; // grupos con los valores máximo y mínimo
};

struct medoide {
    double dist;
    int pos;
};

struct unesco {
    int linea;
    char nombre[32];
};
