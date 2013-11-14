FilePoller
==========

Raspberry PI - Polling sur un fichier sans occuper de CPU


filepoller -[option] [valeur] ... 

 -- PARAM OBLIGATOIRE --
-f file : fichier qui va etre pollé

 -- PARAMS OPTIONNELS --
-t X :   cadence le timer à X milliseconds (  500 par defaut )
-b :     boucle lorsque détecte un changement ( 0 par defaut )
-e :     efface le contenu du fichier apres lecture

 -- PARAMS AFFICHAGES --
-v [X] : Affiche le texte lu dans le fichier, [X] est optionnel
      X = 0, 1 ou 2 -> + ou - de Verbosity ( 0 par defaut, 1 quand l'option est appelée)
-d :     Affiche les infos de debug ( 0 par defaut)
-h :     commande d'aide
