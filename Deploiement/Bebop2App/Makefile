#Ceci est le début du makefile qui lancera le script d'installation

#Default rule displaying help
help :
	@echo "#-------------------------[ AIDE ]-------------------------#"
	@echo "'make install'   -> Installe l'application sur la machine"
	@echo "'make uninstall' -> Désinstalle l'application (supprime les fichiers de l'applications)"
	@echo "'make'  	 -> Affiche cette page d'aide"
	@echo "#-------------------------[ AIDE ]-------------------------#"

#Main install command running the install script
install :
	@echo "Préparation de l'installation de l'application"
	@./install.sh

#Uninstall command to remove all the files of the application
uninstall :
	@echo "Préparation de la suppression de l'application"
	@./uninstall.sh

#Casual PHONY rule
.PHONY : help install uninstall