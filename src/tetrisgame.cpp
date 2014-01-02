#include "tetrisgame.h"
#include "gamesound.h"
#include "graphicboard.h"
#include "playerinfo.h"
#include "device.h"
#include "tetrisparameters.h"

#include <mw/sound.h>

#include <string>
#include <sstream>

TetrisGame::TetrisGame() {
	nbrOfAlivePlayers_ = 0;
	timeStep_ = 17; // Fix time step.
	accumulator_ = 0; // Time accumulator.
	maxLevel_ = 20;
}

void TetrisGame::updateGame(Uint32 deltaTime) {
	// DeltaTime to big?
	if (deltaTime > 250) {
		// To avoid spiral of death.
		deltaTime = 250;
	}

	accumulator_ += deltaTime;
	while (accumulator_ >= timeStep_) {
		accumulator_ -= timeStep_;
		if (countDown_ >= timeStep_) {
			countDown_ -= timeStep_;
			std::stringstream stream;
			stream << "Start in: " << (countDown_ + 1000) / 1000;
			for (PlayerData& playerData : players_) {
				playerData.graphic_.setMiddleMessage(stream.str());
			}
		} else {
			for (PlayerData& playerData : players_) {
				// Not gameover?
				if (!playerData.player_->getTetrisBoard().isGameOver()) {
					playerData.graphic_.setMiddleMessage("");
				}
				updatePlayer(playerData, timeStep_);
			}
		}
	}
}

void TetrisGame::updatePlayer(PlayerData& playerData, Uint32 deltaTime) {
	playerData.player_->update(deltaTime / 1000.0, playerData.level_);

	GameEvent gameEvent;
	while (playerData.player_->pollGameEvent(gameEvent)) {
		applyRules(playerData, gameEvent);
		soundEffects(gameEvent);
	}
}

void TetrisGame::initGame(int columns, int rows, int maxLevel, bool local) {
	players_.clear();

	int nbr = 0;
	iterateAllPlayersInfo([&](PlayerInfoPtr player) {
		std::stringstream stream;
		stream << "Player " << ++nbr;
		players_.push_back(player);
		players_.back().graphic_.setName(stream.str());
		return true;
	});

	nbrOfAlivePlayers_ = players_.size();
	maxLevel_ = maxLevel;
	local_ = local;
	countDown_ = 3000; // 3 seconds.
}

void TetrisGame::draw() {
	int nbr = players_.size();
	int i = 0;
	for (PlayerData& pair : players_) {
		++i;
		glPushMatrix();
		glTranslated(pair.graphic_.getWidth() * (nbr - i), 0, 0);
		pair.graphic_.draw(pair.player_->getTetrisBoard());
		glPopMatrix();
	}
}

double TetrisGame::getWidth() const {
	if (players_.size() > 0) {
		return players_.front().graphic_.getWidth() * players_.size();
	}
	return 400;
}

double TetrisGame::getHeight() const {
	if (players_.size() > 0) {
		return players_.front().graphic_.getHeight();
	}
	return 400;
}

TetrisGame::PlayerData::PlayerData(const PlayerInfoPtr& player) : player_(player) {
	reset();
}

void TetrisGame::PlayerData::reset() {
	level_ = 1;
	points_ = 0;
	nbrOfClearedRows_ = 0;
	levelUpCounter_ = 0;
}

void TetrisGame::soundEffects(GameEvent gameEvent) {
	mw::Sound sound;
	switch (gameEvent) {
		case GameEvent::GRAVITY_MOVES_BLOCK:
			break;
		case GameEvent::BLOCK_COLLISION:
			sound = soundBlockCollision;
			break;
		case GameEvent::PLAYER_ROTATES_BLOCK:
			break;
		case GameEvent::ONE_ROW_REMOVED:
			// Fall through
		case GameEvent::TWO_ROW_REMOVED:
			// Fall through
		case GameEvent::THREE_ROW_REMOVED:
			sound = soundRowRemoved;
			break;
		case GameEvent::FOUR_ROW_REMOVED:
			sound = soundTetris;
			break;
		case GameEvent::GAME_OVER:
			break;
		default:
			break;
	}
	sound.play();
}

void TetrisGame::applyRules(PlayerData& playerData, GameEvent gameEvent) {
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
			if (getNbrOfPlayers() > 1) {
				// Add two rows to all opponents.
				addRowsToAllPlayersExcept(playerData.player_, 2);
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

				playerData.graphic_.setMiddleMessage(stream.str());

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
				playerData.graphic_.setMiddleMessage("Game over!");
				//And is the correct settings?
				if (playerData.player_->getTetrisBoard().getNbrOfRows() == TETRIS_HEIGHT
					&& playerData.player_->getTetrisBoard().getNbrOfColumns() == TETRIS_WIDTH
					&& maxLevel_ == TETRIS_MAX_LEVEL) {

					auto local = std::dynamic_pointer_cast<LocalPlayer>(playerData.player_);
					// Is local and a human player?
					if (local && !local->getDevice()->isAi()) {
						signalEvent(std::make_shared<GameOver>(playerData.points_));
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
		playerData.graphic_.setPoints(playerData.points_);
		playerData.graphic_.setNbrOfClearedRows(playerData.nbrOfClearedRows_);

		// Multiplayer?
		if (getNbrOfPlayers() > 1) {
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
		if (level <= getMaxLevel()) {
			playerData.level_ = level;
			playerData.graphic_.setLevel(level);
		}
	}
}
