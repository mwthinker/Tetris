#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "tetrisboard.h"
#include "graphicboard.h"

#include <string>
#include <memory>

class PlayerInfo;
typedef std::shared_ptr<PlayerInfo> PlayerInfoPtr;

class PlayerInfo {
public:
	PlayerInfo(int id, int width, int height);
    virtual ~PlayerInfo();

	// Is supposed to call pushMove with the appropriate move.
	// This will indirectly update the tetrisboard.
	virtual void update(double deltaTime) = 0;

	// Set the number of cleared rows.
	// May not sync perfectly over network.
    void setNbrOfClearedRows(int nbr);

	// Get the number of cleared rows.
	int getNbrOfClearedRows() const;

	// Set the number of points.
	// May not sync perfectly over network.
    void setPoints(int points);

	// Get the number of points.
	int getPoints() const;

	// Set the level.
	// May not sync perfectly over network.
    void setLevel(int level);

	// Get the level.
	int getLevel() const;

	// Set the player name.
    void setName(std::string name);

	// Returns the players name.
	std::string getName() const;

	void setGameOverMessage(std::string message);
	
	void setCountDownMessage(std::string message);

	// Draws the tetrisboard.
    void draw();

	// Returns the draw width in pixels.
    double getWidth() const;

	// Returns the draw height in pixels.
    double getHeight() const;

	// Return true when a gameEvent is polled otherwise false.
    bool pollGameEvent(GameEvent& gameEvent);

	// Returns the players id.
	int getId() const;

	// Returns the tetrisboard.
	const TetrisBoard& getTetrisBoard() const;

	// Set the number for which the player should level up.
	// May not sync perfectly over network.
	void setLevelUpCounter(int value);

	// Get the level up value.
	int getLevelUpCounter() const;

	// Trigger the player to be game over.
	// May not sync perfectly over network, and can cause
	// the tetrisboard logic to behave out of sync.
	void triggerGameOverEvent();

	// Get the current block type for the board.
	BlockType getCurrentBlock() const;

	// Get the next block type for the board.
	BlockType getNextBlock() const;	

protected:
	TetrisBoard tetrisBoard_;

	// Resets all values.
	void reset();

	inline void setAi(bool ai) {
		ai_ = ai;
	}

private:
	virtual void polledGameEvent(GameEvent gameEvent) {
	}

    GraphicBoard graphicBoard_;

	int level_;			   // Current level.
	int points_;		   // Number of points.
	int nbrOfClearedRows_; // Number of rows the player cleared.
	std::string name_;     // The name of the player.
	int levelUpCounter_;   // Is used to determine when to level up.
	bool ai_;

	const int id_;
};

#endif // PLAYERINFO_H
