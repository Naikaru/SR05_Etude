# Utilisation de la socket dans PIL

La classe `TcpClientManager` permet  de gérer la connection côté client.

Elle gère la connexion avec le robot ou le simulateur à l'aide de la méthode `void connectToRobot(QHostAddress addr, int port);`.
Elle effectue le handshake, et une fois celui ci établi, émet un signal `void receivedMessage(Message message);`.
Celui ci peut être utilisé pour traiter le message reçu.


Le PIL de chaque robot devrait contenir une instance de cette classe. Des messages peuvent être envoyés à l'aide de la méthode `send()`, qui prend en paramètre soit un objet de type `Message`, soit une chaine contenant le message.


En cas d'erreur (connexion échouée, connexion fermée par l'hôte distant, ...), le signal `error(QAbstractSocket::SocketError)` est émis par la socket interne à la classe. Celui ci est connecté à un slot  `handleErrors` de la classe, qui affiche simplement le message d'erreur dans `qDebug()`, mais ce comportement pourait être modifié (pour retenter une connexion après un délais par exemple ...).

**Exemple d'utilisation :**
```cpp
TcpClientManager client;
# Connection du signal de réception de message à une méthode pouvant les traiter
client.connect(&client, SIGNAL(receivedMessage(Message)), &this, SLOT(rmtMessage(Message)));
# Connection du client au RMT du robot.
client.connectToRobot(QHostAddress("127.0.0.1"), 4646);

# Création d'un message à envoyer en whatwhowhere
Message messageToSend = Message(PIL,ROB,LCH);
# Ajout de la mnémonique d'avancée
messageToSend.setValue("move","50");
# Envoie du message
client.send(messageToSend);
```