#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "gamesprite.h"
#include "gamefont.h"

#include "gui/gui.h"
#include "gui/textbutton.h"
#include "gui/textfield.h"
#include "gui/background.h"
#include "mousedoubleclick.h"
#include "pressentertostart.h"
#include "bar.h"
#include "inputipformatter.h"
#include "choosenbrofplayers.h"

#include "tetrisboard.h"

#include "tetrisgame.h"

#include "menuevent.h"

#include "color.h"

#include "gui/glcoordinate.h"

#include <mw/gamewindow.h>
#include <mw/sprite.h>

//#include <guiwindow.h>
#include <SDL_opengl.h>
#include <iostream>

struct View {
public:
    int x, y;
    int width;
    int height;
};

class NetworkLooby : public gui::GuiItem<MenuEvent> {
public:
	NetworkLooby(int x, int y, TetrisGame& tetrisGame, mw::FontPtr font) : tetrisGame_(tetrisGame) {		
		font_ = font;
		x_ = x;
		y_ = y;
		
		header1_ = mw::Text("|Computer|", font, 20);
		header2_ = mw::Text("|Number of Players|", font, 20);
	}
	
	void graphicUpdate(Uint32 deltaTime) {
		unsigned int size = tetrisGame_.getNumberOfConnections();
		// Add text.
		while (texts_.size() < size*2) {
			texts_.push_back(mw::Text("",font_,20));
		}
		// Remove text.
		while (texts_.size() > size*2) {
			texts_.pop_back();			
		}

		// Updates the text to be drawn.
		int index = 0;
		for (unsigned int i = 0; i < size; ++i) {			
			std::stringstream stream;
			stream << "Computer";
			texts_[index].setText(stream.str());
			stream.str("");
			stream << tetrisGame_.getNumberOfPlayers(i);
			texts_[index+1].setText(stream.str());
			index = index + 2;
		}

		glColor3d(1,1,1);

		int w,h;
		windowSize(w,h);
		glPushMatrix();
		glTranslated(x_, h - y_,0);
		
		glPushMatrix();
		//glTranslated(x_,y_,0);
		header1_.draw();
		glTranslated(130,0,0);
		header2_.draw();
		glPopMatrix();
		glTranslated(15,-20,0);
		
		for (unsigned int i = 0; i < size*2; ++i) {
			glPushMatrix();
			//std::cout << "\n" << texts_[i].getText() << " " << texts_[i+1].getText();
			glTranslated(0,-20*i,0);
			texts_[i].draw();
			glTranslated(200,0,0);
			texts_[++i].draw();
			glPopMatrix();			
		}
		glPopMatrix();
	}
	
	void eventUpdate(const SDL_Event& windowEvent) {
	}
private:
	mw::FontPtr font_;
	double x_,y_;

	TetrisGame& tetrisGame_;
	mw::Text header1_, header2_;
	std::vector<mw::Text> texts_;
};

enum Frame {FRAME_MENU,
			FRAME_LOCAL_GAME,
			FRAME_SERVER_CREATE,
			FRAME_SERVER_LOOBY,
			FRAME_SERVER_GAME,
			FRAME_CLIENT_CREATE,
			FRAME_CLIENT_LOOBY,
			FRAME_CLIENT_GAME};

class TetrisGameGuiItem : public gui::GuiItem<MenuEvent> {
public:
	TetrisGameGuiItem(TetrisGame& tetrisGame) : tetrisGame_(tetrisGame) {
	}

	void physicUpdate(Uint32 deltaTime) {
		tetrisGame_.physicUpdate(deltaTime);
	}
	
	void graphicUpdate(Uint32 deltaTime) {
		tetrisGame_.graphicUpdate(deltaTime);
	}

	void eventUpdate(const SDL_Event& windowEvent) {
		tetrisGame_.eventUpdate(windowEvent);
	}
private:
	TetrisGame& tetrisGame_;
};

class TetrisWindow : public mw::GameWindow {
public:
	typedef gui::TextButton<MenuEvent> TextButton;
	typedef gui::Background<MenuEvent> Background;
	typedef gui::TextField<MenuEvent> TextField;

