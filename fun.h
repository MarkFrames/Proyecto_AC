/*  Arquitectura de Computadores - Ingeniería Informática (Facultad de Informática - EHU)
    Laboratorio de OpenMP - PROYECTO

    funth.h
    Prototipos de las funciones utilizadas en el archivo taldegen
*******************************************************************/

extern void   inicializar_centroides
              	(float cent[][DIMENSIONES]);

extern int    calc_centroides
		(float pal[][DIMENSIONES],
		 float cent[][DIMENSIONES],
		 int   *grupo,
		 int   num_pal
		);

extern double dist_pal
		(float *pal1,
		 float *pal2
		);

extern void   grupo_cercano
	        (int   num_pal,
	         float pal[][DIMENSIONES],
	         float cent[][DIMENSIONES],
	         int   *grupo
	        );

extern double validacion
            	(float  pal[][DIMENSIONES],
 	         struct cluster *grupos,
		 float  cent[][DIMENSIONES],
		 float  *compacidad
		);

extern void   analizar_areas
        	(struct cluster *grupos,
		 float  area[][TEMAS],
		 struct analisis *dist_area
		);

extern void calcular_medoides(
	float pal[][DIMENSIONES],
	struct cluster *grupos,
	struct medoide **medoides
);

extern void inicializar_codigos_unesco(
	struct unesco codigos_unesco[]
);
