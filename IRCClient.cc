#include <time.h>
//#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
//#include "TestIRCServer.h"

char * user;
char * password;
char * host;
char * sport;
int port;
int open_client_socket(char * host, int port) {
	// Initialize socket address structure
	struct  sockaddr_in socketAddress;
	
	// Clear sockaddr structure
	memset((char *)&socketAddress,0,sizeof(socketAddress));
	
	// Set family to Internet 
	socketAddress.sin_family = AF_INET;
	
	// Set port
	socketAddress.sin_port = htons((u_short)port);
	
	// Get host table entry for this host
	struct  hostent  *ptrh = gethostbyname(host);
	if ( ptrh == NULL ) {
		perror("gethostbyname");
		exit(1);
	}
	
	// Copy the host ip address to socket address structure
	memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);
	
	// Get TCP transport protocol entry
	struct  protoent *ptrp = getprotobyname("tcp");
	if ( ptrp == NULL ) {
		perror("getprotobyname");
		exit(1);
	}
	
	// Create a tcp socket
	int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}
	
	// Connect the socket to the specified server
	if (connect(sock, (struct sockaddr *)&socketAddress,
		    sizeof(socketAddress)) < 0) {
		perror("connect");
		exit(1);
	}
	
	return sock;
}

#define MAX_RESPONSE (10 * 1024)
int sendCommand(char *  host, int port, char * command, char * response) {
	
	int sock = open_client_socket( host, port);

	if (sock<0) {
		return 0;
	}

	// Send command
	write(sock, command, strlen(command));
	write(sock, "\r\n",2);

	//Print copy to stdout
	write(1, command, strlen(command));
	write(1, "\r\n",2);

	// Keep reading until connection is closed or MAX_REPONSE
	int n = 0;
	int len = 0;
	while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
		len += n;
	}
	response[len]=0;

	printf("response:\n%s\n", response);

	close(sock);

	return 1;
}
void command(char*inp) {
	char* ch;
	host = strtok(inp," ");
	sport = strtok(NULL, " ");
	ch = strtok(NULL, " ");
	sscanf(sport, "%d", &port);
	char response[MAX_RESPONSE];
	sendCommand(host, port, ch, response);
}
GtkListStore * list_rooms;
GtkListStore * list_users;
const char *user1;

void update_list_rooms() {
	GtkTreeIter iter;
	int i;

	/* Add some messages to the window */
	for (i = 0; i < 10; i++) {
		gchar *msg = g_strdup_printf ("Room %d", i);
		gtk_list_store_append (GTK_LIST_STORE (list_rooms), &iter);
		gtk_list_store_set (GTK_LIST_STORE (list_rooms), 
				&iter,
				0, msg,
				-1);
		g_free (msg);
	}
}
void update_list_users() {
	GtkTreeIter iter;
	int i;

	/* Add some messages to the window */
	for (i = 0; i < 10; i++) {
		gchar *msg = g_strdup_printf ("User %d", i);
		gtk_list_store_append (GTK_LIST_STORE (list_users), &iter);
		gtk_list_store_set (GTK_LIST_STORE (list_users),
				&iter,
				0, msg,
				-1);
		g_free (msg);
	}
}


/* Create the list of "messages" */
static GtkWidget *create_list( const char * titleColumn, GtkListStore *model )
{
	GtkWidget *scrolled_window;
	GtkWidget *tree_view;
	//GtkListStore *model;
	GtkCellRenderer *cell;
	GtkTreeViewColumn *column;

	int i;

	/* Create a new scrolled window, with scrollbars only if needed */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC, 
			GTK_POLICY_AUTOMATIC);

	//model = gtk_list_store_new (1, G_TYPE_STRING);
	tree_view = gtk_tree_view_new ();
	gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
	gtk_widget_show (tree_view);

	cell = gtk_cell_renderer_text_new ();

	column = gtk_tree_view_column_new_with_attributes (titleColumn,
			cell,
			"text", 0,
			NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
			GTK_TREE_VIEW_COLUMN (column));

	return scrolled_window;
}

/* Add some text to our text widget - this is a callback that is invoked
   when our window is realized. We could also force our window to be
   realized with gtk_widget_realize, but it would have to be part of
   a hierarchy first */

static void insert_text( GtkTextBuffer *buffer, const char * initialText )
{
	GtkTextIter iter;

	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
	gtk_text_buffer_insert (buffer, &iter, initialText,-1);
}

/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_text( const char * initialText )
{
	GtkWidget *scrolled_window;
	GtkWidget *view;
	GtkTextBuffer *buffer;

	view = gtk_text_view_new ();
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);

	gtk_container_add (GTK_CONTAINER (scrolled_window), view);
	insert_text (buffer, initialText);

	gtk_widget_show_all (scrolled_window);

	return scrolled_window;
}
static void croom_clicked(GtkWidget *button, gpointer data){
	const char *pass = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)data));
	printf("%s\n", pass);

}
static void signup_clicked(GtkWidget *button, gpointer data){
	const char *pass = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)data));
	printf("%s-%s\n", user1, pass);
}
static void signin_clicked(GtkWidget *button, gpointer data){
const char *pass = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)data));
printf("%s\n", user1);
command("localhost 2011 ADD-USER abhiga abhiga");
}
static void user_callback(GtkWidget *button, gpointer data) {
	user1 = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)data));
	//printf("%s\n", user1);
}

