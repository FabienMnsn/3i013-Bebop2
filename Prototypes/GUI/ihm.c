#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/wait.h>

// to compile use :  gcc ihm.c -o ihm `pkg-config --cflags --libs gtk+-2.0`


int main(int argc, char *argv[]){

    void button_clicked(){
        if(fork() == 0){
            printf("Openning the flight plan app in a new web browser tab!\n\n");
            execlp("xdg-open","xdg-open","plan_de_vol.html", NULL);
            perror("execlp");
        }
    }

    void open_dialog(GtkWidget* button, gpointer window){
        GtkWidget *dialog;
        dialog = gtk_file_chooser_dialog_new("Chosse a file", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
        gtk_widget_show_all(dialog);
        //  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog),"/");
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), g_get_home_dir());
        gint resp = gtk_dialog_run(GTK_DIALOG(dialog));
        if(resp == GTK_RESPONSE_OK){
            g_print("%s\n", gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
        }
        else{
            g_print("You pressed Cancel\n");
        }
        gtk_widget_destroy(dialog);
    }


    /* defining gtk variables */   
    GtkWidget *window, *button1, *button2, *dialog;
    
    gtk_init(&argc, &argv);
    
    /* main window creation here */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GtkPrototype");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    /* creation a button to start the creation of a flight plan */
    //button1 = gtk_button_new_with_label("Design a flight plan");
    //gtk_widget_set_size_request(button1, 30, 30);
    //g_signal_connect(G_OBJECT(button1), "clicked", G_CALLBACK(button_clicked), NULL);

    /* creation button selection flight plan */
    button2 = gtk_button_new_with_label("Select a flight plan");
    gtk_widget_set_size_request(button2, 30, 30);
    g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(open_dialog), window);

    /* adding the widgets to the main window */
    //gtk_container_add(GTK_CONTAINER(window), button1);
    gtk_container_add(GTK_CONTAINER(window), button2);

    gtk_widget_show_all(window);
    
    /* GUI main */
    gtk_main ();
    
    return 0;
}
//shx