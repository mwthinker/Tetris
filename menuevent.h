#ifndef MENUEVENT_H
#define MENUEVENT_H

enum MenuEvent {MENUEVENT_STARTGAME,		// Starts the game.
				MENUEVENT_RESUMEGAME,		// From front page, to resume a current game.
				MENUEVENT_READYTOSTART,     // Set the game ready to start.
				MENUEVENT_LOCALGAME,		// Goes to local game screen.				
				MENUEVENT_SERVERCREATE,		// Goes to client game screen
				MENUEVENT_SERVERGAME,		// Goes to client game screen.
				MENUEVENT_SERVERLOOBY,
				MENUEVENT_CLIENTCREATE,
				MENUEVENT_CLIENTGAME,		// Goes to server game screen.
				MENUEVENT_CLIENTLOOBY,
				MENUEVENT_SETTINGS,
				MENUEVENT_CHANGENBROFPLAYERS,		// Goes to server game screen.
				
				MENUEVENT_RESTART,          // Restart game.
				MENUEVENT_MENU,				// Goes back to front page.
				MENUEVENT_EXIT,				// Exit the program.
				MENUEVENT_NOTHING,			
				MENUEVENT_PAUSE,			
				MENUEVENT_FULLSCREEN};		// Toggles fullscreen on/off.

#endif // MENUEVENT_H