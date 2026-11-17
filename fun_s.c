/*  Arquitectura de Computadores - Ingeniería Informática (Facultad de Informática - EHU)
    Laboratorio de OpenMP - PROYECTO

    fun_s.c        VERSIÓN SECUENCIAL
    Rutinas utilizadas en el programa grupopal.c
*****************************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <float.h>      // DBL_MAX
#include <string.h>
#include <omp.h>

#include "def_gp.h"   // definiciones de constantes y estructuras de datos


/* 1 - Función para calcular la distancia entre dos palabras
       (distancia del coseno)

       Entrada:  dos palabras de la variable DIMENSIONES (por referencia)
       Salida:   distancia (double)
******************************************************************************************/
double dist_pal(float *pal1, float *pal2){
    // POR HACER
    // Calcular la distancia entre dos elementos (distancia de correlación)
    return 0.0;
}


/* 2 - Función para calcular el grupo más cercano (centroide más cercano)

       Entrada:  num_pal  número de palabras, int
                 pal     matriz de tamaño EMAX x DIMENSIONES
                 cent     matriz de tamaño num_clusters x DIMENSIONES
       Salida:   grupo   vector de tamaño EMAX, con el grupo correspondiente a cada palabra
******************************************************************************************/
void grupo_cercano (int num_pal, float pal[][DIMENSIONES], float cent[][DIMENSIONES], int *grupo){
    // POR HACER
    // grupo: centroide más cercano a cada elemento; ese elemento es su «grupo»
}


/* 3 - Validación de la clasificación: compacidad de los grupos y de los centroides
       Se calcula el índice CVI

       Entrada:  pal     palabras del archivo (matriz de tamaño EMAX x DIMENSIONES)
                 grupos   lista de miembros de los grupos (vector de structs de tamaño num_clusters: palabras y cantidad)
                 cent     centroides de los grupos (num_clusters x DIMENSIONES)
       Salida:   índice CVI
                 compacidad    compacidad de los grupos
******************************************************************************************/
double validacion (float pal[][DIMENSIONES], struct cluster *grupos, float cent[][DIMENSIONES], float *compacidad){
  // POR HACER
  // Calcular la compacidad de los grupos: la distancia máxima entre sus miembros
  // Calcular la dispersión de los grupos: para todos los elementos de cada grupo, calcular la distancia mínima a los centroides de los demás grupos y, después, el mínimo de esas distancias.
  // Calcular el índice CVI
  return 0.0;
}


/* 4 - Función para analizar las areas

       Entrada:  grupos   lista de miembros de los grupos (vector de structs de tamaño num_clusters: palabras y cantidad)
                 area     información sobre las areas (EMAX x TEMAS)
       Salida:   dist_area análisis de las areas: medianas máximas y mínimas, y sus grupos
******************************************************************************************/
void analizar_areas (struct cluster *grupos, float area[][TEMAS], struct analisis *dist_area){
  // POR HACER
  // Procesar la información de las areas UNESCO entre los miembros de cada grupo:
  //   Mediana de las medidas de pertenencia a cada área UNESCO.
  //   Para cada código UNESCO, encontrar las medianas máxima y mínima, e identificar el grupo correspondiente.
}


// OTRAS FUNCIONES DEL PROGRAMA PRINCIPAL
// ======================================

/* 5 - Calcular los medoides
**************************************************************/

void guardar_ordenado(struct medoide *medoid, int n, double dist, int pos){
    int i, j;
    for(i=0; i<n; i++){
        if(dist < medoid[i].dist){
            for(j=n-2; j>=i; j--){
                medoid[j+1].dist = medoid[j].dist;
                medoid[j+1].pos = medoid[j].pos;
            }
            medoid[i].dist = dist;
            medoid[i].pos = pos;
            break;
        }
    }
}

void calcular_medoides(float pal[][DIMENSIONES], struct cluster *grupos, struct medoide **medoides){
    int     i, j, k;
    int     tam_grupo, elem1, elem2;
    double  dist, dist_min;
    int     pos_min;

    // Inicializar la estructura
    for(i=0; i<num_clusters; i++){
        for(j=0; j<MEDKNN; j++){
            medoides[i][j].dist = DBL_MAX;
            medoides[i][j].pos = -1;
        }
    }

    // Calcular los medoides y sus vecinos
    for(i=0; i<num_clusters; i++){
        // Calcular el medoide
        tam_grupo = grupos[i].num;
        dist_min = DBL_MAX;
        pos_min = -1;
        for(j=0; j<tam_grupo; j++){
            elem1 = grupos[i].elem[j];
            dist = 0.0;
            for(k=0; k<tam_grupo; k++){
                elem2 = grupos[i].elem[k];
                dist += dist_pal(pal[elem1], pal[elem2]);
            }
            if(dist < dist_min){
                dist_min = dist;
                pos_min = elem1;
            }
        }
        if(pos_min!=-1){
            medoides[i][0].dist = 0.0;
            medoides[i][0].pos = pos_min;
        }

        // Buscar los vecinos del medoide
        for(j=0; j<grupos[i].num; j++){
            elem2 = grupos[i].elem[j];
            if(elem2!=pos_min){
                dist = dist_pal(pal[pos_min], pal[elem2]);
                guardar_ordenado(medoides[i]+1, MEDKNN-1, dist, elem2);
            }
        }
    }
}


