// TODO: il processo padre crea N processi figli (il numero è determinato dall’unico argomento di invocazione del programma) ;
// TODO: successivamente il processo padre invia ogni 2 secondi un numero casuale (compreso tra 10 e 100) ai figli in modo tale che un solo processo figlio, non determinato a priori, lo riceva ;
// TODO: il processo figlio che ha ricevuto numeri casuali per un totale superiore a 500 invia un segnale SIGUSR1 al padre e termina ;
// TODO: il processo padre deve essere insensibile al segnale SIGUSR1 fintanto che non ha inviato 5 numeri casuali ai figli ;
// TODO: il processo padre, ricevuto il segnale, termina tutti i figli, attende la loro uscita e termina anch’esso