    TetrisWindow() : mw::GameWindow(400,400,"MWetris","images/tetris.bmp"), gui_() {
        setResizable(true);
        setTimeStep(20);

		setUnicodeInputEnable(true);

		header_ = mw::Text("MWetris",fontDefault,80);
		
		numberOfPlayer_ = 1;
		
		tetrisGuiItem_ = new TetrisGameGuiItem(tetrisGame);
		initGui();
		reshapeWindowsOpenGL();

		tetrisGame.setOnConnectionEvent([&](Protocol::ManagerEvent connectionEvent) {
			handleConnectionEvent(connectionEvent);
		});
    }

	~TetrisWindow() {
		delete menuPlay_;
	}	
private:
	void initGui() {
		// Load fonts.
		mw::FontPtr font = mw::FontPtr(new mw::Font("fonts/DroidSansMono.ttf",18));

		// FRAME_1 Front page.
		// FRAME_2 Local game page.
		// FRAME_3 Server looby page.
		// FRAME_4 Server game page.
		// FRAME_5 Client looby page.
		// FRAME_6 Client game page.
		
		// Add backgrounds to all frames.
		Background* background = new Background(spriteBackground);
		gui_.add(FRAME_MENU,background );
		gui_.add(FRAME_LOCAL_GAME,background);
		gui_.add(FRAME_SERVER_CREATE,background);
		gui_.add(FRAME_SERVER_LOOBY,background);
		gui_.add(FRAME_SERVER_GAME,background);
		gui_.add(FRAME_CLIENT_CREATE,background);
		gui_.add(FRAME_CLIENT_LOOBY,background);
		gui_.add(FRAME_CLIENT_GAME,background);

		// Add bars to all frames.
		bar_ = new Bar(Bar::UP,20);
		gui_.add(FRAME_MENU,bar_);
		gui_.add(FRAME_LOCAL_GAME,new Bar(Bar::UP,20));	
		gui_.add(FRAME_SERVER_CREATE,new Bar(Bar::UP,20));
		gui_.add(FRAME_SERVER_LOOBY,new Bar(Bar::UP,20));
		gui_.add(FRAME_SERVER_GAME,new Bar(Bar::UP,20));
		gui_.add(FRAME_CLIENT_CREATE,new Bar(Bar::UP,20));
		gui_.add(FRAME_CLIENT_LOOBY,new Bar(Bar::UP,20));
		gui_.add(FRAME_CLIENT_GAME,new Bar(Bar::UP,20));

		// Init all frames.
		initFrameMenu();
		initFrameLocalGame();
		initFrameServerCreate();
		initFrameServerLooby();
		initFrameServerGame();
		initFrameClientCreate();
		initFrameClientLooby();
		initFrameClientGame();				

		// Add fullscreen support to all frames.
		MouseDoubleClick* doubleClick = new MouseDoubleClick(MENUEVENT_FULLSCREEN);
		gui_.add(FRAME_MENU,doubleClick);
		gui_.add(FRAME_LOCAL_GAME,doubleClick);
		gui_.add(FRAME_SERVER_LOOBY,doubleClick);
		gui_.add(FRAME_SERVER_GAME,doubleClick);

		gui_.setActiveFrame(FRAME_MENU);

		// Add tetrisgame.
		gui_.add(FRAME_LOCAL_GAME, tetrisGuiItem_);
		gui_.add(FRAME_CLIENT_LOOBY, tetrisGuiItem_);
		gui_.add(FRAME_CLIENT_CREATE, tetrisGuiItem_);
		gui_.add(FRAME_CLIENT_GAME, tetrisGuiItem_);
		gui_.add(FRAME_SERVER_LOOBY, tetrisGuiItem_);
		gui_.add(FRAME_SERVER_CREATE, tetrisGuiItem_);
		gui_.add(FRAME_SERVER_GAME, tetrisGuiItem_);
		
		// The drawing is made manually in this clas.
		tetrisGuiItem_->setVisible(false);
	}

	void initFrameMenu() {
		// Add resume button to the top bar in FRAME_1.
		TextButton* b0 = new TextButton(10,10,"[Resume]", 20, fontDefault, MENUEVENT_RESUMEGAME);
		b0->setReverseAxleY(true);
		gui_.add(FRAME_MENU,b0);

		// Add buttons to menu on front page (FRAME_1).
		menuPlay_ = new TextButton(10,150,"[Play]", 30, fontDefault, MENUEVENT_LOCALGAME);
		menuPlay_->setReverseAxleY(true);
		gui_.add(FRAME_MENU,menuPlay_);

		TextButton* tmp = new TextButton(10,200,"[Play as server]", 30, fontDefault, MENUEVENT_SERVERCREATE);
		tmp->setReverseAxleY(true);
		gui_.add(FRAME_MENU,tmp);

		tmp = new TextButton(10,250,"[Play as client]", 30, fontDefault, MENUEVENT_CLIENTCREATE);
		tmp->setReverseAxleY(true);
		gui_.add(FRAME_MENU,tmp);

		tmp = new TextButton(10,300,"[Exit]", 30, fontDefault, MENUEVENT_EXIT);
		tmp->setReverseAxleY(true);
		gui_.add(FRAME_MENU,tmp);
	}	

