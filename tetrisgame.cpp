#include "tetrisgame.h"
#include "gamesound.h"
#include "graphicboard.h"
#include "player.h"
#include "human.h"
#include "device.h"
#include "devicekeyboard.h"
#include "devicejoystick.h"

#include <mw/sound.h>

#include <iostream>
#include <utility>
#include <memory>
#include <sstream>

TetrisGame::TetrisGame() {
}

TetrisGame::~TetrisGame() {
}

void TetrisGame::setInputDevice(const DevicePtr& device, int playerIndex) {
	devices_[playerIndex] = device;
}

void TetrisGame::updateGame(Uint32 deltaTime) {
	for (auto it = humans_.begin(); it != humans_.end(); ++it) {
		HumanPtr& human = it->first;
		unsigned int index = it->second;
		Player* player = players_[index];
		int level = player->getLevel();

		if (!isPaused()) {
			Input input;
			if (index < devices_.size()) {
				DevicePtr& device = devices_[index];
				input = device->currentInput();
			}
			human->update(input, deltaTime/1000.0,level);
		}

		TetrisBoard::Move move;
		while (human->pollMove(move)) {
			sendInput(player->getId(),move,player->getTetrisBoard().nextBlock().blockType());
		}
	}

	auto& players = players_;
	// Iterate through all players and progress the game and apply game rules and sound effects.
	for (Player* player : players) {
		GameEvent gameEvent;
		while (player->pollGameEvent(gameEvent)) {
			applyRules(player, gameEvent);
			soundEffects(gameEvent);
		}
	}
}

void TetrisGame::draw() {
	glPushMatrix();

	auto& players = players_;
	for (auto it = players.begin(); it != players.end(); ++it) {
		Player* player = *it;
		player->draw();
		glTranslated(player->getWidth(),0,0);
	}

	glPopMatrix();
}

double TetrisGame::getWidth() const {
	double width = 0;
	const auto& players = players_;

	for (Player* player : players) {
		width += player->getWidth();
	}
	return width;
}

double TetrisGame::getHeight() const {
	auto& players = players_;
	if (players.size() == 0) {
		return 400;
	}
	return players.front()->getHeight();
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

void TetrisGame::applyRules(Player* player, GameEvent gameEvent) {
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

			// More than one player->
			if (players_.size() > 1) {
				for (auto it = humans_.begin(); it != humans_.end(); ++it) {
					// Human player => sendTetrisInfo() to all players.
					int nbrOfPlayers = players_.size();
					int nbrOfRowsToAdd = 4;
					if (nbrOfPlayers == 2) {
						nbrOfRowsToAdd = 4;
					} else if (nbrOfPlayers == 3) {
						nbrOfRowsToAdd = 2;
					} else {
						nbrOfRowsToAdd = 1;
					}

					// Send rows to all humans player except to the player who
					// triggered the event.
					Player* tmpPlayer = players_[it->second];
					if (player != tmpPlayer) {
						std::vector<BlockType> blockTypes;
						for (int i = 0; i < nbrOfRowsToAdd; ++i) {
							std::vector<BlockType> tmp = tmpPlayer->getTetrisBoard().generateRow();
							blockTypes.insert(blockTypes.begin(),tmp.begin(),tmp.end());
						}
						sendTetrisInfo(tmpPlayer->getId(), blockTypes);
					}
				}
			}
			break;
		}
	case GAME_OVER:
		// Multiplayer?
		if (players_.size() > 1) {
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
				for (Player* tmp : players_) {
					// Will be noticed in the next call to PlayerManager::applyRules(...).
					// Triggers only for not dead players.
					tmp->tetrisBoard_.triggerGameOverEvent();
				}
			}
		} else {
			// Singleplayer.
			player->setGameOverMessage("Game over!");
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
		if (players_.size() > 1) {
			// Multiplayer

			// Increase level up counter for all opponents to the current player.
			for (Player* opponent : players_) {
				if (opponent != player) {
					// Compensates for more opponents which are also increasing counter
					// Compared to singleplayer.
					int add = rows*(players_.size() - nbrOfAlivePlayers_ + 1);
					opponent->setLevelUpCounter(opponent->getLevelUpCounter() + add);
				}
			}
		} else {
			// Singleplayer.
			player->setLevelUpCounter(player->getLevelUpCounter() + rows);
		}

		// Set level to this player. Only when this players cleares a row.
		int maxLevel = 20;
		if (players_.size() > 1) {
			// Multiplayer
			// Higher counter bar to level up due to more players that contribute to
			// increase counter.
			int level = player->getLevelUpCounter() / (nbrOfRowsToLevelUp*(players_.size()-1)) + 1;
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
