#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "Map.h"
#include "list.h"

//Almacenar los datos...
typedef struct{
  int coor[2];
  int id;
}coordenadas;

//GRAFO IMPLICITO
typedef struct{
  char* nombre;
  int main[2];
  int last;
  int total;
}rutas;

//Estructura de la LISTA
typedef struct{
  char* nombre;
  int id[10];
  int distancia[10];
}nodos;

//Estructura para la funcion 3...
typedef struct{
  int id;
  int distancia;
}best;

int lower_than_int(void * key1, void * key2) {
  if(*(int*)key1 < *(int*)key2) return 1;
  return 0;
}

int is_equal_int(void * key1, void * key2) {
  if(*(int*)key1 == *(int*)key2) return 1;
  return 0;
}

void menu();
void archivos(Map*);
int conversion(char *);
void distanciaEntregas(Map*);
int calculo(int[], int[]);
void mejores3(Map*);
int compare(const void*, const void*);
void crearRUTA(Map *, Map*, List*);
bool repetido(int[], int);
void restantes(Map*, int[], int[]);
void generarRUTA(Map*, Map*, List*);
void mostrarRUTAS(Map*, List*);

int main(void) {
  Map* save = createMap(is_equal_int); //Almacenar los datos...
  Map* ways = createMap(is_equal_int); //Grafo IMPLICITO
  List* connect = create_list(); //Lista que contempla con el grafo

  setSortFunction(save, lower_than_int);
  setSortFunction(ways, lower_than_int);
  menu();

  int num;
  scanf("%i", &num);
  while(num != 0)
  {
    switch(num)
    {
      case 1: archivos(save); break;
      case 2: distanciaEntregas(save); break;
      case 3: mejores3(save); break;
      case 4: crearRUTA(save, ways, connect); break;
      case 5: generarRUTA(save, ways, connect); break;
      case 7: mostrarRUTAS(ways, connect); break;
      default: printf("opción invalida\n"); break;
    }
    menu();
    scanf("%i", &num);
  }

  printf("\n Este programa se finalizo. BYE");
  return 0;
}

// OPCION 1 | "LISTA"
void archivos(Map* save)
{
  char csv[1000];
  printf("\nIngrese nombre del archivo\n");
  scanf("%s", csv);
  FILE *archivo = fopen(csv, "r");

  if(archivo == NULL)
  {
    printf("\nError al abrir el archivo\n");
    exit(EXIT_FAILURE);
  }

  char linea[10];
  int i, j, k, num, cont = 1;
  while(fscanf(archivo, "%9[^\n]s", linea) != EOF)
  {
    fgetc(archivo);
    coordenadas* lugar = malloc(sizeof(coordenadas));

    char x[5];
    char y[5];
    i = 0;


    lugar->id = cont;

    //COORDENADAS X
    while(linea[i] == ' ')
      i++;

    for(j = 0; linea[i] != ' '; i++, j++)
      x[j] = linea[i];
    x[j] = '\0';

    num = conversion(x);
    lugar->coor[0] = num;

    //COORDENADAS Y
    while(linea[i] == ' ')
      i++;

    for(k = 0; linea[i] != ' ' && linea[i] != '\0'; i++, k++)
      y[k] = linea[i];
    y[k] = '\0';

    num = conversion(y);
    lugar->coor[1] = num;

    insertMap(save, &lugar->id, lugar);
    cont++;

  }

  printf("------------------\n");
  printf("Archivo leido...\n");
  printf("------------------\n");
}

//Funcion para convertir un numero en string a dato numerico...
int conversion(char *numero)
{
  int i;
  int digito, decimas, aux;
  int suma = 0, potencia = -1;

  for(i = 0; numero[i] != '\0'; i++)
    potencia++;

  aux = potencia;

  if(potencia == 0)
  {
    digito = numero[0] - 48;
    return digito;
  }
  else
  {
    for(int k = 0; k < i; k++)
    {
      decimas = 1;
      digito = numero[k] - 48;

      for(;aux > 0; aux--)
        decimas *= 10;

      digito *= decimas;
      suma += digito;
      potencia--;
      aux = potencia;
    }

    return suma;
  }

}

// OPCION 2 | "LISTA"
void distanciaEntregas(Map* save)
{
  int coor1[2], coor2[2];
  int numID, sameID;
  coordenadas* aux;


  printf("\nIngrese el ID de una entrega\n ID = ");
  while(1)
  {
    scanf("%i", &numID);
    aux = searchMap(save, &numID);
    if(aux != NULL)
    {
      sameID = numID;
      break;
    }
    printf("El id que ingreso (%i) no se encuentra en el almacenamiento. Favor de ingresar otro\n\n ID = ", numID);
  }

  coor1[0] = aux->coor[0];
  coor1[1] = aux->coor[1];


  printf("\n Ahora ingrese el ID de otra entrega\n ID = ");
  while(1)
  {
    scanf("%i", &numID);
    if(numID == sameID)
      printf("\nEl ID que ingresó ya fue seleccionado anteriormente, favor de ingresar uno diferente\n\n ID = ");
    else
    {
      aux = searchMap(save, &numID);
      if(aux != NULL)
        break;

      printf("El id que ingreso (%i) no se encuentra en el almacenamiento. Favor de ingresar otro\n\n ID = ", numID);
    }
  }

  coor2[0] = aux->coor[0];
  coor2[1] = aux->coor[1];

  int distancia = calculo(coor1, coor2);

  printf("\nLa distancia entre la entrega %i y %i es %i[metros] aproximadamente\n", sameID, numID, distancia);
}

