/******************************************************************************************************************\
		
		-----=====	PinPoller =====-----
	
	Va poller un pin GPIO d'un Raspberry Pi toute les 500 milliseconds par défaut
	Va alerter en fonction des paramètres :
	 - (Edge) sur un front montant, descendant ou les 2.
	 - en continu si le mode boucle est activé
	 - en affichant plus ou moins d'information en fonction de la verbosité

	Une place est laissée libre pour ajouter des actions personnelles dans la fonction "void action_perso( void )" 
	... Pratique en mode boucle et sans verbosité.
	Mais peut etre utilisé en script en retirant le mode boucle et affichant une verbosité minimum (1)
	
	
	Pour utiliser ce source, il est necessaire soit d'installer le paquet libboost-dev, 
	soit de récupérer les sources asio.hpp et posix_time.hpp
	
	bug(s) : 
		- en mode boucle, ne ferme pas le GPIO car on est obligé de killer le programme.
		- ne prend pas le X de l'option -v ....
	
	sources :
		- http://www.blaess.fr/christophe/2012/11/26/les-gpio-du-raspberry-pi/
		- http://www.boost.org/
		  plus préscisément : http://www.boost.org/doc/libs/1_35_0/doc/html/boost_asio/tutorial/tuttimer1.html
		
	
	Développé par Clément ORTIZ (08/11/2013), Open Source ;)
	
\*******************************************************************************************************************/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


int bDebug = 0;						// affiche-t-on les infos de debug.
char sFileName[255]; 				// nom du fichier à checker.



////////////////////////////////////
// Déclaration Fonction(s) Perso
void action_perso( char* );



char *str_sub (const char *s, unsigned int start, unsigned int end)
{
   if( bDebug )
		std::cout << "SUB_STR - s: " << s << " Start: " << start << " End: " << end << "\n";
		
	char *new_s = NULL;

   if (s != NULL && start < end){
      new_s = (char *)malloc (sizeof (*new_s) * (end - start + 2));
      if (new_s != NULL){
         int i;
         for (i = start; i <= end; i++){
            new_s[i-start] = s[i];
         }
         new_s[i-start] = '\0';
      }else{
         fprintf (stderr, "Memoire insuffisante\n");
         exit (EXIT_FAILURE);
      }
   }
   if( bDebug )
		std::cout << "New_s : " << new_s;
   return new_s;
}


