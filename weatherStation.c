#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#define max 1000

// registro encargado de almacenar todos los datos meteorologicos
typedef struct regDiario{
    long int ddmmyyyy;
    int tmax;
    int tmin;
    int HUM;
    int PNM;
    int DV;
    int FF;
    int PP;
    bool borrado; 
}regDiario;

typedef struct fecha{
    int dd;
    int mm;
    int yyyy;
}TFecha;

typedef struct regDiarioC{
    TFecha ddmmyyyy;
    int tmax;
    int tmin;
    int HUM;
    int PNM;
    int DV;
    int FF;
    int PP;
    bool borrado; 
}regDiarioC;

//dato compuesto que almacena un arreglo de registros regDiario y la cantidad de elementos en el arreglo
typedef struct {
    regDiario reg[max];
    int cant;
} TData;

typedef struct {
    regDiarioC reg[max];
    int cant;
} TDatac;

//dato compuesto que indica el tipo de los nodos de la LSE usada en algunas de las funciones del programa
typedef struct nodo {
    regDiario info;
    struct nodo *next;
} TNodo;



//variables usada en el programa
regDiario datos;
TData regAr;
TNodo *listTempMax, *listVientoVel, *listPreciMax, *aux;
FILE* f;
FILE* g;
long fecha;
int opcion, i;

//perfiles de funciones principales
void alta(char name[50]);
void baja(int ddmmyyyy,char name[50]);
void modificar(int ddmmyyyy,char name[50]);
void mostrar(char name[50]);
int busqueda(TData a, long fecha, int cant);
int busquedac(TDatac a,TFecha fecha,int ini, int fin);
TNodo* temperaturaMax(char name[50]);
TNodo* velocidadViento(char name[50]);
TNodo* precipitacionMax(char name[50]);

//perfiles de las funciones auxiliares
TNodo* crearNodo();
void liberar(TNodo **a);
void intercambiar(regDiario *x, regDiario *y);
TData arregloDeArchivo(char name[50]);
TDatac arregloDeArchivoC(char name[50]);

//perfiles de las funciones llamadas por el switch
void opcion5(char name[50]);
void opcion6(char name[50]);
void opcion7(char name[50]);
void opcion8(char name[50]);
void opcion9(char name[50]);

int main() {
    char nameAr[50];

    printf("Ingrese el nombre del archivo: ");
    scanf(" %s", nameAr);
    f = fopen(nameAr,"ab");
    fclose(f);
    do{
        //menu que se mostrara cada vez que el usuario ejecute el programa
        printf("\n-----------------------------------\n");
        printf("Alta de un registro diario (1)\n");
        printf("Suprimir un registro diario (2)\n");
        printf("Modificar un registro (3)\n");
        printf("Mostrar registros (4)\n");
        printf("Buscar registro y mostrar todos sus campos (5)\n");
        printf("Listar dias de maxima temperatura (6)\n");
        printf("Listar dias de maxima precipitacion (7)\n");
        printf("Listar dias de mayor a menor velocidad de viento (8)\n");
        printf("Realizar copia de seguridad del año en curso (9)\n");
        printf("Salir (10)");
        printf("\n-----------------------------------\n");
        printf("Ingrese una opcion: ");
        fflush(stdout); fflush(stdin);
        scanf("%d", &opcion);
        fflush(stdin);

        switch (opcion){
            case 1:
                alta(nameAr);    
                break;

            case 2:
                printf("\nIngrese la fecha (ddmmyyyy) del registro que desea borrar: ");
                scanf(" %ld", &fecha);
                baja(fecha,nameAr);
                break;

            case 3:
                printf("\n Ingrese la fecha (ddmmyyyy) del registro que desea modificar: ");
                scanf(" %ld", &fecha);
                modificar(fecha,nameAr);
                break;

            case 4: 
                mostrar(nameAr);
                break;

            case 5:
                opcion5(nameAr);
                break;

            case 6:
                opcion6(nameAr);
                break;

            case 7:
                opcion7(nameAr);
                break;

            case 8:
                opcion8(nameAr);
                break;
            
            case 9:
                opcion9(nameAr);
                break;

            case 10:
                return 0;
                break;

            default: // la opcion no esta entre 1 y 10.
                printf("\nOpcion invalida.\n");
                break;
        }
    } while (1);
    return 0;
}