//Funcion para calcular las distancia entre 2 puntos...
int calculo(int a[2], int b[2])
{
  int x, y, suma;
  int divisor = 1;

  x = b[0] - a[0]; x *= x;
  y = b[1] - a[1]; y *= y;
  suma = x + y;

  while(suma/divisor > divisor)
    divisor++;

  return divisor;
}

// OPCION 3 | "LISTA"
void mejores3(Map* save)
{
  int user[2];

  printf("\nIngrese sus coordenadas (x,y)\nX = ");
  scanf("%i", &user[0]);
  printf("Y = ");
  scanf("%i", &user[1]);
  coordenadas* aux = firstMap(save);

  if(aux != NULL)
  {
    best datos[48];
    int i;

    for(i = 0; i < 48; i++)
    {
      datos[i].id = aux->id;
      datos[i].distancia = calculo(user, aux->coor);
      aux = nextMap(save);
    }

    qsort(datos, 48, sizeof(best), compare);
    printf("\nLas 3 Mejores Rutas desde tus coordenadas(%i , %i) son:\n", user[0], user[1]);

    for(i = 0; i < 3; i++)
      printf("ID = %i, DISTANCIA = %i\n", datos[i].id, datos[i].distancia);
  }
  else
    printf("\n No hay coordenadas de entrega registradas...\n");
}

int compare(const void* a, const void* b)
{
  best *ptrA = (best *)a;
  best *ptrB = (best *)b;

  if(ptrA->distancia > ptrB->distancia)
    return 1;

  return 0;
}


// OPCION 4 | LISTO (evitar usar el mismo nombre)
void crearRUTA(Map* save, Map* ways, List* connect)
{
  rutas* way = malloc(sizeof(rutas));
  nodos* conexion = malloc(sizeof(nodos));

  int same[10];
  int id, i, suma = 0;

  coordenadas* aux;
  coordenadas* anterior;

  printf("\nIngrese sus coordenadas actuales (x,y)\nX = ");
  scanf("%i", &way->main[0]);
  printf("Y = ");
  scanf("%i", &way->main[1]);

  printf("\nAhora ingresara los 10 ID de entrega\n");

  for(i = 1; i < 10; i++)
  {
    if(i == 1)
    {
      restantes(save, same, way->main);
      printf("\nIngrese el ID de la entrega numero 1\n\n ID = ");
      while(1)
      {
        scanf("%i", &id);
        aux = searchMap(save, &id);
        if(aux != NULL && repetido(same, id))
        {
          same[0] = id;
          conexion->id[0] = id;
          conexion->distancia[0] = calculo(way->main, aux->coor);
          suma += conexion->distancia[0];
          break;
        }

        printf("El ID que ingreso no se encuentra en el almacenamiento o es uno repetido, favor de ingresar otro.\n\n ID = ");
      }
    }

    restantes(save, same, aux->coor);
    printf("\nIngrese el ID de la entrega numero %i\n\n ID = ", i+1);

    while(1)
    {
      scanf("%i", &id);
      aux = searchMap(save, &id);
      if(aux != NULL && repetido(same, id))
      {
        same[i] = id;
        conexion->id[i] = id;
        anterior = searchMap(save, &conexion->id[i - 1]);
        conexion->distancia[i] = calculo(aux->coor, anterior->coor);
        suma += conexion->distancia[i];
        break;
      }

      printf("El ID que ingreso no se encuentra en el almacenamiento o es uno repetido, favor de ingresar otro.\n\n ID = ");
    }

    //Hace el ultimo calculo cuando entrega el ultimo, regrese a su ubicacion de inicio...
    if(i == 9)
    {
      way->last = calculo(way->main, aux->coor);
      suma += way->last;
    }
  }

  printf("\nSECUENCIA DE LA RUTA: ");
  for(i = 0; i < 10; i++)
  {
    printf("%i", conexion->id[i]);
    if(i <= 8) printf("-");
  }
  way->total = suma;
  printf("\nDISTANCIA TOTAL RECORRIDA: %i", way->total);

  char name[100];
  printf("\n\nPara finalizar favor de ingresar un nombre a la ruta creada manualmente, evita que el nombre sea repetido.\n\n NOMBRE = ");

  getchar();
  scanf("%99[^\n]s", name);

  way->nombre = strdup(name);
  conexion->nombre = strdup(name);
  push_back(connect, conexion);
  insertMap(ways, &way->total, way);

  printf("La ruta %s fue creada exitosamente y almacenada\n\n", way->nombre);
}

