  Description:
  This is an implementation of chatClient GUI (Graphical User Interface)communicates with chatServer (https://github.com/abhiga/chatServer). This implemented using the GTK 2.0 library in the Linux environment. The  user of this chat client can perform following functions:
   SignUp
   Login
   Get all Users
   Get Users in a particular room
   Get new messages
   Send Message
   Create Rooms
   Enter Rooms
   List Rooms

   To run this program type “make” and then,
   IRCClient <host> <port>
   where host and host is the host and port correspond to that of the chatServer (https://github.com/abhiga/chatServer).

   The user can enter a room by selecting any of the rooms. The users frame updates with the users in the room in every 5 seconds. If user types a messgae, it will be sent to the server.
    Once every 5 secs, the GUI client contacts the server to get the new messages from the server. The client gets the messages from the server with a number larger than the last message number received.
 
