###############
# Compilation #
###############
make

#############
# Execution #
#############
make exec v=<version> r=<nb_rows> c=<nb_cols> t=<tmax> s=<size> p=<bool>
Paramètres (optionnels):
- r : le nombre de blocs lignes par colonne ;
- c : le nombre de blocs colonnes par ligne ;
- t : le nombre d'itérations ;
- s : la taille du plateau ;
- p : un entier. Si différent de 0, imprime sur le terminal ;
- r : le nom de la version à utiliser, parmi :
 seq, omp, omp-bloc, pthread, mpi (par défaut mpi)

 Vous pouvez aussi lancer directement une version en tapant son nom à la place de "exec"
 Exemple:
 make omp s=1024 t=100 p=1
 Cette commande lancera le programme avec la version OpenMP, sur un plateau de taille 1024, sur 100 itérations, et imprimera le tableau final.
 Remarque: pour une taille supérieure à 80, l'utilisateur recevra un Warning auparavant signifiant que le tableau pourrait ne pas rentrer sur le terminal. Vous avez alors 3 secondes pour arrêter l'affichage par un Ctrl+C


##############
# Validation #
##############
make test N=n e=eps
Paramètres (optionnels):
- N correspond aux tailles de matrices utilisées pour les tests ;
- e est la précision au delà de laquelle deux valeurs sont considérées comme fausses ;
- p : un entier. Si différent de 0, imprime les matrices calculées.


#########
# Stats #
#########
make stat

###########
# Courbes #
###########
make plot
Cette commande trace les courbes du fichier perfs.dat. Vous pouvez choisir de n'imprimer qu'une partie du code, en affectant "fox", "scatter" ou "gather" à la variable stat.
Par exemple:
make plot stat=scatter

Pour imprimer la courbe de speedup, à partir du fichier spup.dat :
make plot-sp