//definicion de las funciones principales
void alta(char name[50]){
    regDiario datos;
    f = fopen(name, "a");
    
    if (f != NULL){
        //se realiza la carga de datos
        printf("\nIngrese la fecha de hoy: ");
        scanf("%ld", &datos.ddmmyyyy);
        
        printf("Ingrese la temperatura maxima del dia: ");
        scanf("%d", &datos.tmax);
        
        printf("Ingrese la temperatura minima del dia: ");
        scanf("%d", &datos.tmin);
        
        do{
            printf("Ingrese el indice de humedad promedio del dia (entre 0 y 100): ");
            scanf("%d", &datos.HUM);
        } while (datos.HUM < 0 || datos.HUM > 100);
        
        do{
            printf("Ingrese la presion atmosferica promedio (en hectopascales) del dia (entre 900 y 3500): ");
            scanf("%d", &datos.PNM);
        } while (datos.PNM < 900 || datos.PNM > 3500);
        
        do{
            printf("Ingrese la direccion (en grados, de 0 a 360) del viento mas fuerte del dia: ");
            scanf("%d", &datos.DV);
        } while (datos.DV < 0 || datos.DV > 360);
        
        printf("Ingrese la maxima velocidad (km/h) de viento del dia: ");
        scanf("%d", &datos.FF);
        
        printf("Ingrese la precipitacion pluvial del dia (en mm): ");
        scanf("%d", &datos.PP);
        
        datos.borrado = false;

        //los datos almacenados son escritos al archivo
        fwrite(&datos, sizeof(datos), 1, f);
        fclose(f);
        
        printf("\nRegistro añadido con exito!\n");
    }else{
        printf("No se ha podido abrir el archivo!\n");
    }
}

void baja(int ddmmyyyy,char name[50]){
    regDiario datos;
    f = fopen(name, "r+b");
    
    if (f != NULL){
        //recorro el archuivo hasta encontrar un registro con la misma fecha que el parametro
        while(fread(&datos, sizeof(regDiario), 1, f) != 0){
            if(datos.ddmmyyyy == ddmmyyyy){
                if (datos.borrado == false){
                    //una vez encontrado, aplico el borrado logico, escribiendo el cambio en el archivo
                    datos.borrado = true;
                    int pos = (ftell(f) - (sizeof(regDiario)));
                    fseek(f,pos,SEEK_SET);
                    
                    fwrite(&datos, sizeof(datos), 1, f);
                    printf("Registro borrado con exito!\n");
                    fclose(f);
                }
            }
        }
    }else{
        printf("No se ha podido abrir el archivo!\n");
    } 
}

void modificar(int ddmmyyyy,char name[50]){
    regDiario nuevoReg;
    regDiario aux;
    f = fopen(name, "r+b");
    
    if(f != NULL){
        //recorro el archivo hasta encontrar un resgitro con la misma fecha que el parametro
        while(fread(&aux, sizeof(regDiario), 1, f) != 0){
            if(aux.ddmmyyyy == ddmmyyyy){
                //realizo la carga de los datos modificados en una variable auxiliar
                printf("Ingrese la fecha de hoy: \n");
                scanf("%ld", &nuevoReg.ddmmyyyy);
            
                printf("Ingrese la temperatura maxima del dia: \n");
                scanf("%d", &nuevoReg.tmax);
            
                printf("Ingrese la temperatura minima del dia: \n");
                scanf("%d", &nuevoReg.tmin);
            
                do{
                    printf("Ingrese el indice de humedad promedio del dia (entre 0 y 100): \n");
                    scanf("%d", &nuevoReg.HUM);
                } while (nuevoReg.HUM <= 0 || nuevoReg.HUM >= 100);
            
                do{
                    printf("Ingrese la presion atmosferica promedio (PNM) del dia (entre 900 y 3500): \n");
                    scanf("%d", &nuevoReg.PNM);
                } while (nuevoReg.PNM <= 900 || nuevoReg.PNM >= 3500);
            
                do{
                    printf("Ingrese la direccion (en grados, de 0 a 360) del viento mas fuerte del dia: \n");
                    scanf("%d", &nuevoReg.DV);
                } while (nuevoReg.DV <= 0 || nuevoReg.DV >= 360);
            
                printf("Ingrese la maxima velocidad de viento del dia: \n");
                scanf("%d", &nuevoReg.FF);
            
                printf("Ingrese la precipitacion pluvial del dia: \n");
                scanf("%d", &nuevoReg.PP);
                
                nuevoReg.borrado = false;

                //sobreescribo el registro con los datos antiguos con los nuevos datos
                int pos = (ftell(f) - (sizeof(regDiario)));
                printf("%d\n", pos);
                fseek(f, pos, SEEK_SET);
                
                fwrite(&nuevoReg, sizeof(nuevoReg), 1, f);
                printf("Registro modificado con exito!\n");
                fclose(f);
            }
        }
    }else{
        printf("No se ha podido abrir el archivo!\n");
    }
}

