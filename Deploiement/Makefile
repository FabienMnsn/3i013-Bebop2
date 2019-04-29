#Ceci est le début du makefile qui lancera le script d'installation

#Default rule displaying help
help :
	echo "Pour installer l'application tapez la commande : 'make install'"
	echo "Pour désinstaller l'application et supprimer tous les fichiers tapez la comande : make uninstall"

#Main install command running the install script
install :
	echo "Préparation de l'installation de l'application"
	./install.sh

#Uninstall command to remove all the files of the application
uninstall :
	echo "Préparation de la suppression de l'application"
	./uninstall.sh

#Casual PHONY rule
.PHONY : help install uninstall