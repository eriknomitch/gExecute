
// DISCLAIMER: I really only worked on this until it worked okay for me.... 
// if I have some time I'll actually make it halfway descent one day
// if you want to bug me about it you can: enomitch@gmail.com if anyone actually uses it

#include <string>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <iostream>
#include <fstream>
#include <libconfig.h++>
#include <vector>
#include <sstream>
#include <stdlib.h>

using namespace std;
using namespace libconfig;

// FIXME: a lot of these dont need to be global
// general 
int actions_loop = 0;
string entry_snoop = "";
string best_match;
string browser_path;

vector<string> match;
vector<string> match_link;
vector<string> actions;
vector<string> url;

// gtk
static GtkWidget *window = NULL;
const gchar *entry_text;
GtkWidget *vbox;
GtkWidget *entry;
GtkWidget *best;
GtkWidget *snoop;

void matches_init (void)
{
    // get config file
    string rc_name = string(getenv("HOME")) + "/.gexecuterc";
    
    Config config;
    config.readFile(rc_name.c_str());
   
    config.lookupValue("browser", browser_path);
    Setting& commands = config.lookup("commands");
    Setting& links    = config.lookup("links");

    // get commands
    for (int i = 0; i < commands.getLength(); i++)
    {
        match.push_back  (commands[i][0]);
        actions.push_back(commands[i][1]);
    }

    // get links
    for (int i = 0; i < links.getLength(); i++)
    {
        match_link.push_back(links[i][0]);
        url.push_back(links[i][1]);
    }
}

void try_match (string send)
{
    int best_length, match_length, send_length, current_length;
    best_length = 0;
    current_length = 0;
    send_length = send.length();

    for (int i = 0; i < match.size(); i++)
    {
        match_length = match[i].length();
        current_length = 0;

        for (int j = 0; j < match_length; j++)
        {
            if (match[i][j] == send[j])
            {
                current_length++;
                if (current_length > best_length)
                {
                    best_length = current_length;
                    best_match  = actions[i];
                    gtk_entry_set_text(GTK_ENTRY(best), match[i].c_str());
                }
            }
            else
            {
                break;
            }
        }
    }

    for (int i = 0; i < match_link.size(); i++)
    {
        match_length = match_link[i].length();
        current_length = 0;

        for (int j = 0; j < match_length; j++)
        {
            if (match_link[i][j] == send[j])
            {
                current_length++;
                if (current_length > best_length)
                {
                    best_length = current_length;
                    ostringstream stream;
                    stream << browser_path << url[i];
                    best_match  = stream.str();

                    gtk_entry_set_text(GTK_ENTRY(best), match_link[i].c_str());
                }
            }
            else
            {
                break;
            }
        }
    }
}

static void enter_callback(GtkWidget *widget, GtkWidget *entry)
{
    if (best_match != "")
    {
        best_match += " &";
        system(best_match.c_str());
    }
    gtk_main_quit();
}

static gboolean snooper(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    char c = (char)event->keyval;
    int event_int = (int)event->keyval;

    if (event->type == 8 && event_int == 65288)
    {
        entry_snoop = "";
        best_match = "";
        gtk_entry_set_text(GTK_ENTRY(entry), "");
        gtk_entry_set_text(GTK_ENTRY(best), "");
    }

    else
    if (event->type == 8 && event_int >= 97 && event_int <= 122)
    {
        entry_snoop += c;
        try_match(entry_snoop);
    }

    else
    if (event_int == 65307)
    {
        gtk_main_quit();
    }

    return FALSE;
}

int main(int argc, char *argv[])
{
    matches_init();

    // init gtk and keysnooper
    gtk_init (&argc, &argv);
    gtk_key_snooper_install((GtkKeySnoopFunc)snooper, 0);

    // gtk objects
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    vbox   = gtk_vbox_new (FALSE, 5);
    best   = gtk_entry_new ();
    entry  = gtk_entry_new ();
    
    gtk_entry_set_editable(GTK_ENTRY(best), FALSE);
    gtk_window_set_title (GTK_WINDOW (window), "gExecute");
    
    // pack and align container
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_box_pack_start (GTK_BOX (vbox), entry, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), best, FALSE, FALSE, 0);

    gtk_widget_show_all (window);
   
    // what happens on a wm destroy
    g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (enter_callback), (gpointer) entry);
    
    gtk_main ();

    return 0;
}