	void initFrameLocalGame() {
		// Add buttons to the top bar in FRAME_2 (local game mode).
		TextButton* b1 = new TextButton(10,10,"[Menu]", 20, fontDefault, MENUEVENT_MENU);
		b1->setReverseAxleY(true);
		TextButton* b2= new TextButton(80,10,"[Restart]", 20, fontDefault, MENUEVENT_RESTART);
		b2->setReverseAxleY(true);
		gui_.add(FRAME_LOCAL_GAME, b1);
		gui_.add(FRAME_LOCAL_GAME, b2);
		gui_.add(FRAME_LOCAL_GAME,nbrOfPlayersLocal_ = new ChooseNbrOfPlayers(20,175,0));
	}

	void initFrameServerCreate() {
		TextButton* b1 = new TextButton(10,10,"[Menu]", 20, fontDefault, MENUEVENT_MENU);
		b1->setReverseAxleY(true);
		gui_.add(FRAME_SERVER_CREATE,b1);

		gui::Text<MenuEvent>* t1 = new gui::Text<MenuEvent>(10,50,"Port:", 20, fontDefault);
		gui_.add(FRAME_SERVER_CREATE,t1);
		gui::InputFormatterPtr ptr = gui::InputFormatterPtr(new PortFormatter);
		serverPort_ = new TextField(80,70,54, 16, fontMonospaced18, MENUEVENT_NOTHING);
		serverPort_->setInputFormatter(ptr);
		gui_.add(FRAME_SERVER_CREATE,serverPort_);

		TextButton* b2 = new TextButton(80,10,"[Create]", 20, fontDefault, MENUEVENT_SERVERLOOBY);
		b2->setReverseAxleY(true);
		gui_.add(FRAME_SERVER_CREATE,b2);

		gui_.add(FRAME_SERVER_CREATE, nbrOfPlayersServer_ = new ChooseNbrOfPlayers(20,180,0));
	}

	void initFrameServerLooby() {
		// Add buttons to the top bar in FRAME_3 (server looby mode).		
		TextButton* b1 = new TextButton(10,10,"[Abort]", 20, fontDefault, MENUEVENT_MENU);
		b1->setReverseAxleY(true);
		gui_.add(FRAME_SERVER_LOOBY,b1);
		TextButton* b2 = new TextButton(80,10,"[Start Game]", 20, fontDefault, MENUEVENT_STARTGAME);
		b2->setReverseAxleY(true);
		gui_.add(FRAME_SERVER_LOOBY, b2);

		TextButton* b3 = new TextButton(10,220,"[Ready]", 20, fontDefault, MENUEVENT_READYTOSTART);
		b3->setReverseAxleY(true);
		gui_.add(FRAME_SERVER_LOOBY, b3);
		gui_.add(FRAME_SERVER_LOOBY, new NetworkLooby(10,50,tetrisGame,fontDefault));
	}

	void initFrameServerGame() {
		// Add buttons to the top bar in FRAME_4 (server game mode).
		TextButton* b1= new TextButton(10,10,"[Menu]", 20, fontDefault, MENUEVENT_MENU);
		b1->setReverseAxleY(true);
		TextButton* b2= new TextButton(80,10,"[Pause]", 20, fontDefault, MENUEVENT_PAUSE);
		b2->setReverseAxleY(true);
		gui_.add(FRAME_SERVER_GAME,b1);
	}