void mostrar(char name[50]){
    regDiario aux;
    f = fopen(name, "rb");
    
    if(f != NULL){
        //recorro el archivo mostrando los registros si su campo "borrado" es false
        while(fread(&aux, sizeof(regDiario), 1, f) != 0){
            if(aux.borrado == false){
                printf("\n-----------------------------------\n");
                printf("Fecha del registro: %ld\n", aux.ddmmyyyy);
                printf("Temperatura maxima de la fecha: %d grados celsius\n", aux.tmax);
                printf("Temperatura minima de la fecha: %d grados celsius\n", aux.tmin);
                printf("Humedad promedio de la fecha: %d%%\n", aux.HUM);
                printf("Presion atmosferica de la fecha: %d hectopascales\n", aux.PNM);
                printf("Direccion del viento con mayor intensidad (de 0 a 360): %d grados\n", aux.DV);
                printf("Maxima velocidad de viento de la fecha: %d km/h\n", aux.FF);
                printf("Precipitacion pluvial de la fecha: %d mm", aux.PP);
                if(aux.borrado){
                    printf("\n VERDADERO \n");
                }
                else{
                    printf("\n FALSO \n");
                }
                printf("\n-----------------------------------\n");
            }
        }
        fclose(f);	
    }else{
        printf("No se ha podido abrir el archivo!\n");
    }
}

TNodo* temperaturaMax(char name[50]){
    FILE* f;
    TNodo *aux, *aux2, *aux3;
    regDiario reg;
    int maxTemp;

    f = fopen(name, "rb");
    
    if(f == NULL){
        printf("El archivo esta vacio.");
    }else{
        //temperatura mas baja posible
        maxTemp = -274;
        
        //recorro el archivo y almaceno la temperatura maxima
        while(fread(&reg, sizeof(regDiario), 1, f) != 0){
            if (reg.tmax > maxTemp && reg.borrado == false){
                maxTemp = reg.tmax;
            }
        }
        rewind(f);
        aux2 = NULL;
        
        //vuelvo a recorrer el archivo mientras se crea una LSE con las temperaturas maximas
        while(fread(&reg, sizeof(regDiario), 1, f) != 0){
            if(reg.tmax == maxTemp && reg.borrado == false){
                aux = crearNodo();
                aux->info = reg;
                if(aux2 == NULL){
                    aux2 = aux;
                    aux3 = aux2;
                }else{
                    aux3->next = aux;
                    aux3 = aux;
                }
            }
        }
    }
    fclose(f);
    //retorno la cabeza de la lista con las temperaturas maximas
    return aux2;
}

TNodo* velocidadViento(char name[50]){
    TData res;
    int i, j;
    TNodo *aux, *aux2, *aux3;

    //en res, esta guardado todo el archivo pasado a un arreglo
    res = arregloDeArchivo(name);
    i = res.cant - 1;
    aux2 = NULL;

    //aplico bubbleSort sobre el arreglo para ordenarlo
    while(i > 0){
        j = 0;
        while(j < i){
            if(res.reg[j].FF < res.reg[j + 1].FF){
                intercambiar(&res.reg[j], &res.reg[j + 1]);
            }
            j++;
        }
        i--;
    }

    i = 0;

    //paso los datos del arreglo a una LSE para mostrar los dias con la maxima velocidad de viento
    while(i < res.cant){
        aux = crearNodo();
        aux->info = res.reg[i];
            if(aux2 == NULL){
                aux2 = aux;
                aux3 = aux2;
            }else{
                aux3->next = aux;
                aux3 = aux3->next;
            }
        i++;
    }
    //retorno la cabeza de la lista de las velocidades del viento
    return aux2;
}

TNodo* precipitacionMax(char name[50]){
    TData res;
    regDiario auxT;
    TNodo *aux,*aux2,*aux3;
    int i, j;

    //en res, esta guardado todo el archivo pasado a un arreglo
    res = arregloDeArchivo(name);
    i = 1;
    
    //utilizando insertionSort, ordeno el arreglo
    while (i < res.cant){
        auxT = res.reg[i];
        j = i - 1;
        while ((j >= 0) && (res.reg[j].PP > auxT.PP)){
            res.reg[j + 1] = res.reg[j];
            j--;
        }
        res.reg[j + 1] = auxT;
        i++;
    }

    i = res.cant - 1;

    //paso los datos del arreglo a una LSE para mostrar los dias con la maxima precipitacion
    aux2 = NULL;
    while (i >= 0){
        aux = crearNodo();
        aux->info = res.reg[i];
        if(aux2 == NULL){
            aux2 = aux;
            aux3 = aux2;
        }else{
            aux3->next = aux;
            aux3 = aux3->next;
        }
        i--;
    }
    //retorno la cabeza de la lista con las precipitaciones maximas
    return aux2;
}