int main(int argc,char **argv)
{

	////////////////////////////////
	// Parsing des parametres.
	long lTime = 		500; 			// temps d'attente en sec.
	int bVerbose = 		0;				// affiche-t-on le texte lu?
	bool bBoucle = 		0;				// boucle-t-on lorsqu'on detecte une différence?
	bool bQuit = 		0;				// Quitte-t-on la boucle?
	bool bErase = 		false;			// Efface ou non le contenu du fichier
	FILE * pFile;						// Pointeur sur le fichier
	long lSize;							// Taille du fichier
	long lSize_old;						// Taille de comparaison du fichier

	// Pour checker si -f a bien été demandé par la suite.
	strcpy( sFileName, "file.test" );
	
	
	for( int i=1; i < argc; i++ ){
		switch(argv[i][1]){
			case 'h':
				std::cout << "t_synchro -[option] [valeur] ... \n\n";
				std::cout << " -- PARAM OBLIGATOIRE --\n";
				std::cout << "-f file : fichier qui va etre pollé \n";
				std::cout << " -- PARAMS OPTIONNELS --\n";
				std::cout << "-t X :   cadence le timer à X milliseconds ( ";
				std::cout << lTime;
				std::cout << " par defaut )\n";
				std::cout << "-b :     boucle lorsque détecte un changement ( 0 par defaut )\n";
				std::cout << "-e :     efface le contenu du fichier apres lecture\n\n";
				std::cout << " -- PARAMS AFFICHAGES --\n";
				std::cout << "-v [X] : Affiche le texte lu dans le fichier, [X] est optionnel\n         X = 0, 1 ou 2 -> + ou - de Verbosity ( 0 par defaut, 1 quand l'option est appelée)\n";
				std::cout << "-d :     Affiche les infos de debug ( 0 par defaut)\n";
				std::cout << "-h :     commande d'aide \n\n";
				exit(0);
			break;
			case 'f':
				strcpy(sFileName, argv[++i]);  // on récupère le nom du fichier et on incrémente le compteur d'argument
			break;
			case 't':
				lTime = atol( argv[++i] ); // on récupère le temps en seconde et on incrémente le compteur d'argument
			break;
			case 'v':
				bVerbose = 1;
				// il n'est pas obligatoire de mettre un chiffre apres l'option -v, mais s'il y a en un alors bVerbose doit le retenir.
				// if( i+1 <= argc ){
					// if( argv[i+1][0] >= '0' &&  argv[i+1][0] >= '9' ){
						// i++;
						// bVerbose = argv[i][0] - '0';
					// }
				// }
			break;
			case 'd':
				bDebug = 1;
			break;
			case 'e':
				bErase = 1;
			break;
			case 'b':
				bBoucle = 1;
			break;
			default:
				std::cout << "Parametre(s) incorrect(s), ";
				std::cout << argv[0];
				std::cout << " -h pour avoir de l'aide\n";
				exit(4);
			break;
		}
	} 
	
	// check si on a bien initialisé le Fichier
	if( strcmp( sFileName, "file.test" ) == 0 ){
		std::cout << "Parametre(s) incorrect(s), ";
		std::cout << argv[0];
		std::cout << " -h pour avoir de l'aide\n";
		std::cout << "-f FICHIER est obligatoire\n\n";
		exit(4);
	}
	
	// Affichage de debug
	if( bDebug ){
		std::cout << "Parametres :\n";
		std::cout << "- Fichier : ";
		std::cout << sFileName;
		std::cout << "\n";
		std::cout << "- Timer : ";
		std::cout << lTime;
		std::cout << " milliseconds\n";
		std::cout << "- Boucle : ";
		std::cout << bBoucle;
		std::cout << " \n";
		std::cout << "- Ecrase : ";
		std::cout << bErase;
		std::cout << " \n";
	}
	// Info Verbosity
	if( bVerbose > 1 || bDebug){
		std::cout << "(Attente de modification du fichier)\n";
	}
	
	
	// On initialise le buffer_old avec la valeur actuelle du fichier.

	// On ouvre le fichier
	pFile = fopen ( sFileName , "r" );
	if (pFile==NULL) {fputs ("File error\n",stderr); exit (1);}

	// Récupère la taille initiale du fichier
	fseek (pFile , 0 , SEEK_END);
	lSize_old = ftell (pFile);
	rewind (pFile);
	
/* 	// Allocation de la memoire en variable pour la taille du fichier
	buffer_old = (char*) malloc (sizeof(char)*(lSize+1));
	if (buffer_old == NULL) {fputs ("Memory error\n",stderr); exit (2);}

	// Ajout du 0 de fin de chaine
	buffer_old[ lSize ] = '\0';
	
	// Lecture du fichier
	fread (buffer_old,1,lSize,pFile);
 */	// Fermeture du fichier en lecture
	fclose (pFile);

	
	
	
	//////////////////////////
	// Timer
	do{
		// Init du timer a 5sec
		boost::asio::io_service io;
		boost::asio::deadline_timer t(io, boost::posix_time::milliseconds(lTime));
		// attente
		t.wait();
		
		
		//////////////////////////////
		// Fichier
		// FILE * pFile;
		// long lSize;
		char * buffer;
		size_t result;

		// On ouvre le fichier
		pFile = fopen ( sFileName , "r" );
		if (pFile==NULL) {fputs ("File error\n",stderr); exit (1);}

		// Récupère la taille du fichier
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);
		if( bDebug ){
			std::cout << "Taille du fichier : ";
			std::cout << lSize;
			std::cout << "\n";
		}
		
		// si la taille du fichier a changé
		if( lSize > lSize_old ){ 
			// Allocation de la memoire en variable pour la taille du fichier
			// buffer = (char*) malloc (sizeof(char)*(lSize+1));
			long lSize_new = lSize - lSize_old +1;
			buffer = (char*) malloc (sizeof(char)*( lSize_new ));
			if (buffer == NULL) {fputs ("Memory error\n",stderr); exit (2);}
			
			// Ajout du 0 de fin de chaine
			buffer[ lSize_new-1 ] = '\0';

			// On cale le pointeur de lecture
			fseek( pFile , lSize_old , SEEK_SET );

			// Lecture du fichier
			result = fread( buffer, 1, lSize_new, pFile );
			// Fermeture du fichier en lecture
			fclose (pFile);
			
			// on mémorise la nouvelle taille
			if( bErase ){
				lSize_old = 0;
				
				// et on purge le fichier
				pFile = fopen ( sFileName , "w" );
				if (pFile==NULL) {fputs ("File erase error\n",stderr); exit (1);}
				fclose (pFile);
			}else
				lSize_old = lSize;
				
			// Si on ne boucle pas... alors on sort.				
			if( !bBoucle )
				bQuit = 1;
				
			// Affichage du contenu du fichier
			if( bVerbose || bDebug ){
				if( bVerbose > 1 || bDebug )
					std::cout << "Buffer : ";
				if( bVerbose > 0 || bDebug )
					std::cout << buffer << "\n";
				if( bVerbose > 1 || bDebug && bBoucle)
					std::cout << "\n(Attente de modification du fichier)\n";
				std::cout.flush();
			}				
			
			
			action_perso( buffer );
			
			// On purge le buffer
			free (buffer);
		}else{
			fclose (pFile);
			lSize_old = lSize;
		}
	
	}while( !bQuit );
		
	return 0;
}




////////////////////////////////////
// Déclaration Fonction(s) Perso
void action_perso( char* sText ){
	// Un peu de code a faire lors de la detection
	;
}
