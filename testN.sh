#!/bin/bash

#exemple execution script:
# ./testN.sh nBNode <chemin-vers-Net> <chemin-vers-Bas>
# ne pas oublier les droits : chmod 755 testN.sh

#si il y a au moins deux paramètres (nb noeuds,NET,BAS )
if [ $# -ge 3 ]
then
    #permet de générer N bas et net
    #le net formant un graphe complet
    N=$1
    NET=$2
    BAS=$3

    `find /tmp/ -maxdepth 1 -name 'in*[0-9]' -delete`
    `find /tmp/ -maxdepth 1 -name 'out*[0-9]' -delete`

    let "n = N - 1"
    netInFifos=""
    #crée tous les groupements BAS et NET avec co juste entre BAS et NET
    for i in `seq 0 $n`;
    do
      mkfifo /tmp/inBas$i /tmp/outBas$i
      mkfifo /tmp/inNet$i /tmp/outNet$i

      ./$BAS --ident=$i < /tmp/inBas$i > /tmp/outBas$i &
      ./$NET --ident=$i --nbNode=$N < /tmp/inNet$i > /tmp/outNet$i &

      cat /tmp/outBas$i > /tmp/inNet$i &
      netInFifos+=/tmp/inNet$i
      netInFifos+=' '
    done

  # création d'un graphe complet entre tous les NETs
  # on boucle sur tous les net
    for i in `seq 0 $n`;
    do
      #pour chaque net on connecte sa sortie à son bas
      #et aux entrées de tous les nets sauf lui même
      netIn=/tmp/inNet$i
      cat /tmp/outNet$i | tee /tmp/inBas$i ${netInFifos//$netIn/} &
    done
fi