//definicion de las funciones auxiliares
TNodo* crearNodo(){
    //funcion que se encarga de crear un nodo de tipo TNodo, que almacenara un registro regDiario
    TNodo* a;
    regDiario reg;

    a = (TNodo *) malloc(sizeof(TNodo));

    a->info = reg;
    a->next = NULL;

    return a;
}

void liberar(TNodo **a){
    TNodo* aux;

    while(*a != NULL){
        aux = *a;
        *a = (*a)->next;
        free(aux);
    }
}

void intercambiar(regDiario *x, regDiario *y){
    //intercambio dos registros entre si usando un auxiliar
    regDiario aux;
    aux = *x;
    *x = *y;
    *y = aux;
}

TDatac arregloDeArchivoC(char name[50]){
    TDatac aux;
    regDiario reg;
    int i;

    f = fopen(name,"rb");

    if(f == NULL){
        printf("El archivo esta vacio.\n");
    }else{
        i = 0;

        //leo los registros del archivo y los paso a un arreglo
        while (fread(&reg, sizeof(reg), 1, f) != 0 && reg.borrado == false){
            aux.reg[i].ddmmyyyy.dd = reg.ddmmyyyy/1000000;
            aux.reg[i].ddmmyyyy.mm = (reg.ddmmyyyy/10000) - (aux.reg[i].ddmmyyyy.dd*100);
            aux.reg[i].ddmmyyyy.yyyy = reg.ddmmyyyy - (aux.reg[i].ddmmyyyy.mm*10000);
            aux.reg[i].tmax = reg.tmax;
            aux.reg[i].tmin = reg.tmin;
            aux.reg[i].HUM = reg.HUM;
            aux.reg[i].PNM = reg.PNM;
            aux.reg[i].DV = reg.DV;
            aux.reg[i].FF = reg.FF;
            aux.reg[i].PP = reg.PP;
            i++;
        }
        aux.cant = i;
        fclose(f);
        return aux;
    }
}
TData arregloDeArchivo(char name[50]){
    TData aux;
    regDiario reg;
    int i;

    f = fopen(name,"rb");

    if(f == NULL){
        printf("El archivo esta vacio.\n");
    }else{
        i = 0;

        //leo los registros del archivo y los paso a un arreglo
        while (fread(&reg, sizeof(reg), 1, f) != 0 && reg.borrado == false){
            aux.reg[i] = reg;
            i++;
        }
        aux.cant = i;
        fclose(f);
        return aux;
    }
}

int busqueda(TData a, long fecha, int i){
    if(i < 0){
        return -1;
    }else{
        //si fecha es igual a la fecha del registro actual, devuelvo su indice
        if(a.reg[i].ddmmyyyy == fecha){
            return i;
        }else{
            //sino, llamo recursivamente decrementando a i para acotar la busqueda
            return busqueda(a, fecha, i - 1);
        }
    }
} 

int busquedac(TDatac a,TFecha fecha,int ini, int fin){
    if(ini > fin) return -1;

    int indiceMitad = floor((ini + fin) / 2);

    TFecha fechaMitad = a.reg[indiceMitad].ddmmyyyy;
    if((fecha.dd == fechaMitad.dd) && (fecha.mm == fechaMitad.mm)){
        return indiceMitad + 1;
    }

    if((fecha.mm < fechaMitad.mm) || ((fecha.dd < fechaMitad.dd) && (fecha.mm == fechaMitad.mm))){
        fin = indiceMitad - 1;
    }else{
        ini = indiceMitad + 1;
    }
    return busquedac(a,fecha,ini,fin);
} 