	void initFrameClientCreate() {
		TextButton* b1 = new TextButton(10,10,"[Menu]", 20, fontDefault, MENUEVENT_MENU);
		b1->setReverseAxleY(true);
		gui_.add(FRAME_CLIENT_CREATE, b1);
		
		gui::Text<MenuEvent>* t1 = new gui::Text<MenuEvent>(10,50,"Ip:", 20, fontDefault);
		gui_.add(FRAME_CLIENT_CREATE,t1);

		gui_.add(FRAME_CLIENT_CREATE,nbrOfPlayersClient_ = new ChooseNbrOfPlayers(20,190,0));
		gui::InputFormatterPtr ptr2(new InputIpFormatter);
		ipToServer_ = new TextField(56,70,100, 16, fontMonospaced18, MENUEVENT_NOTHING);
		ipToServer_->setInputFormatter(ptr2);
		gui_.add(FRAME_CLIENT_CREATE, ipToServer_);

		gui::Text<MenuEvent>* t2 = new gui::Text<MenuEvent>(10,80,"Port:", 20, fontDefault);
		gui_.add(FRAME_CLIENT_CREATE, t2);
		
		gui::InputFormatterPtr ptr = gui::InputFormatterPtr(new PortFormatter);
		connectToPort_ = new TextField(90,100,54, 16, fontMonospaced18, MENUEVENT_NOTHING);
		connectToPort_->setInputFormatter(ptr);
		gui_.add(FRAME_CLIENT_CREATE, connectToPort_);
		
		TextButton* b2 = new TextButton(80,10,"[Connect]", 20, fontDefault, MENUEVENT_CLIENTLOOBY);
		b2->setReverseAxleY(true);
		gui_.add(FRAME_CLIENT_CREATE, b2);
	}

	void initFrameClientLooby() {		
		// Add buttons to the top bar in FRAME_5 (client looby mode).
		TextButton* b1 = new TextButton(10,10,"[Menu]", 20, fontDefault, MENUEVENT_MENU);
		b1->setReverseAxleY(true);
		gui_.add(FRAME_CLIENT_LOOBY, b1);
		
		TextButton* b3 = new TextButton(10,220,"[Ready]", 20, fontDefault, MENUEVENT_READYTOSTART);
		b3->setReverseAxleY(true);
		gui_.add(FRAME_CLIENT_LOOBY, b3);
		gui_.add(FRAME_CLIENT_LOOBY, new NetworkLooby(10,50,tetrisGame,fontDefault));
	}

	void initFrameClientGame() {
		// Add buttons to the top bar in FRAME_6 (client game mode).
		TextButton* b1 = new TextButton(10,10,"[Menu]", 20, fontDefault, MENUEVENT_MENU);
		b1->setReverseAxleY(true);
		TextButton* b2= new TextButton(80,10,"[Pause]", 20, fontDefault, MENUEVENT_PAUSE);
		b2->setReverseAxleY(true);
		gui_.add(FRAME_CLIENT_GAME, b1);
	}

    // Override mw::GameWindow
    void graphicUpdate(Uint32 deltaTime) {
		gui_.graphicUpdate(deltaTime);

		int frame = gui_.getActiveFrame();
		if (frame == FRAME_MENU) {
			glPushMatrix();
			glColor3d(0.8,0,0);
			glTranslated(10,mw::Window::getHeight()-100,0);
			header_.draw();
			glPopMatrix();
		}
		if (frame == FRAME_LOCAL_GAME ||frame == FRAME_SERVER_GAME || frame == FRAME_CLIENT_GAME) {
			glEnable(GL_SCISSOR_TEST);
			glScissor(gameView_.x,gameView_.y,gameView_.width,gameView_.height);
			drawTetrisGame(deltaTime);
			glDisable(GL_SCISSOR_TEST);
		}
    }

    // Override mw::GameWindow
    void physicUpdate(Uint32 deltaTime) {
		gui_.physicUpdate(deltaTime);

		// Collect event
		MenuEvent menuEvent;
		while (gui_.pollEvent(menuEvent)) {
			executeMenuEvent(menuEvent);
		}
    }

    // Override mw::GameWindow
    void eventUpdate(const SDL_Event& windowEvent) {
		switch (windowEvent.type) {
		case SDL_QUIT:
			gui_.pushEvent(MENUEVENT_EXIT);
        case SDL_KEYDOWN:
            switch (windowEvent.key.keysym.sym) {
            case SDLK_t:
                mw::Window::setWindowsSize(100,100);
                //SDL_SetVideoMode(300,300, 32, SDL_OPENGL);
                //SDL_WM_ToggleFullScreen();
                break;
			case SDLK_F2:
				gui_.pushEvent(MENUEVENT_RESTART);
				break;
			case SDLK_p:
			case SDLK_PAUSE:
				gui_.pushEvent(MENUEVENT_PAUSE);
				break;
			case SDLK_ESCAPE:
				gui_.pushEvent(MENUEVENT_EXIT);
				break;
            default:
                break;
			}
			break;
        default:
            break;
		}

		gui_.eventUpdate(windowEvent);
    }

