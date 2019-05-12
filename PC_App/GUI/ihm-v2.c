#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <gmodule.h>

#define MAVLINK_DIR "/opt/lampp/htdocs/plan_de_vol_drone/Mavlinks"
#define CREATE_FLIGHT_PLAN_PHP_PATH "http://localhost:4200/plan_de_vol_drone/plan_de_vol.php"

//USEFULL LINK
// http://gtk.developpez.com/cours/gtk2/?page=page_22
// compile : gcc ihm.c -o ihm `pkg-config --cflags --libs gtk+-3.0`

//_________________________________________________________________________________

//GLOBAL VAR
GtkBuilder *builder;

GtkWidget *window_main;
GtkWidget *flightplanbutton;
GtkWidget *openfilebutton;
GtkWidget *startbutton;
GtkWidget *helpbutton;


GtkWidget *window_combo_box;
GtkWidget *combobox;
GtkWidget *selectbutton;
GtkWidget *deletebutton;

char path[32];
int isSelectSelected = 0;
int validate = 0;
char file_table[32][32];
int nb_file = 0;

//_________________________________________________________________________________

void print_file_table(){
	int i;
	for(i = 0; i < 32; i++){
		printf("file_table[%d] (%s)\n", i, file_table[i]);
	}	
}

void init_file_table(){
	int i;
	for(i = 0; i < 32; i++){
		strcpy(file_table[i], "");
	}
	strcpy(file_table[0], "Selectionnez un plan de vol");
}

//_________________________________________________________________________________

void info_data(){
	printf("__________________\npath : %s\nselected : %d\n__________________\n", path, isSelectSelected);
}

//_________________________________________________________________________________

//CREATION DIALOG INFO WINDOW
void create_info_window(GtkWidget *parent, char *message){
	GtkWidget *dialogInfo = gtk_message_dialog_new(
		GTK_WINDOW(parent),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        message);
	gtk_window_set_title(GTK_WINDOW(dialogInfo), "Information");
  	gtk_dialog_run(GTK_DIALOG(dialogInfo));
  	gtk_widget_destroy(dialogInfo);
}

//CREATION DIALOG ERROR WINDOW
void create_error_window(GtkWidget *parent, char *message){
	GtkWidget *dialogError;
  		dialogError = gtk_message_dialog_new(
  			GTK_WINDOW(parent),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            message);
  		gtk_window_set_title(GTK_WINDOW(dialogError), "Erreur");
  		gtk_dialog_run(GTK_DIALOG(dialogError));
  		gtk_widget_destroy(dialogError);
}

//CREATION DIALOG VALIDATION WINDOW
int create_validate_window(GtkWidget *parent){
	GtkWidget *dialogValidate;
	dialogValidate = gtk_message_dialog_new(
		GTK_WINDOW(parent),
	    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Voulez vous vraiment supprimer ce fichier?");
	gtk_window_set_title(GTK_WINDOW(dialogValidate), "Alerte");
	int res = gtk_dialog_run(GTK_DIALOG(dialogValidate));
	if(res == GTK_RESPONSE_YES){
		validate = 1;
	}
	else{
		if(res == GTK_RESPONSE_NO){
		  	validate = -1;
		}
	}
	gtk_widget_destroy(dialogValidate);
}

//REMOVE THE ACTIVE TEXT TO UPDATE THE COMBO LIST

void update_combo_list(){
	//gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(combobox), gtk_combo_box_get_active(GTK_COMBO_BOX(combobox)));
	gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combobox));
	DIR *rep;
    char *file_name = NULL;
    
    if(nb_file > 0){
        	int j;
        	for(j = 0; j < nb_file+1; j++){
        		gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(combobox),j);
        	}
    }
    //reset nb_files before recalculating
    nb_file	= 0;
    // path du répertoire à changer
    rep = opendir(MAVLINK_DIR);
    if( rep != NULL){
        struct dirent *lecture;
        //remplissage du tableau avec les noms des fichiers trouvés
        while(lecture = readdir(rep)){
            if(strlen(lecture->d_name) > 8){
                strcpy(file_table[nb_file+1], lecture->d_name);
                nb_file++;
            }
        }
        closedir(rep);


        int i;
        //on suppose que le nom du fichier ne dépasse pas 32 characteres sinon on est mort
        for(i = 0; i < nb_file+1; i++){
            if(g_locale_to_utf8(file_table[i], -1, NULL, NULL, NULL)){
            	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), g_locale_to_utf8(file_table[i], -1, NULL, NULL, NULL));
            }
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0);
	}
}


