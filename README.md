###############
# Compilation #
###############
make

#############
# Execution #
#############
make exec v=<version> n=<num_thread> r=<nb_rows> c=<nb_cols> t=<tmax> s=<size> p=<bool>
A noter que les options ne sont pas à toutes utiliser pour chaque version. Si une option n'est à utiliser que pour certaines versions, les versions utiles sont précisées entre parenthèses.
Paramètres (optionnels):
- n : le nombre de threads (omp / pthread) ;
- r : le nombre de blocs lignes par colonne (mpi) ;
- c : le nombre de blocs colonnes par ligne (mpi) ;
- t : le nombre d'itérations ;
- s : la taille du plateau ;
- p : un entier. Si différent de 0, imprime sur le terminal ;
- v : le nom de la version à utiliser, parmi :
 seq, omp, omp-bloc, pthread, mpi-synchrone, mpi-asynchrone, mpi-persistant (par défaut mpi-asynchrone)

 Vous pouvez aussi lancer directement une version en tapant son nom à la place de "exec"
 Exemple:
 make omp n=8 s=1024 t=100 p=1
 Cette commande lancera le programme avec la version OpenMP avec 8 threads, sur un plateau de taille 1024, sur 100 itérations, et imprimera le tableau final.
 Remarque: pour une taille supérieure à 80, l'utilisateur recevra un Warning auparavant signifiant que le tableau pourrait ne pas rentrer sur le terminal. Vous avez alors 3 secondes pour arrêter l'affichage par un Ctrl+C

#########
# Stats #
#########
make stat
Pour lancer le script de tests sur un ordinateur personnel. 
Cependant, pour utiliser les fonctions de MPI sur PlaFRIM, il faut utiliser le fichier "batch" :
qsub batch

###########
# Courbes #
###########
make plot
Cette commande trace les courbes des fichiers .data. 
