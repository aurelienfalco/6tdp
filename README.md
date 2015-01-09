###############
# Compilation #
###############
make

#############
# Execution #
#############
make exec n=a m=size seq=bool
Paramètres (optionnels):
- n : le nombre de processus ;
- m : la taille des blocs de découpage de la matrice ;
- seq : un entier. Si égal à 0 : parallèle, sinon séquentiel ;
- p : un entier. Si différent de 0, imprime les matrices L, U, et A finale.


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