	void executeMenuEvent(MenuEvent menuEvent) {
		switch (menuEvent) {
		case MENUEVENT_RESUMEGAME:
			gui_.setActiveFrame(FRAME_LOCAL_GAME);
			break;
		case MENUEVENT_PAUSE:
			tetrisGame.pause();
			break;
		case MENUEVENT_LOCALGAME:
			gui_.setActiveFrame(FRAME_LOCAL_GAME);
			nbrOfPlayersLocal_->setNbrOfPlayers(1);
			tetrisGame.closeGame();			
			tetrisGame.createLocalGame(nbrOfPlayersLocal_->getNbrOfPlayers());
			tetrisGame.setReadyGame(true);
			tetrisGame.startGame();
			//executeMenuEvent(MENUEVENT_STARTGAME);
			//pressEnterToStart_->setActive(true);
			std::cout << "\n" << "MENUEVENT_LOCALGAME" << std::endl;
			break;
		case MENUEVENT_SERVERCREATE:
			gui_.setActiveFrame(FRAME_SERVER_CREATE);
			break;
		case MENUEVENT_SERVERLOOBY:
			gui_.setActiveFrame(FRAME_SERVER_LOOBY);
			tetrisGame.closeGame();
			{
				int port = atoi(serverPort_->getText().c_str());
				tetrisGame.createServerGame(nbrOfPlayersServer_->getNbrOfPlayers(),port);
			}
			break;
		case MENUEVENT_SERVERGAME:
			gui_.setActiveFrame(FRAME_SERVER_GAME);			
			std::cout << "\n" << "MENUEVENT_SERVERGAME" << std::endl;
			break;
		case MENUEVENT_CLIENTCREATE:
			gui_.setActiveFrame(FRAME_CLIENT_CREATE);
			break;
		case MENUEVENT_CLIENTLOOBY:
			gui_.setActiveFrame(FRAME_CLIENT_LOOBY);
			tetrisGame.closeGame();
			{
				int port = atoi(connectToPort_->getText().c_str());
				std::string ip = ipToServer_->getText();
				tetrisGame.createClientGame(nbrOfPlayersClient_->getNbrOfPlayers(),port,ip);
			}
			break;
		case MENUEVENT_CLIENTGAME:			
			gui_.setActiveFrame(FRAME_CLIENT_GAME);
			//tetrisGame.setReadyGame(true);
			//tetrisGame.startGame();

			//tetrisGame.createServerGame(2,5019);
			//pressEnterToStart_->setActive(true);
			std::cout << "\n" << "MENUEVENT_CLIENTGAME" << std::endl;
			break;		
		case MENUEVENT_CHANGENBROFPLAYERS: {
			ChooseNbrOfPlayers* nbrOfPlayers = nbrOfPlayersLocal_;
			if (gui_.getActiveFrame() == FRAME_LOCAL_GAME) {
				nbrOfPlayers = nbrOfPlayersLocal_;				
				nbrOfPlayers->setNbrOfPlayers(nbrOfPlayers->getNbrOfPlayers() + 1);
				int nbr = nbrOfPlayers->getNbrOfPlayers();
				if (nbr == 0) {
					nbrOfPlayers->setNbrOfPlayers(1);
					nbr = 1;
				}
				tetrisGame.closeGame();
				tetrisGame.createLocalGame(nbr);
				tetrisGame.setReadyGame(true);
				tetrisGame.startGame();

			} else if (gui_.getActiveFrame() == FRAME_SERVER_CREATE) {
				nbrOfPlayers = nbrOfPlayersServer_;
				nbrOfPlayers->setNbrOfPlayers(nbrOfPlayers->getNbrOfPlayers() + 1);
			} else if (gui_.getActiveFrame() == FRAME_CLIENT_CREATE) {
				nbrOfPlayers = nbrOfPlayersClient_;
				nbrOfPlayers->setNbrOfPlayers(nbrOfPlayers->getNbrOfPlayers() + 1);
			}
			break;
		} case MENUEVENT_READYTOSTART:
			tetrisGame.setReadyGame(!tetrisGame.isReadyGame());
			break;
		case MENUEVENT_STARTGAME:
			tetrisGame.startGame();
			break;
		case MENUEVENT_MENU:
			gui_.setActiveFrame(FRAME_MENU);
			break;
		case MENUEVENT_RESTART:
			tetrisGame.restartGame();
			break;
		case MENUEVENT_EXIT:
			if (gui_.getActiveFrame() != FRAME_MENU) {
				gui_.setActiveFrame(FRAME_MENU);
			} else {
				mw::Window::setQuiting(true);
			}
			break;
		case MENUEVENT_FULLSCREEN:
			//mw::Window::setFullScreen(!mw::Window::isFullScreen());
			std::cout << "\nMENUEVENT_FULLSCREEN";
			break;
		case MENUEVENT_NOTHING:
			std::cout << "\nMENUEVENT_NOTHING";
			break;
        default:
            break;
		};
	}

