#!/bin/bash

#---------------------------------

#uninstall script

#---------------------------------

PATH_APPLICATION_FOLDER="/usr/Bebop2App"
PATH_SERVER_FOLDER="/var/www/"

#---------------------------------

echo "----------[ Désinstallation de l'application Bebop2App ]----------"
echo "Suppression des fichiers de l'application (GUI)"
rm -fr $PATH_APPLICATION_FOLDER"/GUI" $PATH_APPLICATION_FOLDER"/GUI.zip"

#---------------------------------

echo "Suppression des fichiers de l'application (controleur du drone)"
rm -fr $PATH_APPLICATION_FOLDER"/staging" $PATH_APPLICATION_FOLDER"/staging.zip"

#---------------------------------

echo "Suppression des fichier du serveur local (lampp)"
rm -fr $PATH_SERVER_FOLDER"public" $PATH_SERVER_FOLDER"public.zip"

#---------------------------------

echo "Suppression du dossier principal de l'application"
rm -fr $PATH_APPLICATION_FOLDER

#---------------------------------

echo "----------[ Désinstallation terminée ! ]----------"