//_________________________________________________________________________________

//CALLBACKS
void on_widget_deleted(GtkWidget *widget){
	gtk_widget_hide(widget);
}

//CALLBACKS MAIN WINDOW
void on_flightplanbutton_clicked(){
	if(fork() == 0){
		execlp("xdg-open","xdg-open", CREATE_FLIGHT_PLAN_PHP_PATH, NULL);
        //execlp("xdg-open","xdg-open", CREATE_FLIGHT_PLAN_PHP_PATH, NULL);
        perror("execlp");
    }
}

void on_openfilebutton_clicked(){
	gtk_widget_show(window_combo_box);
	update_combo_list();
}

void on_startbutton_clicked(){
	if(isSelectSelected && strlen(path) != 0){		
		if (fork() == 0){
			char mavlink_file_path[100];
			strcpy(mavlink_file_path, "/var/www/public/plan_de_vol_drone/Mavlinks/");
			strcat(mavlink_file_path, path);
			//execl("/usr/Bebop2App/staging/native-wrapper.sh", "native-wrapper", "/usr/Bebop2App/staging/usr/bin/BebopSample", mavlink_file_path, NULL);
			execl("echo", "echo", "TOTO ####################################################", NULL);
			perror("execl");
            exit(0);
		}
		else{
			create_info_window(window_main, "Drone paré au décollage !\n Enfilez votre masque FPV.");
		}
	}
	if(!isSelectSelected || strlen(path) == 0){
		GtkWidget *dialog;
  		dialog = gtk_message_dialog_new(
  			GTK_WINDOW(window_main),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Aucun fichier sélectionné !\nVous devez sélectionner un fichier !\n\nPour plus d'aide reportez-vous à la section 'Aide'.");
  		gtk_window_set_title(GTK_WINDOW(dialog), "Erreur");
  		gtk_dialog_run(GTK_DIALOG(dialog));
  		gtk_widget_destroy(dialog);
	}
}

void on_helpbutton_clicked(){
	GtkWidget *window_help;
    GtkWidget *notebook;
    GtkWidget *pageContent;
    GtkWidget *pageNumber;

    int i = 1;

    gchar *pagecontent;
    gchar *pagenumber;


    window_help = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window_help), "A propos");
    gtk_window_set_default_size(GTK_WINDOW(window_help), 640, 400);

    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    gtk_container_add(GTK_CONTAINER(window_help), notebook);

    //page 1

    pagecontent = "Bienvenue dans la section d'aide.\n\nVous trouverez, dans l'ordre, de l'aide concernant :\n\n\tPrérequis au bon fonctionnement de l'application\n\tLa création d'un plan de vol sur votre navigateur web ainsi que la sauvegarde de ce dernier.\n\tLa sélection d'un plan de vol parmis ceux déja enregistrés sur votre machine.\n\tLa suppression d'un plan de vol enregistré sur votre machine.\n\tL'execution du plan de vol sélectionné.\n\tL'installation et la désinstallation de l'application";
    pagenumber = g_strdup_printf("Page %d", i);
    pageContent = gtk_label_new(pagecontent);
    pageNumber = gtk_label_new(pagenumber);
    gtk_label_set_justify(GTK_LABEL(pageContent), GTK_JUSTIFY_LEFT);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageContent, pageNumber);
    i++;    

    //page 2

    pagecontent = "PREREQUIS POUR L'APPLICATION :\n\nPour assurer un installation sans erreur de l'application vous devez avoir installé :\nFFmpeg\nLa librairie graphique gtk3 (sous Ubuntu 18.04 gtk3 est déjà installé)\nUn serveur local lampp (tutoriel facilement trouvable sur le net) que vous devez démarrer avant de lancer l'application principale.\nVous devez vous assurer que votre navigateur web est bien l'application par défaut pour ouvrir des fichiers html et php.\n\nSi vous avez réuni ces points vous pouvez installer l'application.\n(reportez-vous à la section 'INSTALLER ET SUPPRIMER L'APPLICATION' pour plus d'informations)";
    pagenumber = g_strdup_printf("Page %d", i);
    pageContent = gtk_label_new(pagecontent);
    pageNumber = gtk_label_new(pagenumber);
    gtk_label_set_justify(GTK_LABEL(pageContent), GTK_JUSTIFY_LEFT);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageContent, pageNumber);
    i++;

    //page 3

    pagecontent = "CREATION D'UN PLAN DE VOL :\n\nPour créer un plan de vol il faut, dans un premier temps, lancer l'application principale puis cliquer sur le premier des quatres boutons en commençant par la gauche.\nUne fois que vous aurez cliqué dessus, une page web s'ouvrira dans votre navigateur web.\nLa fenetre principale affiche une carte géographique que vous pouvez manipuler avec la souris (zoom avant, zoom arrière, déplacement, etc) comme on pourrai le faire sur google maps.\nAvec un clic gauche de la souris vous ajoutez un point de passage.\nPour supprimer ce point de passage, cliquer sur le bouton portant le libelé 'adding', situé en bas à gauche de votre écran.\nCe bouton permet de passer en mode ajout ou suppression de point de passage.\nChaque point de passage possède une altitude propre que vous pouvez définir en cliquant sur le point de passage.\nUne boite de dialogue s'ouvrira vous demandant de saisir une altitude pour ce point.\nConcernant l'altitude, vous trouverez, en bas à gauche de votre écran, un champ de saisie vous permettant de choisir une altitude par défaut pour tous les points de passage.\nCela vous évite de rentrer chaque altitude manuellement.\nUne fois le plan de vol tracé, il vous faudra cliquer sur le bouton 'export' situé en bas à gauche de votre écran pour sauvegarder votre plan de vol sur votre machine.\nUne boite de dialogue demandant un nom de fichier vous sera alors affichée.\nRemplissez le champ ou laissez le par défaut et cliquez sur 'ok' pour sauvegarder le fichier.\n\nVoila votre plan de vol est maintenant bien sauvegardé sur votre machine !";
    pagenumber = g_strdup_printf("Page %d", i);
    pageContent = gtk_label_new(pagecontent);
    pageNumber = gtk_label_new(pagenumber);
    gtk_label_set_justify(GTK_LABEL(pageContent), GTK_JUSTIFY_LEFT);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageContent, pageNumber);
    i++;

    //page 4

    pagecontent = "SELECTION D'UN PLAN DE VOL :\n\nEn cliquant sur le deuxième bouton en partant de la gauche (icone de dossier) une boite de dialogue vous sera affichée.\nCette boite possède une liste déroulante vous permettant de parcourir tous les plan de vols enregistrés sur votre machine.\nUne fois le plan de vol sélectionné dans la liste déroulante vous devez cliquer sur le bouton 'Sélectionner' sinon la sélection ne sera pas prise en compte.\n\nVoila vous avez maintenant sélectionné un plan de vol vous êtes paré au décollage !";
    pagenumber = g_strdup_printf("Page %d", i);
    pageContent = gtk_label_new(pagecontent);
    pageNumber = gtk_label_new(pagenumber);
    gtk_label_set_justify(GTK_LABEL(pageContent), GTK_JUSTIFY_LEFT);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageContent, pageNumber);
    i++;

    //page 5

    pagecontent = "SUPRESSION D'UN PLAN DE VOL :\n\nEn cliquant sur le même bouton (icone de dossier) vous avez la possibilité de supprimer un plan de vol.\nPour cela, il faut dans un premier temps le sélectionner grâce à la liste déroulant puis cliquer sur le bouton 'Supprimer' situé en bas à droite de la fenêtre de dialogue.\n\nATTENTION une suppression est définitive et irréversible !";
    pagenumber = g_strdup_printf("Page %d", i);
    pageContent = gtk_label_new(pagecontent);
    pageNumber = gtk_label_new(pagenumber);
    gtk_label_set_justify(GTK_LABEL(pageContent), GTK_JUSTIFY_LEFT);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageContent, pageNumber);
    i++;

    //page 6

    pagecontent = "EXECUTION DU PLAN DE VOL :\n\nPour executer un plan de vol il vous faudra au préalable avoir sélectionner ce dernier (pour plus d'informations reportez-vous à la section 'SELECTION D'UN PLAN DE VOL').\nUne fois votre plan de vol sélectionné, vous pouvez cliquer sur le troisième bouton en partant de la gauche (icone 'start' vert).\nL'execution du plan de vol est alors lancée et votre drone décolle !\n\nVous n'avez alors plus qu'a chausser le casque FPV (first person view) pour voir en direct (moyenant un léger délais) ce que voit le drone !";
    pagenumber = g_strdup_printf("Page %d", i);
    pageContent = gtk_label_new(pagecontent);
    pageNumber = gtk_label_new(pagenumber);
    gtk_label_set_justify(GTK_LABEL(pageContent), GTK_JUSTIFY_LEFT);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageContent, pageNumber);
    i++;

    //page 7

    pagecontent = "INSTALLATION ET SUPPRESSION DE L'APPLICATION :\n\nL'installation et la suppression de l'application ce fait via un makefile.\nPour installer l'application tapez 'make install' dans le dossier principal de l'application.\nPour supprimer l'application tapez 'make uninstall' dans le dossier principal de l'application.\nLes fichiers de l'application sont installés dans /usr/Bebop2App ainsi que dans /var/www/public";
    pagenumber = g_strdup_printf("Page %d", i);
    pageContent = gtk_label_new(pagecontent);
    pageNumber = gtk_label_new(pagenumber);
    gtk_label_set_justify(GTK_LABEL(pageContent), GTK_JUSTIFY_LEFT);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pageContent, pageNumber);
    i++;

    gtk_widget_show_all(window_help);
}

