#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/wait.h>

// to compile use :  gcc ihm.c -o ihm `pkg-config --cflags --libs gtk+-2.0`


int main(int argc, char *argv[]){

    /* opens a new tab and load the flight plan designer in your web browser */
    void button_clicked(){
        if(fork() == 0){
            printf("Ouverture de l'application de création de plan de vol dans un nouvel onglet de votre moteur de recherche !\n\n");
            execlp("xdg-open","xdg-open","plan_de_vol.html", NULL);
            perror("execlp");
        }
    }
    /* opens a dialog window where you can choose a file print the path of the file */
    void open_dialog(GtkWidget* button, gpointer window){
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


    /* defining gtk variables */   
    GtkWidget *window, *button1, *button2, *dialog, *vbox, *hbox, *info;
    
    gtk_init(&argc, &argv);
    
    /* main window creation here */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Projet 3i013");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    /* creation Vbox & Hbox */
    vbox = gtk_vbox_new(TRUE, 0);
    hbox = gtk_hbox_new(TRUE,0);
    /* creation a button to start the creation of a flight plan */
    button1 = gtk_button_new_with_label("Créer un plan de vol");
    gtk_widget_set_size_request(button1, 30, 30);
    g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(button_clicked), NULL);

    /* creation button selection flight plan */
    button2 = gtk_button_new_with_label("Choisir un plan de vol");
    gtk_widget_set_size_request(button2, 30, 30);
    g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(open_dialog), window);

    /* creation label */
    //WIP
    //info = gtk_label_new("");
    
    /* adding the widgets to the main window */
    gtk_box_pack_start(GTK_BOX(vbox), button1, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button2, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, TRUE, 0);
    //gtk_box_pack_start(GTK_BOX(hbox), info, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    gtk_widget_show_all(window);
    
    /* GUI main */
    gtk_main ();
    
    return 0;
}
//shx