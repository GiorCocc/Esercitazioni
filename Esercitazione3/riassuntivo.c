#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define N 3

typedef struct persona{
  char nome[16];
  char eta[3];
}Persona;

int main(){
  Persona vettore[N];

  //TODO: metterli in un file separato e leggerne il contenuto per inserirli nella struttura
  strcpy (vettore[0].nome, "Giovanni");
  strcpy (vettore[1].nome, "Pietro");
  strcpy (vettore[2].nome, "Riccardo");
  strcpy (vettore[0].eta, "18");
  strcpy (vettore[1].eta, "32");
  strcpy (vettore[2].eta, "22");

}