    // Override mw::GameWindow
    void resize(int width, int height) {
        reshapeWindowsOpenGL();
    }

    void reshapeWindowsOpenGL() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        int w = getWidth();
        int h = getHeight();

        glViewport(0,0,w,h);
        glOrtho(0,w,0,h,-1,1);
        std::cout << "\nreshapeWindowsOpenGL" << std::endl;

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void drawTetrisGame(Uint32 deltaTime) {
        glPushMatrix();
        int w = getWidth();
		int h = getHeight() - bar_->getSize();

        // Centers the game and holds the correct proportions. The sides is transparent.
        if (tetrisGame.getWidth() / w > tetrisGame.getHeight() / h) {
            // Black sides, up and down.
            double scale = w / tetrisGame.getWidth();
            glTranslated(0, (h - scale*tetrisGame.getHeight()) * 0.5, 0);
            glScaled(scale, scale, 1);
            gameView_.x = 0;
            gameView_.y = static_cast<int>((h - scale*tetrisGame.getHeight()) * 0.5);
            gameView_.width = w;
            gameView_.height = static_cast<int>(scale*tetrisGame.getHeight());
        } else {
            // Black sides, left and right.
            double scale = h / tetrisGame.getHeight();
            glTranslated((w-scale*tetrisGame.getWidth()) * 0.5, 0, 0);
            glScaled(scale,scale,1);
            gameView_.x = static_cast<int>((w-scale*tetrisGame.getWidth()) * 0.5);
            gameView_.y = 0;
            gameView_.width = static_cast<int>(scale*tetrisGame.getWidth());
            gameView_.height = h;
        }
        tetrisGame.graphicUpdate(deltaTime);
        glPopMatrix();
    }

	void handleConnectionEvent(Protocol::ManagerEvent connectionEvent) {
		if (connectionEvent == PlayerManager::ManagerEvent::STARTS_GAME) {
			PlayerManager::Status status = tetrisGame.getStatus();
			std::cout << "MENUEVENT"<<std::endl;
			switch (status) {
			case PlayerManager::Status::LOCAL:
				//gui_.pushEvent(MENUEVENT_LOCALGAME);
				std::cout << "MENUEVENT_LOCALGAMEDAS"<<std::endl;
				break;
			case PlayerManager::Status::SERVER:
				//gui_.pushEvent(MENUEVENT_SERVERGAME);
				std::cout << "MENUEVENT_SERVERGAME"<<std::endl;
				break;
			case PlayerManager::Status::CLIENT:
				gui_.pushEvent(MENUEVENT_CLIENTGAME);
				break;
			}
		}
	}

    TetrisGame tetrisGame;
	TetrisGameGuiItem* tetrisGuiItem_;

	gui::Gui<MenuEvent> gui_;
	TextButton* menuPlay_;	
	mw::Text header_;

	Bar* bar_;									// The bar at the top.	
	TextButton* restart_;
	ChooseNbrOfPlayers* nbrOfPlayersLocal_;
	ChooseNbrOfPlayers* nbrOfPlayersServer_;
	ChooseNbrOfPlayers* nbrOfPlayersClient_;
	//PressEnterToStart* pressEnterToStart_;
		
	TextField* serverPort_;
	TextField* ipToServer_;
	TextField* connectToPort_;

	int numberOfPlayer_;

    View gameView_;
};

#endif // TETRISWINDOW_H
