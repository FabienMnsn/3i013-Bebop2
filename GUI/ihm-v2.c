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

//GLOBAL VAR AND STRUC MAYBE?
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
		GTK_WINDOW(window_combo_box),
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
		printf("Fichier sélectionné : %s, paré au decollage !\n", path);
		//fork + exec en passant le path du plan de vol en parametre de l'executable
		printf("Partie incomplète, il manque le fork+exec du code C du drone\n");
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
    window_help = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window_help), "A propos");
    gtk_window_set_default_size(GTK_WINDOW(window_help), 640, 400);
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
	}
	if(strlen(path) != 0){
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