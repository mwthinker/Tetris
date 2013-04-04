#ifndef PLAYER_H
#define PLAYER_H

#include "tetrisboard.h"
#include "graphicboard.h"

#include "mw/window.h"
#include "mw/sprite.h"

#include <string>
#include <queue>

class Player {
public:
	friend class PlayerManager;
	friend class Protocol;

	Player(int id) : graphicBoard_(tetrisBoard_) {		
		init(id);
    }

	void restart() {
		bool random = tetrisBoard_.isDecideRandomBlockType();
		tetrisBoard_ = TetrisBoard();
		tetrisBoard_.setDecideRandomBlockType(random);
		init(id_);
	}

	virtual ~Player() {
	}

    void setNbrOfClearedRows(int nbr) {
        nbrOfClearedRows_ = nbr;
        graphicBoard_.setNbrOfClearedRows(nbr);
    }

	int getNbrOfClearedRows() const {
        return nbrOfClearedRows_;
    }

    void setPoints(int points) {
        points_ = points;
        graphicBoard_.setPoints(points);
    }

	int getPoints() const {
        return points_;
    }

    void setLevel(int level) {
        level_ = level;
        graphicBoard_.setLevel(level_);
    }

	int getLevel() const  {
        return level_;
    }

    void setName(std::string name) {
        name_ = name;
        graphicBoard_.setName(name);
    }

	// Returns the players name.
	std::string getName() const {
        return name_;
    }

	void setGameOverMessage(std::string message) {
        graphicBoard_.setGameOverMessage(message);
    }	

	// Draws the tetrisboard.
    void draw() {
        graphicBoard_.draw();
    }

	// Returns the draw width in pixels.
    double getWidth() const {
        return graphicBoard_.getWidth();
    }

	// Returns the draw height in pixels.
    double getHeight() const {
        return graphicBoard_.getHeight();
    }

	// Return true when a gameEvent is polled otherwise false.
    bool pollGameEvent(GameEvent& gameEvent) {
        return tetrisBoard_.pollGameEvent(gameEvent);
    }

	// Returns the players id.
	int getId() const {
		return id_;
	}	

	const TetrisBoard& getTetrisBoard() const {
		return tetrisBoard_;
	}

	void update(TetrisBoard::Move move) {
		tetrisBoard_.update(move);
    }

	void setLevelUpCounter(int value) {
		levelUpCounter_ = value;
	}

	int getLevelUpCounter() const {
		return levelUpCounter_;
	}
private:
	void init(int id) {
		level_ = 1;
        points_ = 0;
        nbrOfClearedRows_ = 0;        
		id_ = id;
		levelUpCounter_ = 0;		
		graphicBoard_.setPoints(points_);
		graphicBoard_.setLevel(level_);
		graphicBoard_.setNbrOfClearedRows(nbrOfClearedRows_);
		graphicBoard_.setGameOverMessage("Game over!");
	}

    TetrisBoard tetrisBoard_;
    GraphicBoard graphicBoard_;
    
	int level_;			   // Current level.
	int points_;		   // Number of points.
	int nbrOfClearedRows_; // Number of rows the player cleared.
	std::string name_;     // The name of the player.
	int id_;
	int levelUpCounter_;	// Is used to determen when to level up.	
};

#endif // PLAYER_H