//_________________________________________

//CALLBACKS COMBO BOX WINDOW
/*
void on_combobox_changed(){
}
*/

void on_selectbutton_clicked(){
	if(strcmp(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox)),"Selectionnez un plan de vol") != 0){
		sprintf(path, "%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox)));
		isSelectSelected = 1;
		gtk_widget_hide(window_combo_box);
	}
	else{	
		create_error_window(window_combo_box, "Auncun fichier sélectionné !");
	}
}

void on_deletebutton_clicked(){

	if(strcmp(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox)),"Selectionnez un plan de vol") != 0){
		sprintf(path, "%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox)));
		create_validate_window(window_combo_box);
		switch(validate){
			case 0:
				printf("ERROR UNKNOW VALUE OF VALIDATE\n");
				break;
			
			case 1:
				//delete the file in an other process
				if(fork() == 0){
					char buff[64];
					sprintf(buff, "");
					strcat(buff, MAVLINK_DIR);
					strcat(buff, "/");
					strcat(buff, path);
			        execlp("rm","rm", "-f", buff, NULL);
			        perror("execlp");
			    }
			    //reset the vars on main process
			    gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(combobox), gtk_combo_box_get_active(GTK_COMBO_BOX(combobox)));
			    sprintf(path, "%s", "");
				isSelectSelected = 0;
				update_combo_list();
				break;
			
			case -1:
				break;
		}
	}
	else{
		create_error_window(window_combo_box, "Auncun fichier sélectionné !");
	}
	update_combo_list();
}

