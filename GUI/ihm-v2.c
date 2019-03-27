#include <stdlib.h>
#include <gtk/gtk.h>

//_________________________________________________________________________________
//CALLBACKS


//FONCTION QUI OUVRE L'APPLICATION DE SAISIE DU PLAN DE VOL DANS UN ONGLET DU NAVIGATEUR INTERNET PAR DEFAUT
void cb_open_plan(){
    if(fork() == 0){
        printf("Ouverture de l'application de création de plan de vol dans un nouvel onglet de votre moteur de recherche !\n\n");
        execlp("xdg-open","xdg-open","plan_de_vol.html", NULL);
        perror("execlp");
    }
}

//FONCTION QUI OUVRE UN FILE CHOOSER DASN UNE NOUVELLE FENETRE ET RECUPERE LE PATH DU FICHIER SELECTIONNE
void cb_open_file(GtkWidget* button, gpointer window){
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Coisissez un plan de vol", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    gtk_widget_show_all(dialog);
    //  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),"/");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), g_get_home_dir());
    gint resp = gtk_dialog_run(GTK_DIALOG(dialog));
    if(resp == GTK_RESPONSE_OK){
        g_print("%s\n", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
    }
    else{
        g_print("Vous avez annulé la sélection de fichier\n");
    }
    gtk_widget_destroy(dialog);
}

//_________________________________________________________________________________

int main(int argc, char ** argv){

	//___________________________________________________________________
	
	GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget 		*flightplanbutton;
    GtkWidget 		*openfilebutton;
    GtkWidget 		*helpbutton;
    GtkWidget 		*infobutton;
 	
 	//___________________________________________________________________

    gtk_init(&argc, &argv);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "ihm-v2.glade", NULL);
 	
 	//___________________________________________________________________

 	//GETTING OBJECTS FROM BUILDER
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    flightplanbutton = GTK_WIDGET(gtk_builder_get_object(builder, "flightplanbutton"));
    openfilebutton = GTK_WIDGET(gtk_builder_get_object(builder, "openfilebutton"));

    //___________________________________________________________________

    //CONNECTION WITH CALLBACKS
    g_signal_connect(G_OBJECT(flightplanbutton), "clicked",G_CALLBACK(cb_open_plan), NULL);
    g_signal_connect(G_OBJECT(openfilebutton), "clicked",G_CALLBACK(cb_open_file), NULL);

    //___________________________________________________________________

    //UNKNOWN ??
    gtk_builder_connect_signals(builder, NULL);
 	
    g_object_unref(builder);
 
    gtk_widget_show(window);
    gtk_main();

    //___________________________________________________________________

	return 0;
}

//_________________________________________________________________________________

//CLEAN EXIT?
void on_window_main_destroy(){
    gtk_main_quit();
}

//_________________________________________________________________________________