//definicion de las funciones llamdas por el switch
void opcion5(char name[50]){

    TDatac regArc;
    TFecha fechac;

    printf("\n Ingrese el dia del registro(1-31):");
    scanf(" %d", &fechac.dd);  
    fflush(stdin);
    printf("\n Ingrese el mes del registro(1-12):");
    scanf(" %d", &fechac.mm);  
<<<<<<< HEAD
    fflush(stdin);
    printf("\n Ingrese el año del registro(2022):");
    scanf(" %d", &fechac.yyyy);  
=======
>>>>>>> 47e229f8b8b2f7fef477a615fcf0bceaedda9b47
    fflush(stdin);
    printf("\n Ingrese el año del registro(2022):");
    scanf(" %d", &fechac.yyyy);  
    fflush(stdin);
    
    
    
    //en a guardo el archivo en forma de arreglo
    regArc = arregloDeArchivoC(name);
    //aplico la busqueda sobre el arrelgo con el archivo
    int p = busquedac(regArc, fechac, 0, regArc.cant-1);
    printf("%d", p);
    
    if(p != -1){
        g = fopen(name,"rb");
        fseek(g, (p * (sizeof(regDiario)) - (sizeof(regDiario))), SEEK_SET);
        
        //muestro los datos del registro que se corresponden a la fecha indicada
        fread(&datos, sizeof(regDiario), 1, g);
        printf("\n-----------------------------------\n");
        printf("Fecha del registro: %ld\n", datos.ddmmyyyy);
        printf("Temperatura maxima de la fecha: %d grados celsius\n", datos.tmax);
        printf("Temperatura minima de la fecha: %d grados celsius\n", datos.tmin);
        printf("Humedad promedio de la fecha: %d%%\n", datos.HUM);
        printf("Presion atmosferica de la fecha: %d hectopascales\n", datos.PNM);
        printf("Direccion del viento con mayor intensidad (de 0 a 360): %d grados\n", datos.DV);
        printf("Maxima velocidad de viento de la fecha: %d km/h\n", datos.FF);
        printf("Precipitacion pluvial de la fecha: %d mm", datos.PP);
        printf("\n-----------------------------------\n");
    }else{
        printf("El registro no existe.");
    }
    fclose(g);
    memset(regAr.reg, 0, max);
}

void opcion6(char name[50]){
    //en un puntero, guardo la lista con las temperaturas maximas
    listTempMax = temperaturaMax(name);  
    //le paso la direccion de memoria de la cabeza de la lista de temperaturas maximas a un puntero auxiliar
    aux = listTempMax;

    //mustro todas las fechas de las temperaturas maximas          
    while(aux != NULL){
        printf("\n-----------------------------------\n");
        printf("Fecha del registro con maxima temperatura: %ld grados celsius\n", aux->info.ddmmyyyy);
        printf("\n-----------------------------------\n");
        aux = aux->next;
    } 
    liberar(&listTempMax);
}

void opcion7(char name[50]){
    //en un puntero, guardo la lista con las maximas precipitaciones (ordenadas, de menor a mayor)
    listPreciMax = precipitacionMax(name);
    //en un puntero auxiliar, guardo la cabeza de la lista de maximas precipitaciones
    aux = listPreciMax;
    i = 1;

    //muestro las primeras 10 mayores precipitaciones            
    while((aux != NULL) && (i <= 10)){
        printf("\n-----------------------------------\n");
        printf("Fecha del registro: %ld\n", aux->info.ddmmyyyy);
        printf("Precipitacion pluvial de la fecha: %d mm", aux->info.PP);
        printf("\n-----------------------------------\n");
        aux = aux->next;
        i++;
    } 
    liberar(&listPreciMax);
}

void opcion8(char name[50]){
    //en un puntero, guardo la lista de la velocidad del viento (ordenada, de mayor a menor)
    listVientoVel = velocidadViento(name);
    //en un puntero auxiliar, guardo la cabeza de la lista de la velocidad del viento
    aux = listVientoVel;
    i = 1;

    //muestro las primeras 10 mayores velocidades de viento de la lista            
    while((aux != NULL) && (i <= 10)){
        printf("\n-----------------------------------\n");
        printf("Fecha del registro: %ld\n", aux->info.ddmmyyyy);
        printf("Direccion del viento con mayor intensidad (de 0 a 360): %d grados\n", aux->info.DV);
        printf("Maxima velocidad de viento de la fecha: %d km/h\n", aux->info.FF);
        printf("\n-----------------------------------\n");
        aux = aux->next;
        i++;
    } 
    liberar(&listVientoVel);
}

void opcion9(char name[50]){
    //abro ambos archivos (el actual, y el cual en donde voy a guardar la copia de seguridad)

    char name2[50];
    strcpy(name2, "copia_seguridad_");
    strcat(name2,name);
    f = fopen(name,"rb");
    g = fopen(name2,"wb+");

    //rewind(f);

    //a los registros que no estan borrados (borrado == false), los guarda en el nuevo archivo
    while(fread(&datos,sizeof(regDiario),1,f) != 0){
        if(datos.borrado == false){
            fwrite(&datos,sizeof(datos),1,g);
        }
    }
    fclose(f);
    fclose(g);
}
