#include "tetrisgame.h"
#include "gamesound.h"
#include "graphicboard.h"
#include "playerinfo.h"
#include "device.h"
#include "tetrisparameters.h"

#include <mw/sound.h>

#include <string>
#include <sstream>

TetrisRules::TetrisRules() {
	nbrOfAlivePlayers_ = 0;
	maxLevel_ = 20;
}

void TetrisRules::applyRules() {
	for (PlayerData& playerData : players_) {
		GameEvent gameEvent;
		while (playerData.player_->pollGameEvent(gameEvent)) {
			applyRules(playerData, gameEvent);
		}
	}
}

void TetrisRules::initGame(std::vector<PlayerInfoPtr> players, int columns, int rows, int maxLevel, bool local) {
	players_.clear();

	int nbr = 0;
	for (PlayerInfoPtr player : players) {
		std::stringstream stream;
		stream << "Player " << ++nbr;
		players_.push_back(player);
		//players_.back().graphic_.update(0, 0, 1, stream.str());
	}

	nbrOfAlivePlayers_ = players_.size();
	maxLevel_ = maxLevel;
	local_ = local;
}

TetrisRules::PlayerData::PlayerData(const PlayerInfoPtr& player) : player_(player) {
	reset();
}

void TetrisRules::PlayerData::reset() {
	level_ = 1;
	points_ = 0;
	nbrOfClearedRows_ = 0;
	levelUpCounter_ = 0;
}

void TetrisRules::applyRules(PlayerData& playerData, GameEvent gameEvent) {
	// Warning a slight risk of being out of sync in multiplayer.
	// However only effecting points and level and in very subtle ways.
	// Nothing other than graphics is effected.
	int rows = 0;
	switch (gameEvent) {
		case GameEvent::ONE_ROW_REMOVED:
			rows = 1;
			break;
		case GameEvent::TWO_ROW_REMOVED:
			rows = 2;
			break;
		case GameEvent::THREE_ROW_REMOVED:
			rows = 3;
			break;
		case GameEvent::FOUR_ROW_REMOVED:
			rows = 4;
			// Multiplayer?
			if (players_.size() > 1) {
				// Add two rows to all opponents.
				// addRowsToAllPlayersExcept(playerData.player_, 2);
			}
			break;
		case GameEvent::GAME_OVER:
			// Multiplayer?
			if (players_.size() > 1) {
				std::stringstream stream;
				stream << nbrOfAlivePlayers_;
				if (nbrOfAlivePlayers_ == 1) {
					stream << ":st place!";
				} else if (nbrOfAlivePlayers_ == 2) {
					stream << ":nd place!";
				} else if (nbrOfAlivePlayers_ == 3) {
					stream << ":rd place!";
				} else {
					stream << ":th place!";
				}

				//playerData.graphic_.setMiddleMessage(stream.str());

				// One player more is dead.
				--nbrOfAlivePlayers_;

				// All dead except one => End game!
				if (nbrOfAlivePlayers_ == 1) {
					for (PlayerData& tmp : players_) {
						// Will be noticed in the next call to PlayerManager::applyRules(...).
						// Triggers only for not dead players.
						tmp.player_->triggerGameOverEvent();
					}
				}
			} else { // Singleplayer.
				//playerData.graphic_.setMiddleMessage("Game over!");
				//And is the correct settings?
				if (playerData.player_->getTetrisBoard().getNbrOfRows() == TETRIS_HEIGHT
					&& playerData.player_->getTetrisBoard().getNbrOfColumns() == TETRIS_WIDTH
					&& maxLevel_ == TETRIS_MAX_LEVEL) {

					auto local = std::dynamic_pointer_cast<LocalPlayer>(playerData.player_);
					// Is local and a human player?
					if (local && !local->getDevice()->isAi()) {
						//signalEvent(std::make_shared<GameOver>(playerData.points_));
					}
				}
			}
			break;
		default:
			break;
	}

	if (rows != 0) {
		// Assign points and number of cleared rows.
		playerData.nbrOfClearedRows_ += rows;
		playerData.points_ += playerData.level_*rows*rows;
		//playerData.graphic_.update(playerData.nbrOfClearedRows_, playerData.points_, playerData.level_, playerData.name_);		

		// Multiplayer?
		if (players_.size() > 1) {
			// Increase level up counter for all opponents to the current player.
			for (PlayerData& opponentData : players_) {
				if (opponentData.player_ != playerData.player_) {
					opponentData.levelUpCounter_ += rows;
				}
			}
		} else { // Singleplayer!
			playerData.levelUpCounter_ += rows;
		}

		// Set level.
		int level = (playerData.levelUpCounter_ / ROWS_TO_LEVEL_UP) + 1;
		if (level <= maxLevel_) {
			playerData.level_ = level;
			//playerData.graphic_.update(playerData.nbrOfClearedRows_, playerData.points_, playerData.level_, playerData.name_);
		}
	}
}
