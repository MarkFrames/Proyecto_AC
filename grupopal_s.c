/*  Arquitectura de Computadores - Ingeniería Informática (Facultad de Informática - EHU)
    Laboratorio de OpenMP - PROYECTO

    grupo_pal.c     VERSIÓN SECUENCIAL

    Procesamiento de vectores de palabras para obtener información sobre areas de ciencia y tecnología.
    Clasificación de las palabras de la variable DIMENSIONES en «num_clusters» grupos, según las «distancias».

    Entrada: vecpal.dat  archivo que contiene vectores de palabras
             campos.dat         archivo con información sobre areas de ciencia y tecnología
    Salida:  res.out     centroides, número de grupos, su tamaño y compacidad
                                información sobre las areas de ciencia y tecnología

    Compilar con el módulo fun_s.c; añadir la opción -lm
*****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "def_gp.h"               // definiciones de constantes y estructuras de datos
#include "fun.h"                  // funciones llamadas en el programa

char  *nombre_pal[EMAX];             // palabras de la base de datos, como strings

float             pal[EMAX][DIMENSIONES];  // palabras que se deben procesar (en vecpal.dat)
struct cluster grupos[MAX_GRUPOS]; // lista de miembros de cada grupo

float            area[EMAX][TEMAS];    // datos sobre las areas de las palabras (en campos.dat)
struct analisis  dist_area[TEMAS];       // probabilidades de las areas: medianas máximas, mínimas...

struct medoide **medoides;             // medoides y sus MEDKNN vecinos más cercanos para cada clúster

struct unesco cod_unesco[TEMAS];    // nombres de las areas de la clasificación de la UNESCO

int  num_clusters = 35;                    // número inicial de grupos



// ================================
//       PROGRAMA PRINCIPAL
// ================================

int main (int argc, char *argv[])
{
  float   cent[MAX_GRUPOS][DIMENSIONES]; // centroides (grupos)
  float   compacidad[MAX_GRUPOS];   // compacidad de cada grupo

  int     i, j, num_pal, grp;
  int     grupo[EMAX];                 // grupo asignado a cada palabra
  int     fin = 0, iter = 0, fin_clasificacion;
  double  cvi, cvi_ant, dif;

  FILE    *f1, *f2;
  struct timespec  t1, t2, t3, t4, t5, t6;
  double  t_lectura, t_clasificacion, t_areas, t_escritura, t_med;

  inicializar_codigos_unesco(cod_unesco);

  if ((argc < 3) || (argc > 4)) {
    printf ("AVISO:  progr fichero_palabras fichero_areas [n.º de palabras]\n");
    exit (-1);
  }

  printf ("\n >> Ejecucion secuencial\n");
  clock_gettime (CLOCK_MONOTONIC, &t1);

  // Leer los datos de los archivos de entrada
  // ==========================================

  // Vectores de palabras >> pal[i][j]

  f1 = fopen (argv[1], "r");
  if (f1 == NULL) {
    printf ("Error al abrir el archivo %s\n", argv[1]);
    exit (-1);
  }

  num_pal = EMAX;
  if (argc == 4) num_pal = atoi (argv[3]);  // 4.º parámetro = número de palabras que se deben procesar
  num_pal = num_pal > EMAX ? EMAX : num_pal;

  for (i=0; i<num_pal; i++){
    nombre_pal[i] = (char *)malloc(100*sizeof(char));
    fscanf (f1, "%s ", nombre_pal[i]);
    for (j=0; j<DIMENSIONES; j++){
      fscanf (f1, "%f", &(pal[i][j]));      // lista de palabras, de la variable DIMENSIONES
    }
  }
  fclose (f1);

  // Información sobre las areas (ztalor) >> area[i][j]
  f1 = fopen (argv[2], "r");
  if (f1 == NULL) {
    printf ("Error al abrir el archivo %s\n", argv[2]);
    exit (-1);
  }

  for (i=0; i<num_pal; i++)
  for (j=0; j<TEMAS; j++)
    fscanf (f1, "%f", &(area[i][j]));        // información sobre las areas

  fclose (f1);

  clock_gettime (CLOCK_MONOTONIC, &t2);

  // FASE 1: proceso iterativo para clasificar las palabras en «num_clusters» grupos
  // hasta que la diferencia en el índice CVI sea menor que el valor DELTA2.
  // ============================================================================

  fin_clasificacion = 0;
  cvi_ant = -1;

  while (num_clusters < MAX_GRUPOS && fin_clasificacion == 0)
  {
    // Elegir los primeros centroides al azar
    inicializar_centroides (cent);

    // A. Proceso de clasificación en «num_clusters» grupos
    // ================================================
    iter = 0;
    fin = 0;
    while ((fin == 0) && (iter < ITMAX))
    {
      // Calcular el grupo más cercano
      /** COMPLETAR en el archivo funth_s.c **/
      grupo_cercano (num_pal, pal, cent, grupo);

      // Calcular los nuevos centroides de los grupos: la media de cada dimensión de sus miembros
      // Si el cambio en los centroides es menor que DELTA1, establecer fin = 1
      /** COMPLETAR en el archivo funth_s.c **/
      fin = calc_centroides (pal, cent, grupo, num_pal);

      iter ++;
    }
    printf ("   Numero de grupos: %d -- Numero de iteraciones: %d\n", num_clusters, iter);

    // B. Calidad de la clasificación
    // ===============================
    for (i=0; i<num_clusters; i++)
        grupos[i].num = 0;

    // Palabras (elementos) de cada grupo y su número
    for (i=0; i<num_pal; i++)
    {
      grp = grupo[i];
      grupos[grp].elem[grupos[grp].num] = i;
      grupos[grp].num ++;
    }

    // Validación de la clasificación y convergencia
    /** COMPLETAR en el archivo funth_s.c **/
    cvi = validacion (pal, grupos, cent, compacidad);

    dif = cvi - cvi_ant;
    if (dif < DELTA2 || (num_clusters+10)>MAX_GRUPOS)
        fin_clasificacion = 1;
    else {
      num_clusters += 10;
      cvi_ant = cvi;
    }
  }
  clock_gettime (CLOCK_MONOTONIC, &t3);

 // Calcular el medoide de cada grupo y sus MEDKNN palabras más cercanas

  medoides = (struct medoide **)malloc(num_clusters*sizeof(struct medoide *));
  for(i=0; i<num_clusters; i++){
    medoides[i] = (struct medoide *)malloc(MEDKNN*sizeof(struct medoide));
  }
  calcular_medoides(pal, grupos, medoides);

  clock_gettime (CLOCK_MONOTONIC, &t4);

  // FASE 2: analizar las areas
  // =========================

  // Analisis de las areas de los grupos: medianas máximas, mínimas...
  /** COMPLETAR en el archivo funth_s.c **/
  analizar_areas (grupos, area, dist_area);

  clock_gettime (CLOCK_MONOTONIC, &t5);

  // Escribir los resultados en el archivo res.out
  // ====================================================

  f2 = fopen ("res.out", "w");
  if (f2 == NULL) {
    printf ("Error al abrir el archivo res.out\n");
    exit (-1);
  }

  fprintf (f2, " >> Centroides de los grupos\n");
  for (i=0; i<num_clusters; i++) {
    for (j=0; j<DIMENSIONES; j++) fprintf (f2, "%7.3f", cent[i][j]);
    fprintf (f2,"\n");
  }

  fprintf (f2, "\n >> Tamaño de los grupos: %d grupos\n\n", num_clusters);
  for (i=0; i<num_clusters/10; i++) {
    for (j=0; j<10; j++) fprintf (f2, "%9d", grupos[10*i+j].num);
    fprintf (f2, "\n");
  }
  for (i=i*10; i<num_clusters; i++) fprintf (f2, "%9d", grupos[i].num);
  fprintf (f2, "\n");

  fprintf (f2, "\n >> Compacidad de los grupos\n\n");
  for (i=0; i<num_clusters/10; i++) {
    for (j=0; j<10; j++) fprintf (f2, "%9.2f", compacidad[10*i+j]);
    fprintf (f2, "\n");
  }
  for (i=i*10; i<num_clusters; i++) fprintf (f2, "%9.2f", compacidad[i]);
  fprintf (f2, "\n");

  fprintf (f2, "\n >> Analisis de las areas (medianas)\n");
  fprintf (f2, "\n Area.  M_max - Grupo   M_min - Grupo");
  fprintf (f2, "\n ====================================\n");
  for (i=0; i<TEMAS; i++)
    fprintf (f2, "  %2d     %4.2f - %2d      %4.2f - %2d\n", i,
                dist_area[i].mmax, dist_area[i].grupo_max,
                dist_area[i].mmin, dist_area[i].grupo_min);

  fclose (f2);

  clock_gettime (CLOCK_MONOTONIC, &t6);


  // Mostrar algunos resultados en pantalla
  // =======================================

  t_lectura   = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / (double)1e9;
  t_clasificacion = (t3.tv_sec - t2.tv_sec) + (t3.tv_nsec - t2.tv_nsec) / (double)1e9;
  t_med    = (t4.tv_sec - t3.tv_sec) + (t4.tv_nsec - t3.tv_nsec) / (double)1e9;
  t_areas    = (t5.tv_sec - t4.tv_sec) + (t5.tv_nsec - t4.tv_nsec) / (double)1e9;
  t_escritura = (t6.tv_sec - t5.tv_sec) + (t6.tv_nsec - t5.tv_nsec) / (double)1e9;


  printf ("\n   Tiempos de ejecucion\n");
  printf ("\n   T_lectura:       %7.3f s", t_lectura);
  printf ("\n   T_clasificacion: %7.3f s", t_clasificacion);
  printf ("\n   T_medoides:      %7.3f s", t_med);
  printf ("\n   T_areas:         %7.3f s", t_areas);
  printf ("\n   T_escritura:     %7.3f s", t_escritura);
  printf ("\n   ===============================");
  printf ("\n   T_TOTAL:         %7.3f s\n\n", t_lectura + t_clasificacion + t_areas + t_escritura);

  // Escribir algunos resultados en pantalla
  // ========================================
  printf ("\n >> Centroides 0, 20, 40...\n");
  for (i=0; i<num_clusters; i+=20) {
    printf ("\n  cent%2d -- ", i);
    for (j=0; j<DIMENSIONES; j++) printf ("%7.3f", cent[i][j]);
    printf("\n");
  }

  printf ("\n >> Tamaño de los grupos: %d grupos\n\n", num_clusters);
  for (i=0; i<num_clusters/10; i++) {
    for (j=0; j<10; j++) printf ("%9d", grupos[10*i+j].num);
    printf("\n");
  }
  for (i=i*10; i<num_clusters; i++) printf ("%9d", grupos[i].num);
  printf ("\n");

  printf ("\n >> Compacidad de los grupos\n\n");
  for (i=0; i<num_clusters/10; i++) {
    for (j=0; j<10; j++) printf ("%9.2f", compacidad[10*i+j]);
    printf("\n");
  }
  for (i=i*10; i<num_clusters; i++) printf ("%9.2f", compacidad[i]);
  printf ("\n");

  printf ("\n >> Analisis de las areas (medianas)\n");
  printf ("\n Area.  M_max - Grupo   M_min - Grupo");
  printf ("\n ====================================\n");
  for (i=0; i<TEMAS; i++){
    printf ("  Area: %2d (%-12s)   %4.2f - %3d (%-12s)   %4.2f - %3d (%-12s)\n",
      i, cod_unesco[i].nombre,
      dist_area[i].mmax, dist_area[i].grupo_max, nombre_pal[medoides[dist_area[i].grupo_max][0].pos],
      dist_area[i].mmin, dist_area[i].grupo_min, nombre_pal[medoides[dist_area[i].grupo_min][0].pos]);
  }
  printf("\n\n");

  printf("\n>> Medoides de los grupos:\n\n");
  for(i=0; i<num_clusters; i++){
    printf("Grupo: %3d; Medoide:", i);
    for(j=0; j<MEDKNN; j++){
      if(medoides[i][j].pos!=-1){
        printf(" %s", nombre_pal[medoides[i][j].pos]);
      } else {
        break;
      }
    }
    printf("\n");
  }

  // Liberar nombre_pal
  for (i=0; i<num_pal; i++){
    free(nombre_pal[i]);
  }

  // Liberar medoides
  for(i=0; i<num_clusters; i++){ free(medoides[i]); }
  free(medoides);
}