bool repetido(int a[10], int id)
{
  int i;
  for(i = 0; i < 10 && a[i] != 0; i++)
    if(a[i] == id)  return false;

  return true;
}

void restantes(Map* save, int same[10], int a[2])
{
  best* disponibles = malloc(48 * sizeof(best));
  int i, cont = 0;
  coordenadas* aux = firstMap(save);

  for(i = 0; i < 48 && aux != NULL; i++)
  {
    while(repetido(same, aux->id) == false)
    {
      cont++;
      aux = nextMap(save);
      if(aux == NULL) break;
    }
    if(aux == NULL) break;

    disponibles[i].id = aux->id;
    disponibles[i].distancia = calculo(a, aux->coor);

    aux = nextMap(save);
  }

  disponibles = realloc(disponibles, (48 - cont) * sizeof(best));
  qsort(disponibles, (48 - cont), sizeof(best), compare);
  printf("\nENTREGAS RESTANTES\n\n");

  for(i = 0; i < (48 - cont); i++)
    printf("%i %i\n", disponibles[i].id, disponibles[i].distancia);

  free(disponibles);
}

// OPCION 5 | LISTO (evitar usar el mismo nombre)
void generarRUTA(Map* save, Map* ways, List* connect)
{
  rutas* way = malloc(sizeof(rutas));
  nodos* conexion = malloc(sizeof(nodos));

  int same[10];
  int id, i, suma = 0;

  coordenadas* aux;
  coordenadas* anterior;

  printf("\nIngrese sus coordenadas actuales (x,y)\nX = ");
  scanf("%i", &way->main[0]);
  printf("Y = ");
  scanf("%i", &way->main[1]);

  for(i = 1; i < 10; i++)
  {
    if(i == 1)
      while(1)
      {
        srand(time(NULL));
        id = 1 + rand() % 49 - 1;
        aux = searchMap(save, &id);
        if(aux != NULL && repetido(same, id))
        {
          same[0] = id;
          conexion->id[0] = id;
          conexion->distancia[0] = calculo(way->main, aux->coor);
          suma += conexion->distancia[0];
          break;
        }
      }

    while(1)
    {
      srand(time(NULL));
      id = 1 + rand() % 49 - 1;
      aux = searchMap(save, &id);
      if(aux != NULL && repetido(same, id))
      {
        same[i] = id;
        conexion->id[i] = id;
        anterior = searchMap(save, &conexion->id[i - 1]);
        conexion->distancia[i] = calculo(aux->coor, anterior->coor);
        suma += conexion->distancia[i];
        break;
      }
    }

    if(i == 9)
    {
      way->last = calculo(way->main, aux->coor);
      suma += way->last;
    }
  }

  printf("\nSECUENCIA DE LA RUTA: ");
  for(i = 0; i < 10; i++)
  {
    printf("%i", conexion->id[i]);
    if(i <= 8) printf("-");
  }
  way->total = suma;
  printf("\nDISTANCIA TOTAL RECORRIDA: %i", way->total);

  char name[100];
  printf("\n\nPara finalizar favor de ingresar un nombre a la ruta creada manualmente, evita que el nombre sea repetido.\n\n NOMBRE = ");

  getchar();
  scanf("%99[^\n]s", name);

  way->nombre = strdup(name);
  conexion->nombre = strdup(name);
  push_back(connect, conexion);
  insertMap(ways, &way->total, way);

  printf("La ruta %s fue creada exitosamente y almacenada\n\n", way->nombre);
}

// OPCION 7 | Listo por el momento
void mostrarRUTAS(Map* ways, List* connect)
{
  rutas* aux = firstMap(ways);
  nodos* data;
  int i;

  if(aux != NULL)
  {
    printf("\nNOMBRE | RUTA | DISTANCIA TOTAL RECORRIDA\n\n");
    while(aux != NULL)
    {
      printf("%s  ", aux->nombre);
      data = first(connect);

      while(strcmp(data->nombre, aux->nombre) != 0)
        data = next(connect);

      for(i = 0; i < 10; i++)
      {
        printf("%i", data->id[i]);
        if(i <= 8) printf("-");
      }

      printf("  %i\n", aux->total);
      aux = nextMap(ways);
    }
  }
  else
    printf("No se creado/generado alguna ruta de entregas\n\n");
}

void menu(){
  printf(" ______________________________________\n");
  printf("|         Bienvenido al Menú de        |\n");
  printf("|           Rutas de entrega           |\n");
  printf("|______________________________________|\n");
  printf("| 1) Importar coordenadas de entrega   |\n");
  printf("| 2) Mostrar distancia entre entregas  |\n");
  printf("| 3) Mostrar 3 rutas mas cercanas      |\n");
  printf("| 4) Crear ruta                        |\n");
  printf("| 5) Generar ruta aleatoria            |\n");
  printf("| 6) Mejorar ruta                      |\n");
  printf("| 7) Mostrar rutas                     |\n");
  printf("| 8) Mostrar la ruta mas optima        |\n");
  printf("|______________________________________|\n");
  printf("¿Qué deseas hacer hoy?\n");
}
