#include "tetrisrules.h"
#include "tetrisgame.h"
#include "tetrisparameters.h"
#include "localplayer.h"

#include <string>

TetrisRules::TetrisRules() {
	nbrOfAlivePlayers_ = 0;
	maxLevel_ = 20;
}

void TetrisRules::initGame(std::vector<PlayerPtr> players, int columns, int rows, int maxLevel, bool local) {
	int nbr = 0;
	players_.clear();
	nbrOfAlivePlayers_ = players.size();
	maxLevel_ = maxLevel;
	local_ = local;

	for (auto& p : players) {
		p->setLevel(1);
		players_[p->getId()] = PlayerInfo();
	}
}

void TetrisRules::applyRules(PlayerPtr player, GameEvent gameEvent, TetrisGame* game) {
	PlayerInfo pInfo = players_[player->getId()];

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
			if (game->getNbrOfPlayers() > 1) {
				// Add two rows to all opponents.
				game->addRowsToAllPlayersExcept(player, 2);
			}
			break;
		case GameEvent::GAME_OVER:
			// Multiplayer?
			if (game->getNbrOfPlayers() > 1) {
				// One player more is dead.
				--nbrOfAlivePlayers_;

				// All dead except one => End game!
				if (nbrOfAlivePlayers_ == 1) {
					for (auto& tmp : players_) {
						// Will be noticed in the next call to PlayerManager::applyRules(...).
						// Triggers only for not dead players.
						
						//tmp.second.player_->triggerGameOverEvent();
					}
				}
			} else { // Singleplayer.
				//And is the correct settings?
				if (player->getTetrisBoard().getNbrOfRows() == TETRIS_HEIGHT
					&& player->getTetrisBoard().getNbrOfColumns() == TETRIS_WIDTH
					&& maxLevel_ == TETRIS_MAX_LEVEL) {

					auto local = std::dynamic_pointer_cast<LocalPlayer>(player);
					// Is local and a human player?
					if (local && !local->getDevice()->isAi()) {
						game->signalEvent(std::make_shared<GameOver>(pInfo.points_));
					}
				}
			}
			break;
		default:
			break;
	}

	if (rows != 0) {
		// Assign points and number of cleared rows.
		pInfo.nbrClearedRows_ += rows;
		pInfo.points_ += player->getLevel() * rows * rows;

		// Multiplayer?
		if (players_.size() > 1) {
			// Increase level up counter for all opponents to the current player.
			for (auto& opponent : players_) {
				if (opponent.second.player_->getId() != player->getId()) {
					opponent.second.levelUpCounter_ += rows;
				}
			}
		} else { // Singleplayer!
			pInfo.levelUpCounter_ += rows;
		}

		// Set level.
		int level = (pInfo.levelUpCounter_ / ROWS_TO_LEVEL_UP) + 1;
		if (level <= maxLevel_) {
			player->setLevel(level);
		}
	}
}
