#ifndef PLAYER_H
#define PLAYER_H

#include "tetrisboard.h"
#include "graphicboard.h"

#include "mw/window.h"
#include "mw/sprite.h"

#include <string>
#include <queue>
#include <memory>

class PlayerInfo {
public:
	PlayerInfo(int id) : graphicBoard_(tetrisBoard_), id_(id) {
		reset();
    }

    virtual ~PlayerInfo() {
	}

	// Is supposed to call pushMove with the appropriate move.
	// This will indirectly update the tetrisboard.
	virtual void update(double deltaTime) = 0;

	// Set the number of cleared rows.
	// May not sync perfectly over network.
    void setNbrOfClearedRows(int nbr) {
        nbrOfClearedRows_ = nbr;
        graphicBoard_.setNbrOfClearedRows(nbr);
    }

	// Get the number of cleared rows.
	int getNbrOfClearedRows() const {
        return nbrOfClearedRows_;
    }

	// Set the number of points.
	// May not sync perfectly over network.
    void setPoints(int points) {
        points_ = points;
        graphicBoard_.setPoints(points);
    }

	// Get the number of points.
	int getPoints() const {
        return points_;
    }

	// Set the level.
	// May not sync perfectly over network.
    void setLevel(int level) {
        level_ = level;
        graphicBoard_.setLevel(level_);
    }

	// Get the level.
	int getLevel() const  {
        return level_;
    }

	// Set the player name.
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

	// Returns the tetrisboard.
	const TetrisBoard& getTetrisBoard() const {
		return tetrisBoard_;
	}

	// Set the number for which the player should level up.
	// May not sync perfectly over network.
	void setLevelUpCounter(int value) {
		levelUpCounter_ = value;
	}

	// Get the level up value.
	int getLevelUpCounter() const {
		return levelUpCounter_;
	}

	// Trigger the player to be game over.
	// May not sync perfectly over network, and can cause
	// the tetrisboard logic to behave out of sync.
	void triggerGameOverEvent() {
		tetrisBoard_.triggerGameOverEvent();
	}

	// Get the current block type for the board.
	BlockType getCurrentBlock() const {
		return tetrisBoard_.currentBlock().blockType();
	}

	// Get the next block type for the board.
	BlockType getNextBlock() const {
		return tetrisBoard_.nextBlock().blockType();
	}

protected:
	TetrisBoard tetrisBoard_;

	// Resets all values.
	void reset() {
		level_ = 1;
        points_ = 0;
        nbrOfClearedRows_ = 0;
		levelUpCounter_ = 0;
		graphicBoard_.setPoints(points_);
		graphicBoard_.setLevel(level_);
		graphicBoard_.setNbrOfClearedRows(nbrOfClearedRows_);
		graphicBoard_.setGameOverMessage("Game over!");
	}

private:
    GraphicBoard graphicBoard_;

	int level_;			   // Current level.
	int points_;		   // Number of points.
	int nbrOfClearedRows_; // Number of rows the player cleared.
	std::string name_;     // The name of the player.
	int levelUpCounter_;   // Is used to determine when to level up.

	const int id_;
};

typedef std::shared_ptr<PlayerInfo> PlayerInfoPtr;

class Player : public PlayerInfo {
public:
	Player(int id, bool remote) : PlayerInfo(id) {
		tetrisBoard_.setDecideRandomBlockType(!remote);
    }

    virtual ~Player() {
	}

	// Restart the player.
	void restart() {
		moves_ = std::queue<TetrisBoard::Move>();
		bool random = tetrisBoard_.isDecideRandomBlockType();
		tetrisBoard_ = TetrisBoard();
		tetrisBoard_.setDecideRandomBlockType(random);
		reset();
	}

	bool updateBoard(TetrisBoard::Move& move, BlockType& next) {
		TetrisBoard::Move polledMove;
		bool polled = pollMove(polledMove);
		if (polled) {
			next = tetrisBoard_.nextBlock().blockType();
			move = polledMove;
			update(move);
		}
		return polled;
	}

	void update(TetrisBoard::Move move) {
		tetrisBoard_.update(move);
    }

	void update(BlockType current, BlockType next) {
		tetrisBoard_.setNonRandomCurrentBlockType(current);
		tetrisBoard_.setNonRandomNextBlockType(next);
	}

	void update(TetrisBoard::Move move, BlockType next) {
		tetrisBoard_.setNonRandomNextBlockType(next);
		tetrisBoard_.update(move);
    }

	void update(const std::vector<BlockType>& blockTypes) {
		tetrisBoard_.addRows(blockTypes);
	}

protected:
    // Pushed to a queue.
    void pushMove(TetrisBoard::Move move) {
        moves_.push(move);
    }

private:
	// Remove from the queue.
    bool pollMove(TetrisBoard::Move& move) {
        if (moves_.empty()) {
            return false;
        }

        move = moves_.front();
        moves_.pop();
        return true;
    }
	
	std::queue<TetrisBoard::Move> moves_;
};

typedef std::shared_ptr<Player> PlayerPtr;

#endif // PLAYER_H
