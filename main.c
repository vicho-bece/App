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
  int last; //Distancia entre la ultima entrega y su ubicacion...
  int total;
}rutas;

//Estructura de la LISTA
typedef struct{
  char* nombre;
  int id[10]; //Arreglo para los IDs ingresados...
  int distancia[10]; //Distancia entre los IDs...
}nodos;

//Estructura auxiliar
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

//Opciones del menu y funciones apartes...
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
void mejorarRUTA(Map*, Map*, List*);
void mostrarRUTAS(Map*, List*);
void cambiar(rutas*, nodos*, best*, int, int);
void bestWAY(Map*, Map*, List*);
int cerca(Map*, int[], int[]);


int main(void) {
  Map* save = createMap(is_equal_int); //Almacenar los datos...
  Map* ways = createMap(is_equal_int); //Grafo IMPLICITO 
  List* connect = create_list(); //Lista que contempla con el grafo

  setSortFunction(save, lower_than_int); //Ordenar por ID
  setSortFunction(ways, lower_than_int); //Ordenar por Distancia Recorrida total
  menu();

  //El usuario ingresa el numero de la opcion para realizar alguna operacion...
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
      case 6: mejorarRUTA(save, ways, connect); break;
      case 7: mostrarRUTAS(ways, connect); break;
      case 8: bestWAY(save, ways, connect); break;
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
  //Se lee y se verifica el archivo...
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

  //Indices, contador para el ID y una variable auxiliar...
  int i, j, k, num, cont = 1;

  while(fscanf(archivo, "%9[^\n]s", linea) != EOF)
  {
    fgetc(archivo);
    coordenadas* lugar = malloc(sizeof(coordenadas));
    
    //Variables para almacenar la informacion del archivo tipo texto
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

  //Calculo la cantidad de digitos que ocupa el char...
  for(i = 0; numero[i] != '\0'; i++)
    potencia++;

  aux = potencia;

  if(potencia == 0)
  {
    //El caso de que el numero sea de un digito...
    digito = numero[0] - 48;
    return digito;
  }
  else
  {
    //El caso de que el numero sea mayor o igual a 2 digitos..
    for(int k = 0; k < i; k++)
    {
      decimas = 1;
      digito = numero[k] - 48;

      //Ajusto las decimas dependiendo la posicion del string...
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
  coordenadas* aux = firstMap(save);

  //Pregunta si hay entregas almacenadas...
  if(aux != NULL)
  {
    printf("\nIngrese el ID de una entrega\n ID = ");
    //El usuario ingresa y verifica el ID de una entrega del almacenamiento...
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
  
    //Guardo coordenadas...
    coor1[0] = aux->coor[0];
    coor1[1] = aux->coor[1];
  

    printf("\n Ahora ingrese el ID de otra entrega\n ID = ");
    //Ingresa el otro ID de entrega, verifica que esta en el almacenamiento y que no sea el mismo del anterior...
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
  
    //Guardo coordenadas...
    coor2[0] = aux->coor[0];
    coor2[1] = aux->coor[1];

    int distancia = calculo(coor1, coor2);
  
    printf("\nLa distancia entre la entrega %i y %i es %i[metros] aproximadamente\n", sameID, numID, distancia);
  }
  else
    printf("\n\nNo hay IDs de entregas registradas\n\n");
}

//Funcion para calcular las distancia entre 2 puntos...
int calculo(int a[2], int b[2])
{
  int x, y, suma;
  int divisor = 1;

  //Calculo la diferencia, le aplico "al cuadrado" y se suma...
  x = b[0] - a[0]; x *= x;
  y = b[1] - a[1]; y *= y;
  suma = x + y;

  //Busco la raiz de la suma...
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
    //Estructura aux..
    best datos[48];
    int i;
    
    //Cada entrega se calcula la distancia con respecto la ubicacion del usuario...
    for(i = 0; i < 48; i++)
    {
      datos[i].id = aux->id;
      datos[i].distancia = calculo(user, aux->coor);
      aux = nextMap(save); 
    }

    //Se ordena por QuickSort y muestra las 3 primeras entregas mas cercanas...
    qsort(datos, 48, sizeof(best), compare);
    printf("\nLas 3 Mejores Rutas desde tus coordenadas(%i , %i) son:\n", user[0], user[1]);

    for(i = 0; i < 3; i++)
      printf("ID = %i, DISTANCIA = %i\n", datos[i].id, datos[i].distancia);
  }
  else
    printf("\n No hay coordenadas de entrega registradas...\n");
}

//Funcion necesaria para el QuickSort...
int compare(const void* a, const void* b)
{
  best *ptrA = (best *)a;
  best *ptrB = (best *)b;

  if(ptrA->distancia > ptrB->distancia)
    return 1;

  return 0;
}


// OPCION 4 | LISTO 
void crearRUTA(Map* save, Map* ways, List* connect)
{
  rutas* way = malloc(sizeof(rutas));
  nodos* conexion = malloc(sizeof(nodos));

  //Creo un arreglo para evitar que los numeros de ID ingresados sean repetidos...
  int* same = malloc(10 * sizeof(int));
  int id, i, suma = 0;

  coordenadas* aux = firstMap(save);
  coordenadas* anterior;

  if(aux != NULL)
  {
    printf("\nIngrese sus coordenadas actuales (x,y)\nX = ");
    scanf("%i", &way->main[0]);
    printf("Y = ");
    scanf("%i", &way->main[1]);

    printf("\nAhora ingresara los 10 ID de entrega\n");

    for(i = 1; i < 10; i++)
    {
      //Calculo la distancia desde el usuario y el primer ID entrega, como caso particular...
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
     
      //Se calculo los IDs de entregas...
      while(1)
      {
        scanf("%i", &id);
        aux = searchMap(save, &id);
        if(aux != NULL && repetido(same, id))
        {
          same[i] = id;
          conexion->id[i] = id;
          anterior = searchMap(save, &conexion->id[i - 1]);
          conexion->distancia[i] = calculo(anterior->coor, aux->coor);
          suma += conexion->distancia[i];
          break;
        }

        printf("El ID que ingreso no se encuentra en el almacenamiento o es uno repetido, favor de ingresar otro.\n\n ID = ");
      }

    //Hace el ultimo calculo cuando entrega el ultimo, regrese a su ubicacion de inicio...
      if(i == 9)
      {
        way->last = calculo(aux->coor, way->main);
        suma += way->last;
      }
    }

    /*
    Se informa al usuario la secuencia de la ruta y la distancia total recorrida...
    */
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

    //Se pide ingresar un nombre particular a la ruta creada...
    getchar();
    scanf("%99[^\n]s", name);
    rutas* find = firstMap(ways);
    
    while(find != NULL)
    {
      if(strcmp(find->nombre, name) == 0)
      {
        printf("\n\nEste nombre ya esta ocupado en una ruta alamacenada. Favor de ingresar otro nombre.\n\n NOMBRE = ");
        getchar();
        scanf("%99[^\n]s", name);
        find = firstMap(ways);
      }
      else
        find = nextMap(ways);
    }

    //Se guarda la ultima informacion y se almacena la Lista y Mapa...

    way->nombre = strdup(name);
    conexion->nombre = strdup(name);
    push_back(connect, conexion);
    insertMap(ways, &way->total, way);

    printf("La ruta %s fue creada exitosamente y almacenada\n\n", way->nombre);
  }
  else
    printf("\n\nNo hay IDs de entregas para crear una ruta personalizada\n\n");
}

//Verifica si el numero que ingreso el usuario es repetido...
bool repetido(int a[10], int id)
{
  int i;
  for(i = 0; i < 10 && a[i] != 0; i++)
    if(a[i] == id)  return false;

  return true;
}

//Muestra las entregas restantes...
void restantes(Map* save, int same[10], int a[2])
{
  best* disponibles = malloc(48 * sizeof(best));
  int i, cont = 0;
  coordenadas* aux = firstMap(save);

  /*
  Durante el ciclo, se imprime la informacion de los IDs y su distancia con respecto a la anterior. Evitando de utilizar/imprimir una entrega utilizada...
  */
  for(i = 0; i < 48 && aux != NULL; i++)
  {
    while(repetido(same, aux->id) == false)
    {
      cont++;
      aux = nextMap(save);
      if(aux == NULL) break; //Se hace referencia el ultimo dato...
    }
    if(aux == NULL) break; //Break auxiliar...

    disponibles[i].id = aux->id;
    disponibles[i].distancia = calculo(a, aux->coor);

    aux = nextMap(save);
  }

  //Se ajusta con respecto a las entregas utilizadas y se ordena el arreglo de estructuras y se imprimen...
  disponibles = realloc(disponibles, (48 - cont) * sizeof(best));
  qsort(disponibles, (48 - cont), sizeof(best), compare);
  printf("\nENTREGAS RESTANTES\n\n");

  for(i = 0; i < (48 - cont); i++)
    printf("%i %i\n", disponibles[i].id, disponibles[i].distancia);

  free(disponibles);
}

// OPCION 5 | LISTO 
//El algoritma es muy similar a la funcion 4...
void generarRUTA(Map* save, Map* ways, List* connect)
{
  rutas* way = malloc(sizeof(rutas));
  nodos* conexion = malloc(sizeof(nodos));

  int same[10];
  int id, i, suma = 0;

  coordenadas* aux = firstMap(save);
  coordenadas* anterior;

  if(aux != NULL)
  {
    printf("\nIngrese sus coordenadas actuales (x,y)\nX = ");
    scanf("%i", &way->main[0]);
    printf("Y = ");
    scanf("%i", &way->main[1]);
    printf("\nCalculando...\n");
    for(i = 1; i < 10; i++)
    {
      if(i == 1)
        while(1)
        {
          //Se escoje el numero aleatorio dentro del rango de la entregas almacenadas en total...
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
        //Se escoje otro numero aleatorio y distinto...
        srand(time(NULL));
        id = 1 + rand() % 49 - 1;
        aux = searchMap(save, &id);
        if(aux != NULL && repetido(same, id))
        {
          same[i] = id;
          conexion->id[i] = id;
          anterior = searchMap(save, &conexion->id[i - 1]);
          conexion->distancia[i] = calculo(anterior->coor, aux->coor);
          suma += conexion->distancia[i];
          break;
        }
      }

      if(i == 9)
      {
        way->last = calculo(aux->coor, way->main);
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
  
    rutas* find = firstMap(ways);
    
    while(find != NULL)
    {
      if(strcmp(find->nombre, name) == 0)
      {
        printf("\n\nEste nombre ya esta ocupado en una ruta alamacenada. Favor de ingresar otro nombre.\n\n NOMBRE = ");
        getchar();
        scanf("%99[^\n]s", name);
        find = firstMap(ways);
      }
      else
        find = nextMap(ways);
    }

    way->nombre = strdup(name);
    conexion->nombre = strdup(name);
    push_back(connect, conexion);
    insertMap(ways, &way->total, way);

    printf("La ruta %s fue creada exitosamente y almacenada\n\n", way->nombre);
  }
  else
    printf("\n\nNo hay IDs de entrega para crear una ruta aleatoria\n\n");
}

//OPCION 6 | LISTO
void mejorarRUTA(Map* save, Map* ways, List* connect){
  rutas* aux = firstMap(ways);
  char nombre_ruta[100];
  
  //Pregunta si hay rutas...
  if(aux != NULL)
  {
    printf("\nIngrese Nombre de la ruta guardada anteriomente\n");
    getchar();
    scanf("%99[^\n]s", nombre_ruta);
    
    //Se verifica el nombre de la Ruta que desea mejorar...
    while(1)
    {
      if(aux == NULL)
      {
        printf("\nEl Nombre de la ruta que ingreso no se encontro, favor de ingresar uno que este almacenado\n");
        getchar();
        scanf("%99[^\n]s", nombre_ruta);
        aux = firstMap(ways);
      }

      if(strcmp(aux->nombre,nombre_ruta) == 0)
        break;

      aux = nextMap(ways);
    }

    //Se rescata la lista asociado con el Mapa
    nodos* datos = first(connect);
    while(strcmp(datos->nombre, aux->nombre) != 0)
      datos = next(connect);

    printf("\n¿Prefiere buscar la mejor ruta de forma manual o automática?\n");
    printf("\n0 = Manual\n");
    printf("1 = automático\n\n OPCION = ");
    
    //El usuario ingresa un de esas 2 opciones y se verifica...
    int num;
    scanf("%i", &num);
    
    while(num != 0 && num != 1)
    {
      printf("\nLa Opcion que escogio no se encuentra en el listado mencionado. Favor de ingresar otro\n\n OPCION = ");
      scanf("%i", &num);
    }
    
    int num1, num2, indice, indice2, i;

    //Arreglo similar a la secuencia de la ruta...
    best* respaldo = malloc(10 * sizeof(best));

    printf("Estas son la secuencia de entregas de la ruta:\n");

    for(i = 0; i < 10; i++)
    {
      respaldo[i].id = datos->id[i];
      printf("%i-", datos->id[i]);
    }

    if(num == 0)
    {
      //Ingresa el primer ID y verifica que este dentro de la secuencia...
      printf("\n\nIndique 2 IDs de entrega a intercambiar\n");
      printf("Primer ID = ");
      scanf("%i", &num1);

      for(i = 0; i < 10; i++)
      {
        //Encuentra la coincidencia...
        if(num1 == datos->id[i])
        {
          indice = i;
          break;
        }

        //Si no lo encuentra, se pide ingresar otro ID y se reinicia el ciclo...
        if(i == 9)
        {
          printf("\nEl ID que ingreso no se encuentra dentro de la secuencia. Favor de ingresar un numero de la secuencia\n\nPrimer ID = ");
          scanf("%i", &num1);
          i = -1;
        }
      }

      //Ingresa el segundo ID...
      printf("\nSecundo ID = ");
      scanf("%i", &num2);
      for(i = 0; i < 10; i++)
      {
        //Se asegura que no sea el mismo del anterior...
        while(num2 == num1)
        {
          //Se obliga a ingresar uno diferente y dentro de la secuencia...
          printf("El ID que ingreso es igual al Primero. Favor de ingresar uno diferente.\n\nSecundo ID = ");
          scanf("%i", &num2);
        }

        //Encuentra coincidencia...
        if(num2 == datos->id[i])
        {
          indice2 = i;
          break;
        }

        if(i == 9)
        {
          printf("\nEl ID que ingreso no se encuentra dentro de la secuencia. Favor de ingresar un numero de la secuencia\n\nSecundo ID = ");
          scanf("%i", &num2);
          i = -1;
        }
      }
    }
    else
    {
      //Se elige de manera aleatoria el primer ID...
      srand(time(NULL));
      num1 = 1 + rand() % 49 - 1;

      for(i = 0; i < 10; i++)
      {
        if(datos->id[i] == num1)
        {
          indice = i;
          break;
        }

        if(i == 9)
        {
          srand(time(NULL));
          num1 = 1 + rand() % 49 - 1;
          i = -1;
        }
      }

      //Se elige el segundo ID...
      srand(time(NULL));
      num2 = 1 + rand() % 49 - 1;

      for(i = 0; i < 10; i++)
      {
        while(num1 == num2)
        {
          srand(time(NULL));
          num2 = 1 + rand() % 49 - 1;
        }
        
        if(datos->id[i] == num2)
        {
          indice2 = i;
          break;
        }

        if(i == 9)
        {
          srand(time(NULL));
          num2 = 1 + rand() % 49 - 1;
          i = -1;
        }
      }
    }

    //Se el intercambio en el arreglo de respaldo...
    respaldo[indice].id = num2;
    respaldo[indice2].id = num1;
    coordenadas* data;
    coordenadas* ant;

    printf("\n\nSecuencia Propuesta:\n\n");
    for(i = 0; i < 10; i++)
      printf("%i-", respaldo[i].id);


    int suma = 0, ultimo;

    //AQUI SE HACE LOS CALCULOS...

    for(i = 1; i < 10; i++)
    {
      //El calculo entre el usuario y el primer ID...
      if(i == 1)
      {
        data = searchMap(save, &respaldo[0].id);
        respaldo[0].distancia = calculo(aux->main, data->coor);
        suma += respaldo[0].distancia;
      }

      //Se calculo entre IDs de entrega...
      data = searchMap(save, &respaldo[i].id);
      ant = searchMap(save, &respaldo[i - 1].id);
      
      respaldo[i].distancia = calculo(ant->coor, data->coor);
      suma += respaldo[i].distancia;

      //Ultima entrega y regreso...
      if(i == 9)
      {
        ultimo = calculo(data->coor, aux->main);
        suma += ultimo;
      }
    }

    //Pregunta si la suma de la secuencia propuesta es menor que la anterior....
    if(aux->total > suma)
    {
      //Se hace las modificaciones...
      printf("\n\nLos IDs de entregas propuestas, mejora en el sentido que reduce la distancia recorrida total. Entonces se realiza el intercambio. Resultado:\n\n Anterior = %i\n\n Propuesto = %i\n\n", aux->total, suma);
      cambiar(aux, datos, respaldo, ultimo, suma);
    }
    else
    {
      //No se hace las modificaciones...
      free(respaldo);
      printf("\n\nLos IDs de entregas propuestas, automatico/manual, empeora o es igual el resultado con respecto a la distancia recorrido total, el cual no se hace la modificacion a la ruta. Resultado:\n\n Anterior = %i\n\n Propuesto = %i\n\n", aux->total, suma);
    }
  }
  else
    printf("\nNo hay rutas almacenadas para realizar una mejora\n");
}

//Modifica los datos del mapa y se cambia por completo la secuencia(por seguridad)
void cambiar(rutas* aux, nodos* datos, best* respaldo, int ultimo, int suma)
{
  aux->last = ultimo;
  aux->total = suma;
  
  int i;
  for(i = 0; i < 10; i++)
  {
    datos->id[i] = respaldo[i].id;
    datos->distancia[i] = respaldo[i].distancia;
  }
}

// OPCION 7 | LISTO
void mostrarRUTAS(Map* ways, List* connect)
{
  rutas* aux = firstMap(ways);
  nodos* data;
  int i;

  //Pregunta si hay rutas...
  if(aux != NULL)
  {
    //Va por orden del mapa
    printf("\nNOMBRE | RUTA | DISTANCIA TOTAL RECORRIDA\n\n");
    while(aux != NULL)
    {
      printf("%s  ", aux->nombre);

      //Busca la lista asociado con el nombre...
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

// OPCION 8 | LISTO 
void bestWAY(Map* save, Map* ways, List* connect)
{
  rutas* way = malloc(sizeof(rutas));
  nodos* conexion = malloc(sizeof(nodos));
  coordenadas* aux = firstMap(save);

  //Respaldo
  coordenadas* indice;
  
  //Arreglo para evitar IDs repetidos que interfieren con el resultado...
  int same[10];
  int id, i, suma = 0;


  printf("\nIngrese sus coordenadas actuales (x,y)\nX = ");
  scanf("%i", &way->main[0]);
  printf("Y = ");
  scanf("%i", &way->main[1]);

  //Pregunta si hay coordenadas...
  if(aux != NULL)
  {
    for(i = 1; i < 10; i++)
    {
      if(i == 1)
      {
        //Busco el ID mas cercano al usuario...
        id = cerca(save, same, way->main);
        aux = searchMap(save, &id);
        conexion->distancia[0] = calculo(way->main, aux->coor);
        conexion->id[0] = aux->id;

        //Guardo el ID usado...
        same[0] = aux->id;
      }

      
      //Obtengo los datos del ultimo ID...
      indice = searchMap(save, &conexion->id[i - 1]);
      //Busco el ID mas cercano con respecto al ultimo ID ingresado...
      id = cerca(save, same, indice->coor);
      aux = searchMap(save, &id); 

      conexion->distancia[i] = calculo(indice->coor, aux->coor);
      conexion->id[i] = aux->id;
      suma += conexion->distancia[i];
      same[i] = aux->id;

      //Ultima entregra y regreso...
      if(i == 9)
      {
        way->last = calculo(aux->coor, way->main);
        suma += way->last;
      }
    }

    way->total = suma;
    char name[] = ("RUTA_OPTIMA");
    
    way->nombre = strdup(name);
    conexion->nombre = strdup(name);
    push_back(connect, conexion);
    insertMap(ways, &way->total, way);

    printf("\n\nLa Ruta %s se inserto en el almacen de Rutas de forma Exitosa\n\n", name);

  }
  else
    printf("\n\nNo hay IDs de entrega almacenados para generar la Mejor Ruta\n\n");
}

//Funcion similar al "mejores3"...
int cerca(Map* save, int same[10], int a[2])
{
  coordenadas* aux = firstMap(save);
  best* almacenado = malloc(48 * sizeof(best));
  int i, cont = 0;

  for(i = 0; i < 48 && aux != NULL; i++)
  {
    while(repetido(same, aux->id) == false)
    {
      cont++;
      aux = nextMap(save);
      if(aux == NULL) break;
    }
    if(aux == NULL) break;

    almacenado[i].id = aux->id;
    almacenado[i].distancia = calculo(a, aux->coor);

    aux = nextMap(save);
  }

  almacenado = realloc(almacenado, (48 - cont) * sizeof(best));
  qsort(almacenado, 48 - cont, sizeof(best),  compare);

  //Se retorne el primer dato a partir del QuickSort...
  return almacenado[0].id;
}

//Menú/Interfaz...
void menu(){ 
  printf(" ______________________________________\n");
  printf("|         Bienvenido al Menú de        |\n");
  printf("|           Rutas de entrega           |\n");
  printf("|______________________________________|\n");
  printf("| 1) Importar coordenadas de entrega   |\n");   
  printf("| 2) Mostrar distancia entre entregas  |\n");   
  printf("| 3) Mostrar 3 Entregas mas cercanas   |\n"); 
  printf("| 4) Crear Ruta                        |\n"); 
  printf("| 5) Generar Ruta Aleatoria            |\n");
  printf("| 6) Mejorar Ruta                      |\n");
  printf("| 7) Mostrar Rutas                     |\n");
  printf("| 8) Encontrar Ruta Optima             |\n");
  printf("|______________________________________|\n");
  printf("¿Qué deseas hacer hoy?\n");
}