# Guida alla sopravvivenza alla programmazione in Linux

[TOC]

------

## 0. Introduzione

Questa non vuole essere una guida alla programmazione in Linux. Si tratta di una guida alla sopravvivenza, con piccoli tips e un ripasso delle principali system call. 

Si consiglia comunque di navigare sulla pagina `man` da terminale (oppure sul sito [man7.org](https://www.kernel.org/doc/man-pages/)) specificando la voce alla quale si è interessati, in modo da comprendere appieno tutte le caratteristiche dei comandi.

### Compilazione di un programma in C

Dallo Shell, utilizzare `gcc` nel seguente modo:

```bash
gcc -o <nome del file eseguibile> <nome file sorgente.c>
```

L'opzione `-o` seguita da `<nome del file eseguibile>` comunica a `gcc` che deve creare un le eseguibile chiamato `<nome del file eseguibile>`. Ad esempio, se dalla shell si digita il comando `gcc -o hello hello.c`, il file sorgente `hello.c` verrà compilato e il file eseguibile sarà chiamato `hello`. Se non si scrive `-o filename`, il compilatore `gcc` crea un eseguibile chiamato `a.out`.

L'esecuzione di un programma da shell prevede la seguente sintassi: 

```bash
./<nome eseguibile> [lista argomenti invocazione]
```

In generale, la sintassi del comando `gcc` è: `gcc <opzioni> <filename>`
Oltre a `-o`, le opzioni più comuni sono:

- `-c` per creare il file oggetto anziché l'eseguibile: `gcc -c hello.c` genera il file oggetto `hello.o`
- `-Wall` genera tutti i messaggi di warning che `gcc` può fornire
- `-pedantic` mostra tutti gli errori e i warning richiesti dallo standard ANSI C
- `-O -O1 -O2 -O3` servono per definire il livello di ottimizzazione (dal più basso al più alto)
- `-O0` per non avere nessuna ottimizzazione
- `-g` per un successivo debugging

Per creare l'eseguibile a partire dal file oggetto: `gcc hello.o -o hello`
Quest'ultima operazione prende il nome di **linking**, che in generale consiste nella risoluzione dei simboli tra programmi e l'inclusione di eventuali librerie.

### Strace

Il programma `strace` consente di visualizzare le system call (con i relativi argomenti) invocate da un processo in esecuzione. È uno strumento di debug molto istruttivo che permette di ottenere, in tempo reale su schermo o in un file di testo, numerose informazioni sull'esecuzione di un programma.

Può essere invocato in due modalità principali:

1. `strace nomeprogramma [argomenti]` (esegue `nomeprogramma`)
2. `strace -p <pid processo in esecuzione>` (visualizza le SVC invocate dal processo con il PID specificato, già in esecuzione ).

Per memorizzare su le la traccia delle system call invocate, utilizzare l'opzione `-o` : ad es. 

```bash
strace -o traccialog.txt nomeprogramma
```


Per ottenere la traccia delle system call invocate da un processo e dai suoi processi figli, utilizzare l'opzione `-f` : ad es. 

```bash
strace -f nomeprogramma
```

Si tratta di una funzionalità molto importante, difficilmente ottenibile in un normale debugger che può soltanto seguire il processo padre oppure il processo glio (cfr. il comando ` set follow-fork-mode` di gdb).

Per le altre opzioni consultare il manuale di `strace`.

---

## 1. Chiamate di sistema per I/O

Quando un programma viene messo in esecuzione, possiede almeno tre file descriptor:

```c
0 Standard input
1 Standard output
2 Standard error
```

Vediamo le principali chiamate di sistema che fanno uso dei descrittori di file. La maggior parte di queste chiamate restituisce `-1` in caso di errore e assegna alla variabile `errno `il codice di errore. I codici di errore sono documentati nelle pagine di manuale delle singole chiamate di sistema e in quella di `errno`. La funzione `perror()` può essere utilizzata per visualizzare un messaggio di errore basato sul relativo codice.

### `write`

```c
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);
```

Dove: 

- `fd`: file descriptor su cui effettuare la scrittura
- `buf`: buffer che contiene il messaggio da andare a scrivere nel file descriptor indicato
- `count`: numero di byte da andare a scrivere nel file descriptor

Con la funzione `write()`, i primi `count` byte di `buf ` vengono scritti
nel file che è stato associato al file descriptor `fd`. La chiamata restituisce il numero dei byte scritti oppure `-1` se si è verificato un errore.

Per maggiori informazioni vedere [man 2 write](https://man7.org/linux/man-pages/man2/write.2.html).

### `read`

```c
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
```

Dove: 

- `fd`: file descriptor su cui effettuare la lettura
- `buf`: area di memoria in cui salvare i byte letti
- `count`: numero di byte da leggere

Con la funzione `read()`, vengono letti i primi `count` byte dal file descriptor `fd` e salvati nel `buf`. La chiamata alla funzione restituisce il numero di byte letti.

Per maggiori informazioni vedere [man 2 read](https://man7.org/linux/man-pages/man2/read.2.html).

### `open` e `close`

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

Dove:

- `pathname`: percorso del file da aprire
- `flags`: modo di accesso per il file. Si usano le voci:
  - `O_RDONLY` per aprire il file in sola lettura
  - `O_WRONLY` per aprire il file in sola scrittura
  - `O_RDWR` per aprire il file in scrittura e lettura
- `mode`: modalità di accesso per il file (indispensabile quando si usa come `flags` il valore `O_CREAT`) in formato ottale per user, group e others

Se la chiamata alla funzione ha successo, viene restituito il file descriptor del file specificato; altrimenti si ritorna `-1`.

Per maggiori informazioni vedere [man 2 open](https://man7.org/linux/man-pages/man2/open.2.html).

```c
#include <unistd.h>

int close(int fd);
```

Dove:

- `fd`: file descriptor del file da chiudere

La chiamata alla funzione chiude il file descriptor indicato come parametro e tutte le risorse ad esso associato sono liberate.

Per maggiori informazioni vedere [man 2 close](https://man7.org/linux/man-pages/man2/close.2.html).

### `sendfile`

```c
#include <sys/sendfile.h>

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
```

Dove:

- `out_fd`: file descriptor di destinazione
- `in_fd`: file descriptor di partenza
- `offset`: punto in cui iniziare la lettura del file `in_fd`
- `count`: numero di byte da copiare in `out_fd`

La funzione `sendfile()` copia i dati da un file descriptor a un altro rimanendo all'interno del kernel. Risulta essere più efficiente dell'utilizzo combinato di `read` e `write`, che invece trasferiscono dati tra spazio utente e kernel. La chiamata alla funzione `sendfile` restituisce il numero di byte scritti in `out_fd`.

Per maggiori informazioni vedere [man 2 sendfile](https://man7.org/linux/man-pages/man2/sendfile.2.html).

### `lseek`

```c
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

Dove:

- `fd`: file descriptor su cui operare
- `offset`: punto in cui posizionare il cursore all'interno del file descriptor
- `whence`: direttiva di comportamento. Può essere una tra:
  - `SEEK_SET`: il file offset diventa `offset` byte dall'inizio del file `fd`
  - `SEEK_CUR`: al file offset sono aggiunti `offset` byte dalla sua posizione corrente
  - `SEEK_END`: il file offset diventa la somma della dimensione del file più `offset` byte dalla fine del file

La chiamata alla funzione `lseek()` sposta il cursore nel file `fd` di `offset` byte.

Alcuni utilizzi:

1. Riportare il cursore a inizio file: `lseek(fd, 0, SEEK_SET)`
2. Posizionare il cursore alla fine del file: `lseek(fd,0,SEEK_END)`

Per maggiori informazioni vedere [man 2 lseek](https://man7.org/linux/man-pages/man2/lseek.2.html).

### `fstat`

```c
#include <sys/stat.h>
#include <unistd.h>

int fstat(int fd, struct stat *buf);
```

Dove:

- `fd`: file descriptor
- `buf`: destinazione delle informazioni

La chiamata alla funzione `fstat()` restituisce le informazioni del file `fd` e le salva all'interno di `buf`.

Per maggiori informazioni vedere [man 2 fstat](https://man7.org/linux/man-pages/man2/fstat.2.html).

### `dup` e `dup2`

```c
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

Dove:

- `oldfd`: file descriptor di cui eseguire la copia da inserire in tabella nel primo posto libero
- `newfd`: file descriptor specificato dall'utente

Le chiamate di sistema `dup()` e `dup2()` creano una copia del descrittore di file `oldfd`. Il nuovo descrittore è, nel caso della `dup()`, il descrittore non utilizzato con numero d'ordine più basso; nel caso della `dup2()`, è il `newfd` specificato dall'utente.

Per maggiori informazioni vedere [man 2 dup](https://man7.org/linux/man-pages/man2/dup.2.html) e [man 2 dup2](https://man7.org/linux/man-pages/man2/dup2.2.html).

### `link` e `unlink`

```c
#include <unistd.h>

int link(char *original_name, char *alias_name);
int unlink(char *alias_name);
```

Dove: 

- `original_name`: file descriptor
- `alias_name`: nome del link

La chiamata alla funzione `link()` crea un hard link al file `fd`. La chiamata alla funzione `unlink()`, invece, rimuove l'hard link.

Per maggiori informazioni vedere [man 2 link](https://man7.org/linux/man-pages/man2/link.2.html) e [man 2 dup2](https://man7.org/linux/man-pages/man2/unlink.2.html).

---

## 2. Controllo dei processi

### `fork`

```c
#include <unistd.h>
pid_t fork(void);
```

La funzione restituisce un valore intero che rappresenta il pid del processo figlio; in caso di errore, ritorna con `-1`. La chiamata alla funzione crea un processo figlio che esegue lo stesso codice, dispone di una copia dell'area dati e della tabella dei file aperti del padre, ma possiede un pid diverso. Tra processo padre e figlio non c'è nessuna condivisione di memoria!

Per il figlio, il pid è `0` mentre per il padre rappresenta il proprio identificativo.

Per fare eseguire operazioni diverse a padre e figlio si usa fare:

```c
if((pid = fork())==0)
{
	//PROCESSO FIGLIO
	...
} else
{
	// PROCESSO PADRE
	...
}
```

Per maggiori informazioni vedere [man 2 fork](https://man7.org/linux/man-pages/man2/fork.2.html).

### `getpid` e `getppid`

```c
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);
pid_t getppid(void);
```

La chiamata alla funzione `getpid()` restituisce il pid del processo attuale. La chiamata alla funzione `getppid()` restituisce il pid del processo padre.

Per maggiori informazioni vedere [man 2 getpid](https://man7.org/linux/man-pages/man2/getpid.2.html).

### `exit`

```c
void exit(int status);
```

Dove:

- `status`: valore d'uscita del processo figlio

La chiamata alla funzione `exit()` chiude tutti i file aperti, per il processo che termina. Il valore d'uscita viene passato al processo padre, se questo attende la terminazione di un suo figlio. La terminazione del processo figlio può avvenire involontariamente quando vengono eseguite azioni non consentite, segnali da tastiera (`^C` o `^\`) oppure da segnali inviati da terminale (attraverso il comando `kill -<segnale> <pid>`)

Per maggiori informazioni vedere [man 2 exit](https://man7.org/linux/man-pages/man2/exit.2.html).

### `wait ` e `waitpid`

```c
#include <sys/wait.h>
#include <sys/types.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```

Dove:

- `status`: valore d'uscita del processo figlio
- `pid`: pid del processo che si vuole attendere che termini (se `pid = -1` si attende la terminazione di un qualunque processo)
- `options`: opzioni (di default `0`)

La primitiva `wait()` attende la terminazione di un qualunque processo figlio. Se il processo figlio termina con una `exit()` il secondo byte meno significativo di status è pari all'argomento passato alla `exit()` e il byte meno significativo è zero.

La primitiva `waitpid()` sospende l'esecuzione del processo chiamante
finchè il processo figlio identificato da `pid` termina. Se un processo figlio è già terminato al momento dell'invocazione di `waitpid`, essa ritorna immediatamente. 

Le primitive ritornano il pid del processo figlio terminato.

Per maggiori informazioni vedere [man 2 wait](https://man7.org/linux/man-pages/man2/wait.2.html).

### `sleep`

```c
#include <unistd.h>
unsigned int sleep(unsigned int secs);
```

Dove:

- `secs`: numero di secondi di attesa

La funzione `sleep()` sospende il processo per un periodo di tempo pari a `secs` secondi.

Per maggiori informazioni vedere [man 3 sleep](https://man7.org/linux/man-pages/man3/sleep.3.html).

### `execve`

```c
int execve(char *file_name, *argv[], *envp[]);
```

Dove:

- `file_name`: percorso del file da eseguire
- `argv`: argomenti d'invocazione da eseguire sul file specificato da `file_name`
- `envp`: eventuali variabili d'ambiente per il nuovo ambiente

La primitiva `execve` non produce nessun nuovo processo, ma cambia ambiente d'esecuzione. Non è previsto il ritorno della chiamata a meno che non vi sia un errore (-1). Esistono diverse varianti della medesima funzione:

```c
#include <unistd.h>

int execl( const char *path, const char *arg, ...);
int execlp( const char *file, const char *arg, ...);
int execle( const char *path, const char *arg , ..., char * const envp[]);
int execv( const char *path, char *const argv[]);
int execvp( const char *file, char *const argv[]);
```

Per maggiori informazioni vedere [man 2 execve](https://man7.org/linux/man-pages/man2/execve.2.html).

---

## 3. Segnali

### `signal`

```
#include <signal.h>
void (*signal (int signumber, void (*handler)(int)))
```



### `kill`

```
#include <signal.h>
int kill (int process_id, int signumber )
```



### `alarm`

```
#include <unistd.h>
unsigned int alarm(int seconds)
```



### `pause`

```
#include <unistd.h>
unsigned int pause(void)
```



### `sleep` e `nanosleep`

```
#include <unistd.h>
unsigned int sleep(unsigned int n_seconds);
```



```
#include <time.h>
int nanosleep(const struct timespec *req, struct timespec *rem);
```



### GESTIONE AFFIDABILE DEI SEGNALI

### `sigaction`

```
#include <signal.h>
int sigaction(int signum, const struct sigaction *act,struct sigaction *oldact);
```



### `sigprogmask`

```
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```



---

## 4. Pipe

```
#include <unistd.h>
int pipe (int file_descriptors[2])
```



---

## 5. FIFO

---

## 6. Socket

### `socket`

```
#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol)
```



### `bind`

```
int bind(int sockd,struct sockaddr* my_addr_p,int addrlen)
```



### `listen`

```
int listen(int sockfd,int backlog)
```



### `accept`

```
int accept(int sockfd,struct sockaddr* addr_p,int* len_p)
```



### `connect`

```
int connect(int sockfd,struct sockaddr* addr_p,int len)
```



### `close`

```
close(sock);
```



### `select`

```
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```



### SERVER CONCORRENTI