//_________________________________________________________________________________

//CLEAN EXIT?
gint CloseAppWindow (GtkWidget *widget, gpointer *data){
    gtk_main_quit ();
    return (FALSE);
}

//_________________________________________________________________________________

int main(int argc, char ** argv){

    init_file_table();

    //___________________________________________________________________

    gtk_init(&argc, &argv);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "ihm-v2.glade", NULL);
 	
 	//___________________________________________________________________

 	//GETTING OBJECTS FROM BUILDER
    window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    flightplanbutton = GTK_WIDGET(gtk_builder_get_object(builder, "flightplanbutton"));
    openfilebutton = GTK_WIDGET(gtk_builder_get_object(builder, "openfilebutton"));
    startbutton = GTK_WIDGET(gtk_builder_get_object(builder, "startbutton"));
    helpbutton = GTK_WIDGET(gtk_builder_get_object(builder, "helpbutton"));

    window_combo_box = GTK_WIDGET(gtk_builder_get_object(builder, "window_combo_box"));
    combobox = GTK_WIDGET(gtk_builder_get_object(builder, "combobox"));
    selectbutton = GTK_WIDGET(gtk_builder_get_object(builder, "selectbutton"));
    deletebutton = GTK_WIDGET(gtk_builder_get_object(builder, "deletebutton"));
    //___________________________________________________________________

    /*
	//FILLING COMBOBOX TEXT
    DIR *rep;
    char *file_name = NULL;
    
    // path du répertoire à changer
    rep = opendir(MAVLINK_DIR);
    if( rep != NULL){
        struct dirent *lecture;
        //remplissage du tableau avec les noms des fichiers trouvés
        while(lecture = readdir(rep)){
            if(strlen(lecture->d_name) > 8){
                strcpy(file_table[nb_file+1], lecture->d_name);
                nb_file++;
            }
        }
        closedir(rep);

        int i;
        //on suppose que le nom du fichier ne dépasse pas 32 characteres sinon on est mort
        char str[32] = "";
        for(i = 0; i < nb_file+1; i++){
            if(g_locale_to_utf8(file_table[i], -1, NULL, NULL, NULL)){
            	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), g_locale_to_utf8(file_table[i], -1, NULL, NULL, NULL));
            }
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0);
    }
    */
	

    //___________________________________________________________________

    //CONNECTION WITH CALLBACKS
    g_signal_connect(G_OBJECT(flightplanbutton), "clicked",G_CALLBACK(on_flightplanbutton_clicked), NULL);
    g_signal_connect(G_OBJECT(openfilebutton), "clicked",G_CALLBACK(on_openfilebutton_clicked), NULL);
    g_signal_connect(G_OBJECT(startbutton), "clicked",G_CALLBACK(on_startbutton_clicked), NULL);
    g_signal_connect(G_OBJECT(helpbutton), "clicked",G_CALLBACK(on_helpbutton_clicked), NULL);

    g_signal_connect(G_OBJECT(window_combo_box), "delete-event", G_CALLBACK(on_widget_deleted), NULL);
    g_signal_connect(G_OBJECT(selectbutton), "clicked", G_CALLBACK(on_selectbutton_clicked), NULL);
    g_signal_connect(G_OBJECT(deletebutton), "clicked", G_CALLBACK(on_deletebutton_clicked), NULL);
	
    //___________________________________________________________________

    //CONNECT DELETE EVENT WITH PROPER DESTROY
    g_signal_connect (G_OBJECT(window_main), "delete_event",G_CALLBACK(CloseAppWindow), NULL);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show(window_main);
    gtk_main();

    //___________________________________________________________________

	return 0;
}

//_________________________________________________________________________________