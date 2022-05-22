#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void handler(int signo)
{
  static int beeps = 0; // static e' importante perchè viene creato una singola volta (alla prima chiamata) e poi viene mantenuto

  printf("BEEP\n");
  if (++beeps < 5) // se non avessi static non sarebbe mai possibile raggiungere i 5 BEEP
    alarm(1);
  else
  {
    printf("BOOM!\n"); // se non avessi lo static non raggiungerei mai il BOOM
    exit(0);
  }
}

int main()
{
  struct timespec ts; // struct timespec e' una struttura che contiene un tempo

  signal(SIGALRM, handler); // funzione che gestisce il segnale SIGALRM
  alarm(1);                 // funzione che imposta un timer che scatta dopo 1 secondo

  ts.tv_sec = 20; // setta il tempo di attesa in secondi del timer
  ts.tv_nsec = 0; // setta il tempo di attesa in nanosecondi del timer

  while (1)               // ciclo infinito
    nanosleep(&ts, NULL); // funzione che fa dormire il processo per un tempo

  exit(0);
}