/* 6 - Valores iniciales de los centroides
**************************************************************/
void inicializar_centroides (float cent[][DIMENSIONES])
{
  int  i, j;
  float rnd;
  srand (13);
  for (i=0; i<num_clusters; i++)
  for (j=0; j<DIMENSIONES/2; j++){
    rnd = ((rand() % 10000) / 10000.0)*8-4;
    cent[i][j] = rnd;
    cent[i][j+DIMENSIONES/2] = rnd;
  }
}



/* 7 - Calcular los nuevos centroides y decidir si se debe terminar o no
**************************************************************/

int calc_centroides (float pal[][DIMENSIONES], float cent[][DIMENSIONES], int *grupo, int num_pal)
{
    int     i, j, fin;
    float   nuevo_cent[num_clusters][DIMENSIONES];
    double  dis_cent;
    double  sumas[num_clusters][DIMENSIONES+1];

    fin = 1;     // 1: la simulación ha terminado; 0: hay que continuar

    // Nuevos centroides: medias de las variables de cada dimensión de los miembros
    // grupo: grupo al que pertenece cada palabra
    // sumas: suma de los valores de las variables de los miembros de cada grupo; el último elemento guarda la cantidad

    for (i=0; i<num_clusters; i++)
    for (j=0; j<DIMENSIONES+1; j++)
      sumas[i][j] = 0.0;

    // Se procesan muchas palabras en paralelo, aunque la mayor parte del cálculo
    // se realiza en una variable de reducción.
    for (i=0; i<num_pal; i++){
      for (j=0; j<DIMENSIONES; j++)
        sumas[grupo[i]][j] += pal[i][j];  // sumar los valores de los miembros de cada grupo para cada variable

      sumas[grupo[i]][DIMENSIONES] ++;         // el último elemento (DIMENSIONES) guarda la cantidad
    }

    // Calcular los nuevos centroides de los grupos y decidir si se debe terminar (DELTA1)
    for (i=0; i<num_clusters; i++)
    {
      if (sumas[i][DIMENSIONES] > 0)        // el grupo no está vacío
      {
        // «Coordenadas» de los nuevos centroides: medias de los miembros
        for (j=0; j<DIMENSIONES; j++)
          nuevo_cent[i][j] = sumas[i][j] / sumas[i][DIMENSIONES];

        // Decidir si se debe terminar
        dis_cent = dist_pal (&nuevo_cent[i][0], &cent[i][0]);
        if (dis_cent > DELTA1)  fin = 0; // el centroide del grupo se ha movido más que DELTA1; continuar la simulación

        // Copiar el nuevo centroide
        for (j=0; j<DIMENSIONES; j++)
          cent[i][j] = nuevo_cent[i][j];
      }
    }

    return (fin);
}


/* 8 - Guardar los códigos UNESCO y sus números de línea para imprimirlos después
**************************************************************/
void inicializar_codigos_unesco(struct unesco cod_unesco[]){
  cod_unesco[0].linea = 8015;
  strcpy(cod_unesco[0].nombre, "logic");
  cod_unesco[1].linea = 6934;
  strcpy(cod_unesco[1].nombre, "mathematics");
  cod_unesco[2].linea = 14452;
  strcpy(cod_unesco[2].nombre, "astronomy");
  cod_unesco[3].linea = 5913;
  strcpy(cod_unesco[3].nombre, "physics");
  cod_unesco[4].linea = 6298;
  strcpy(cod_unesco[4].nombre, "chemistry");
  cod_unesco[5].linea = 210;
  strcpy(cod_unesco[5].nombre, "life");
  cod_unesco[6].linea = 1896;
  strcpy(cod_unesco[6].nombre, "earth");
  cod_unesco[7].linea = 2277;
  strcpy(cod_unesco[7].nombre, "agriculture");
  cod_unesco[8].linea = 743;
  strcpy(cod_unesco[8].nombre, "medical");
  cod_unesco[9].linea = 766;
  strcpy(cod_unesco[9].nombre, "technology");
  cod_unesco[10].linea = 14797;
  strcpy(cod_unesco[10].nombre, "anthropology");
  cod_unesco[11].linea = 6288;
  strcpy(cod_unesco[11].nombre, "demographics");
  cod_unesco[12].linea = 259;
  strcpy(cod_unesco[12].nombre, "economic");
  cod_unesco[13].linea = 4461;
  strcpy(cod_unesco[13].nombre, "geography");
  cod_unesco[14].linea = 312;
  strcpy(cod_unesco[14].nombre, "history");
  cod_unesco[15].linea = 3737;
  strcpy(cod_unesco[15].nombre, "judicial");
  cod_unesco[16].linea = 19675;
  strcpy(cod_unesco[16].nombre, "linguistics");
  cod_unesco[17].linea = 37538;
  strcpy(cod_unesco[17].nombre, "pedagogy");
  cod_unesco[18].linea = 215;
  strcpy(cod_unesco[18].nombre, "politic");
  cod_unesco[19].linea = 6991;
  strcpy(cod_unesco[19].nombre, "psychology");
  cod_unesco[20].linea = 731;
  strcpy(cod_unesco[20].nombre, "art");
  cod_unesco[21].linea = 12425;
  strcpy(cod_unesco[21].nombre, "sociology");
  cod_unesco[22].linea = 5424;
  strcpy(cod_unesco[22].nombre, "ethics");
  cod_unesco[23].linea = 4268;
  strcpy(cod_unesco[23].nombre, "philosophy");
}