int main( int   argc,
		char *argv[] )
{
	GtkWidget *window;
	GtkWidget *list;
	GtkWidget *messages;
	GtkWidget *myMessage;
	GtkWidget *label;
	GtkWidget *croom;
	GtkWidget *user;
	GtkWidget *pass;
	gtk_init (&argc, &argv);
	//label = gtk_label_new("Username");
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "IRC Client");
	g_signal_connect (window, "destroy",
			G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window), 850, 800);

	// Create a table to place the widgets. Use a 7x4 Grid (7 rows x 4 columns)
	GtkWidget *table = gtk_table_new (7, 5, TRUE);
	gtk_container_add (GTK_CONTAINER (window), table);
	gtk_table_set_row_spacings(GTK_TABLE (table), 5);
	gtk_table_set_col_spacings(GTK_TABLE (table), 5);
	gtk_widget_show (table);

	list_users = gtk_list_store_new (1, G_TYPE_STRING);
	update_list_users();
	list = create_list ("Users", list_users);
        gtk_table_attach_defaults (GTK_TABLE (table), list, 1,2, 0, 2);
        gtk_widget_show (list);

	// Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
	list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
	update_list_rooms();
	list = create_list ("Rooms", list_rooms);
	gtk_table_attach_defaults (GTK_TABLE (table), list, 0, 1, 0, 2);
	gtk_widget_show (list);
	
	label = gtk_label_new("Username");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 2,3,5,6);
	gtk_widget_show(label);
	label = gtk_label_new("Password");
	gtk_table_attach_defaults(GTK_TABLE(table), label, 2,3,6,7);
         gtk_widget_show(label);
	// Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
	messages = create_text ("Server Message List");
	gtk_table_attach_defaults (GTK_TABLE (table), messages, 2, 5, 0, 5);
	gtk_widget_show (messages);
	// Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 

	myMessage = create_text ("Type your message here.");
	gtk_table_attach_defaults (GTK_TABLE (table), myMessage, 0, 2, 2, 5);
	gtk_widget_show (myMessage);

	// Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
	GtkWidget *send_button = gtk_button_new_with_label ("Send Message");
	gtk_table_attach_defaults(GTK_TABLE (table), send_button, 0, 1, 5, 6); 
	gtk_widget_show (send_button);

	GtkWidget *create_room = gtk_button_new_with_label ("Create Room");
	gtk_table_attach_defaults(GTK_TABLE (table), create_room, 0, 1, 6,7);
	gtk_widget_show (create_room);
	
	croom = gtk_entry_new ();
	//gtk_entry_set_max_length (GTK_ENTRY (croom), 50);
	//g_signal_connect (croom, "activate",
	//G_CALLBACK (entry_callback),
	//croom);
	g_signal_connect(G_OBJECT(create_room), "clicked", G_CALLBACK(croom_clicked),croom);
	gtk_table_attach_defaults(GTK_TABLE (table), croom, 1,2, 6, 7);
	gtk_widget_show (croom);

	user = gtk_entry_new ();
	g_signal_connect(user, "activate", G_CALLBACK(user_callback),user);
	//user1 = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)user));
	gtk_table_attach_defaults(GTK_TABLE (table), user, 3,4, 5, 6);
	gtk_widget_show (user);
	
	pass = gtk_entry_new ();
	//gtk_entry_set_visibility(GTK_ENTRY(pass), FALSE);
	gtk_table_attach_defaults(GTK_TABLE (table), pass, 3,4, 6, 7);
	gtk_widget_show (pass);

	GtkWidget *leave_room = gtk_button_new_with_label ("Leave Room");
	gtk_table_attach_defaults(GTK_TABLE (table), leave_room, 1, 2, 5,6);
	gtk_widget_show (leave_room);

	GtkWidget *sign_up = gtk_button_new_with_label ("Sign Up");
	g_signal_connect(G_OBJECT(sign_up), "clicked", G_CALLBACK(signup_clicked),pass);
	gtk_table_attach_defaults(GTK_TABLE (table), sign_up, 4,5, 5,6);
	gtk_widget_show (sign_up);

	GtkWidget *sign_in = gtk_button_new_with_label ("Sign In");
	g_signal_connect(G_OBJECT(sign_in), "clicked", G_CALLBACK(signin_clicked),pass);
	gtk_table_attach_defaults(GTK_TABLE (table), sign_in, 4,5, 6,7);
	gtk_widget_show (sign_in);




	gtk_widget_show (table);
	gtk_widget_show (window);

	gtk_main ();

	return 0;
}

