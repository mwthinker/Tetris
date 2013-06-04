#include "tetrisgame.h"
#include "gamesound.h"
#include "graphicboard.h"
#include "player.h"
#include "localplayer.h"
#include "device.h"
#include "devicekeyboard.h"
#include "devicejoystick.h"

#include <mw/sound.h>

#include <iostream>
#include <utility>
#include <memory>
#include <sstream>

TetrisGame::TetrisGame() {
    nbrOfAlivePlayers_ = 0;
    timeStep_ = 0.017; // Fix time step for physics update.
    accumulator_ = 0.0; // Time accumulator.
    columns_ = 10;
    rows_ = 20;
    maxLevel_ = 20;
}

TetrisGame::~TetrisGame() {
}

void TetrisGame::updateGame(double deltaTime) {
    // DeltaTime to big?
    if (deltaTime > 0.250) {
        // To avoid spiral of death.
        deltaTime = 0.250;
    }

    accumulator_ += deltaTime;
    while (accumulator_ >= timeStep_) {
        accumulator_ -= timeStep_;
        iterateAllPlayersInfo([&](PlayerInfoPtr player) {
            updatePlayer(player, timeStep_);
            return true;
        });
    }
}

void TetrisGame::updatePlayer(PlayerInfoPtr player, double deltaTime) {
    player->update(deltaTime);

    GameEvent gameEvent;
    while (player->pollGameEvent(gameEvent)) {
        applyRules(player, gameEvent);
        soundEffects(gameEvent);
    }
}

void TetrisGame::initGame(int columns, int rows, int maxLevel, bool local) {
	nbrOfAlivePlayers_ = getNbrOfPlayers();
	int nbr = nbrOfAlivePlayers_;
	iterateAllPlayersInfo([&](PlayerInfoPtr player) {
		std::stringstream stream;
		stream << "Player " << nbrOfAlivePlayers_ - nbr + 1;
		player->setName(stream.str());
		--nbr;
        return true;
    });

	columns_ = columns;
	rows_ = rows;
	maxLevel_ = maxLevel;
}

void TetrisGame::draw() {
	int nbr = getNbrOfPlayers();
    int i = 0;
    iterateAllPlayersInfo([&](PlayerInfoPtr player) {
        ++i;
        glPushMatrix();
		glTranslated(player->getWidth() * (nbr-i), 0, 0);
        player->draw();
        glPopMatrix();
        return true;
    });
}

double TetrisGame::getWidth() const {
	double width = 400;
    iterateAllPlayersInfo([&](PlayerInfoPtr player) {
        width = player->getWidth() * getNbrOfPlayers();
        return false;
    });
    return width;
}

double TetrisGame::getHeight() const {
	double height = 400;
    iterateAllPlayersInfo([&](PlayerInfoPtr player) {
        height = player->getHeight();
        return false;
    });

	return height;
}

void TetrisGame::soundEffects(GameEvent gameEvent) {
	mw::Sound sound;
	switch (gameEvent) {
	case GRAVITY_MOVES_BLOCK:
		break;
	case BLOCK_COLLISION:
		sound = soundBlockCollision;
		break;
	case PLAYER_ROTATES_BLOCK:
		break;
	case ONE_ROW_REMOVED:
		// Fall through
	case TWO_ROW_REMOVED:
		// Fall through
	case THREE_ROW_REMOVED:
		sound = soundRowRemoved;
		break;
	case FOUR_ROW_REMOVED:
		sound = soundTetris;
		break;
	case GAME_OVER:
		break;
	default:
		break;
	}
	sound.play();
}

void TetrisGame::applyRules(PlayerInfoPtr player, GameEvent gameEvent) {
	// Warning a slight risk of being out of sync in multiplayer.
	// However only effecting points and level and in very subtle ways.
	// Nothing other than graphics is effected.
	int rows = 0;
	switch (gameEvent) {
	case ONE_ROW_REMOVED:
		rows = 1;
		break;
	case TWO_ROW_REMOVED:
		rows = 2;
		break;
	case THREE_ROW_REMOVED:
		rows = 3;
		break;
	case FOUR_ROW_REMOVED:
		{
			rows = 4;
			int nbrOfPlayers = getNbrOfPlayers();
			// Multiplayer?
			if (nbrOfPlayers > 1) {
				int nbrOfRowsToAdd = 4;
				if (nbrOfPlayers == 2) {
					nbrOfRowsToAdd = 4;
				} else if (nbrOfPlayers == 3) {
					nbrOfRowsToAdd = 2;
				} else {
					nbrOfRowsToAdd = 1;
				}

				addRowsToAllPlayersExcept(player,nbrOfRowsToAdd);
			}
			break;
		}
	case GAME_OVER:
		// Multiplayer?
		if (getNbrOfPlayers() > 1) {
			std::stringstream stream;
			stream << nbrOfAlivePlayers_ ;
			if (nbrOfAlivePlayers_ == 1) {
				stream << ":st place!";
			} else if (nbrOfAlivePlayers_ == 2) {
				stream << ":nd place!";
			} else if (nbrOfAlivePlayers_ == 3) {
				stream << ":rd place!";
			} else {
				stream << ":th place!";
			}

			player->setGameOverMessage(stream.str());

			// One player more is dead.
			--nbrOfAlivePlayers_;

			// All dead except one => End game!
			if (nbrOfAlivePlayers_ == 1) {
                iterateAllPlayersInfo([](PlayerInfoPtr tmpPlayer) {
                    // Will be noticed in the next call to PlayerManager::applyRules(...).
					// Triggers only for not dead players.
					tmpPlayer->triggerGameOverEvent();
                    return true;
                });
			}
		} else {
			// Singleplayer.
			player->setGameOverMessage("Game over!");
			// Is local game? And is the correct settings?
			if (local_ && rows_ == 20 && columns_ == 10 && maxLevel_ == 20) {
                signalEvent(std::make_shared<GameOver>(player->getPoints()));
			}
		}
		break;
	default:
		break;
	}

	if (rows != 0) {
		// Assign points and number of cleared rows.
		player->setNbrOfClearedRows(player->getNbrOfClearedRows() + rows);
		player->setPoints(player->getPoints() + player->getLevel()*rows*rows);

		int nbrOfRowsToLevelUp = 10;

		// Increase level up counter.
		// If assuming all players are equally good. Players should level up with the
		// the same time in both single- and multiplayer game.
		if (getNbrOfPlayers() > 1) {
			// Multiplayer

			// Increase level up counter for all opponents to the current player.
			iterateAllPlayersInfo([&](PlayerInfoPtr opponent) {
                if (opponent != player) {
					// Compensates for more opponents which are also increasing counter
					// Compared to singleplayer.
					int add = rows*(getNbrOfPlayers() - nbrOfAlivePlayers_ + 1);
					opponent->setLevelUpCounter(opponent->getLevelUpCounter() + add);
				}
                return true;
            });
		} else {
			// Singleplayer.
			player->setLevelUpCounter(player->getLevelUpCounter() + rows);
		}

		// Set level to this player. Only when this players cleares a row.
		int maxLevel = player->getMaxLevel();
		if (getNbrOfPlayers() > 1) {
			// Multiplayer
			// Higher counter bar to level up due to more players that contribute to
			// increase counter.
			int level = player->getLevelUpCounter() / (nbrOfRowsToLevelUp*(getNbrOfPlayers()-1)) + 1;
			if (level <= maxLevel) {
				player->setLevel(level);
			}
		} else {
			int level = (player->getLevelUpCounter() / nbrOfRowsToLevelUp) + 1;
			if (level <= maxLevel) {
				player->setLevel(level);
			}
		}
	}
}
