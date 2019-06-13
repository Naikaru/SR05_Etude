#!/bin/bash

#exemple execution script:
# ./testN.sh nBNode <chemin-vers-Net> <chemin-vers-Bas>
# ne pas oublier les droits : chmod 755 testN.sh

#si il y a au moins deux paramètres (nb noeuds,NET,BAS )
if [ $# -ge 3 ]
then
    #permet de générer N bas et net
    #le net formant un graphe complet
    BAS=$1
    NET=$2
    N=$3

    `find /tmp/ -maxdepth 1 -name 'in*[0-9]' -delete`
    `find /tmp/ -maxdepth 1 -name 'out*[0-9]' -delete`

    let "n = N - 1"
    #variable qui contiendra chemin de toutes les fifos entrante des nets
    netInFifos=""
    #crée tous les groupements BAS et NET avec co juste entre BAS -> NET
    for i in `seq 0 $n`;
    do
      mkfifo /tmp/inBas$i /tmp/outBas$i
      mkfifo /tmp/inNet$i /tmp/outNet$i
      let "X = 5 * i + 1"
      ./$BAS --ident=$i --nbNode=$N --x=$X --y=1 < /tmp/inBas$i > /tmp/outBas$i &
      ./$NET --ident=$i --nbNode=$N < /tmp/inNet$i > /tmp/outNet$i &

      #co BASi -> NETi
      cat /tmp/outBas$i > /tmp/inNet$i &
      netInFifos+=/tmp/inNet$i
      netInFifos+=' '
    done

  # création d'un graphe complet entre tous les NETs
  # on boucle sur tous les net
    for i in `seq 0 $n`;
    do
      #pour chaque net on connecte sa sortie à son bas : /tmp/inBas$i
      #et aux entrées de tous les nets sauf lui même : ${netInFifos//$netIn/}
      netIn=/tmp/inNet$i
      #co NETi -> BASi et NET de 0 à n sauf i
      cat /tmp/outNet$i | tee /tmp/inBas$i ${netInFifos//$netIn/} &
    done
fi
