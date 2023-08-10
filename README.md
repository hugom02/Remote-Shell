# Remote-Shell

Il y a besoin de 2 machines : une serveur et une cliente.

Compiler : 
  gcc mainServer.c src/server.c -o mainServer
  gcc mainClient.c src/client.c -o mainClient

Exécuter : 
  ./mainServer -v -i=IP server (+ autres arguments)
  ./mainClient -v -i=IP server (+ autres arguments)

Arguments : 
  -v : mode verbeux
  -i=IP : Adresse IP du server
  -p : port spécifique pour le server
  -b : nombre de bytes max par communication
