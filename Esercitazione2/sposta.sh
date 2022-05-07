#! /bin/sh

if (test $# -ne 2) then
  echo "Uso: $0 estensione direttorio"
  exit 1
fi

if (test ! -d $2) then
  echo "Il direttorio $2 non esiste, adesso lo creo"
  mkdir $2
fi

echo "Dimmi la parola che devo cercare: "
read parola

for i in *.$1
do
echo "Esamino il file $i"
if (grep $parola $i) then
  echo "Sposto $i in $2"
  mv $i $2
  fi
done
