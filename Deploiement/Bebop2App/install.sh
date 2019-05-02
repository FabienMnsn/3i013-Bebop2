#!/bin/bash

#---------------------------------

#install script

#---------------------------------

#install order :
	#SDK		/staging
	#lampp		/public
	#GTK		/GUI

#---------------------------------

APPLICATION_FOLDER="Bebop2App"
PATH_APPLICATION_FOLDER="/usr/"${APPLICATION_FOLDER}""
PATH_SERVER_FOLDER="/var/www/"

#---------------------------------

if [ -d $PATH_APPLICATION_FOLDER ];then
	echo "Un Dossier de même nom existe déjà. Voulez-vous le supprimer ?"
	select answer in y n
	    do 
	        case $answer in 
		    	# Laisser passer ceux qui répondent correctement à la question
		    	y)
		    		echo "Suppression du dossier existant en cours"
		    		rm -fr $PATH_APPLICATION_FOLDER $PATH_SERVER_FOLDER"public" $PATH_SERVER_FOLDER"public.zip"
					break
					;;
				n)
					echo "Exiting installation"
					exit
					;;
		    	*) 
				continue
				;;
			esac	    
		done
fi

#---------------------------------

echo "----------[ Installation de l'application Bebop2App ]----------"

#---------------------------------

echo "Création du répertoire principal de l'application"
mkdir $PATH_APPLICATION_FOLDER

#---------------------------------

echo "Installation de l'interface graphique (GUI)"	
cp "./GUI.zip" $PATH_APPLICATION_FOLDER
unzip -q $PATH_APPLICATION_FOLDER"/GUI.zip" -d $PATH_APPLICATION_FOLDER
rm -f $PATH_APPLICATION_FOLDER"/GUI.zip"
echo "Fait"

#---------------------------------

echo "Installation du controleur de drone (SDK)"
cp "./staging.zip" $PATH_APPLICATION_FOLDER
unzip -q $PATH_APPLICATION_FOLDER"/staging.zip" -d $PATH_APPLICATION_FOLDER
rm -f $PATH_APPLICATION_FOLDER"/staging.zip"
echo "Fait"

#---------------------------------

echo "Installation des composant sur le serveur local (lampp)"
cp "./public.zip" $PATH_SERVER_FOLDER
unzip -q $PATH_SERVER_FOLDER"public.zip" -d $PATH_SERVER_FOLDER
rm -f $PATH_SERVER_FOLDER"public.zip"
echo "Fait"

#---------------------------------

#lien symbolique marche pas donc on ajoute le path du binaire a $PATH
#ln -s $PATH_APPLICATION_FOLDER"/GUI/Bebop2App" $TMP_BIN_PATH"Bebop2App"
echo 'export PATH=$PATH:/usr/Bebop2App' >> /home/$USER/.bashrc

#---------------------------------

echo "----------[ Installation terminée ! ]----------"

#---------------------------------
#shx corp