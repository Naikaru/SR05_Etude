# Fonctionnement du handshake :

## phase I. : réception des paramètres :
Envoi par RMT, dans cet ordre, de ces valeurs, séparées par des espaces :

- APG_rmt_snd_delim "^"
- APG_rmt_snd_equal "~"
- APG_rmt_snd_mnemoapp "app"
- APG_rmt_snd_mnemobeglch "beglch"
- APG_rmt_snd_mnemobegair "begair"
- APG_rmt_snd_mnemoendlch "endlch"
- APG_rmt_snd_mnemoendair "endair"
- APG_rmt_snd_mnemodiscon "rmtend"

## phase II. :
Envoi par le client de 
`$::APG_rmt_snd_delim$::APG_rmt_snd_mnemoapp$::APG_rmt_snd_equal$::APP`
("^app~PIL")

Envoi par RMT du filtre : une liste d'application que RMT peut contacter : devrait contenir ROB

## phase III. :
Envoi par le client de la liste des applications auquelles le RMT s'abonne, en local (LCH) et dans le voisinage (AIR)
chez nous : "^begair~PIL"
