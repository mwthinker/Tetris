#ifndef CONSOLETETRIS_H
#define CONSOLETETRIS_H

#include "tetrisparameters.h"
#include "tetrisentry.h"
#include "tetrisgameevent.h"
#include "device.h"
#include "tetrisgame.h"
#include "tetrisboard.h"

#include <chrono>
#include <map>

#include <rlutil.h>

class ConsoleGraphic {
public:
	ConsoleGraphic() : tetrisBoard_(20, 10, BlockType::I, BlockType::I) {
	}

	~ConsoleGraphic() {
		connection_.disconnect();
	}

	void restart(Player& player, TetrisEntry tetrisEntry) {
		connection_.disconnect();
		connection_ = player.addGameEventListener(std::bind(&ConsoleGraphic::callback, this, std::placeholders::_1, std::placeholders::_2));
	}

	int getWidth() const {
		return 25;
	}

	void callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
		tetrisBoard_ = tetrisBoard;
	}

	void draw(double time, int x, int y) {
		int rows = tetrisBoard_.getRows();
		int columns = tetrisBoard_.getColumns();

		// Draw frame around tetris board.
		rlutil::setColor(rlutil::WHITE);
		for (int row = 0; row < rows + 1; ++row) {
			draw(x + 1, y + 1 + row, 'X');
			draw(x + 1 + columns, y + 1 + row, 'X');
		}
		for (int column = 0; column < columns + 1; ++column) {
			draw(x + 1 + column, y + 1 + rows, 'X');
		}

		// Draw tetris board.
		for (int row = 0; row < rows; ++row) {
			for (int column = 0; column < columns; ++column) {
				BlockType blockType = tetrisBoard_.getBlockType(row, column);
				drawSquare(x + 1 + columns - column, y + 1 + rows - row, blockType);
			}
		}

		// Draw current block.
		Block currentBlock = tetrisBoard_.getBlock();
		for (const Square& sq : currentBlock) {
			drawSquare(x + 1 + columns - sq.column_, y + 1 + rows - sq.row_, sq.blockType_);
		}
	}

	void drawSquare(int x, int y, BlockType blockType) {
		rlutil::locate(x, y);
		switch (blockType) {
			case BlockType::I:
				rlutil::setColor(rlutil::CYAN);
				std::cout << SQUARE;
				break;
			case BlockType::J:
				rlutil::setColor(rlutil::BLUE);
				std::cout << SQUARE;
				break;
			case BlockType::L:
				rlutil::setColor(rlutil::LIGHTRED);
				std::cout << SQUARE;
				break;
			case BlockType::O:
				rlutil::setColor(rlutil::YELLOW);
				std::cout << SQUARE;
				break;
			case BlockType::S:
				rlutil::setColor(rlutil::RED);
				std::cout << SQUARE;
				break;
			case BlockType::T:
				rlutil::setColor(rlutil::MAGENTA);
				std::cout << SQUARE;
				break;
			case BlockType::Z:
				rlutil::setColor(rlutil::GREEN);
				std::cout << SQUARE;
				break;
			default:
				break;
		}
	}

private:
	void draw(int x, int y, char key) {
		rlutil::locate(x, y);
		std::cout << key;
	}

	static const char SQUARE = 254;

	mw::signals::Connection connection_;
	RawTetrisBoard tetrisBoard_;
};

class ConsoleKeyboard : public Device {
public:
	ConsoleKeyboard(std::string name, char down, char left, char right, char rotate, char downGround) : Device(false),
		name_(name),
		down_(down),
		right_(right),
		left_(left),
		rotate_(rotate),
		downGround_(downGround)	{
	}

	Input currentInput() override {
		return input_;
	}

	std::string getName() const override {
		return name_;
	}

	std::string getPlayerName() const override {
		return playerName_;
	}

	void setPlayerName(std::string playerName) override {
		playerName_ = playerName;
	}

	void eventUpdate(char key) {
		if (key == down_) {
			input_.down_ = true;
		} else if (key == left_) {
			input_.left_ = true;
		} else if (key == right_) {
			input_.right_ = true;
		} else if (key == rotate_) {
			input_.rotate_ = true;
		} else if (key == downGround_) {
			input_.downGround_ = true;
		}
	}

private:
	std::string name_, playerName_;
	char down_, right_, left_, rotate_, downGround_;
	Input input_;
};

class ConsoleTetris {
public:
	ConsoleTetris(TetrisEntry tetrisEntry) :
		tetrisEntry_(tetrisEntry),
		keyboard1_(std::make_shared<ConsoleKeyboard>("Keyboard 1", rlutil::KEY_DOWN, rlutil::KEY_LEFT, rlutil::KEY_RIGHT, rlutil::KEY_UP, '-')),
		keyboard2_(std::make_shared<ConsoleKeyboard>("Keyboard 2", 's', 'a', 'd', 'w', 'f')) {

		tetrisGame_.addCallback(std::bind(&ConsoleTetris::handleConnectionEvent, this, std::placeholders::_1));
	}

	void startLoop() {
		rlutil::hidecursor();
		rlutil::saveDefaultColor();
		rlutil::setColor(2);
		printf("MWetris Use arrow to move, ESC to quit.\n");
		rlutil::setColor(6);
		//rlutil::anykey("Hit any key to start.\n");

		auto time = std::chrono::high_resolution_clock::now();
		bool quit = false;

		// Initialization local game settings.
		std::vector<DevicePtr> devices_ = {keyboard1_, keyboard2_};
		tetrisGame_.setPlayers(devices_);
		tetrisGame_.createLocalGame();

		while (!quit) {
			auto currentTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> delta = currentTime - time;
			update(delta.count());
			time = currentTime;

			if (kbhit()) {
				char key = rlutil::getkey();

				keyboard1_->eventUpdate(key);
				keyboard2_->eventUpdate(key);

				if (key == rlutil::KEY_ESCAPE) {
					quit = true;
				}
				std::cout << key;
			}
			rlutil::msleep(100);
		}

		rlutil::cls();
		rlutil::resetColor();
		rlutil::showcursor();
	}

private:
	void handleConnectionEvent(TetrisGameEvent& tetrisEvent) {
		try {
			auto& gameOver = dynamic_cast<GameOver&>(tetrisEvent);
			
			return;
		} catch (std::bad_cast exp) {}

		try {
			auto& gamePause = dynamic_cast<GamePause&>(tetrisEvent);
			
			return;
		} catch (std::bad_cast exp) {}

		try {
			auto& newConnection = dynamic_cast<NewConnection&>(tetrisEvent);
			
			return;
		} catch (std::bad_cast exp) {}

		try {
			auto& initGameVar = dynamic_cast<InitGame&>(tetrisEvent);
			graphicPlayers_.clear();

			for (auto& player : initGameVar.players_) {
				auto& graphic = graphicPlayers_[player->getId()];
				graphic.restart(*player, tetrisEntry_.getDeepChildEntry("window"));
			}

			return;
		} catch (std::bad_cast exp) {}

		try {
			auto& start = dynamic_cast<GameStart&>(tetrisEvent);
			switch (start.status_) {
				case GameStart::LOCAL:
					break;
				case GameStart::CLIENT:
					break;
				case GameStart::SERVER:
					break;
			}
			return;
		} catch (std::bad_cast exp) {}
	}

	void update(double deltaTime) {
		// Drawing
		rlutil::cls();

		int w = 5;
		for (auto& pair : graphicPlayers_) {
			ConsoleGraphic& graphic = pair.second;
			graphic.draw(deltaTime, w, 2);
			w += graphic.getWidth();
		}

		tetrisGame_.update((int) (deltaTime * 1000));
	}

	TetrisGame tetrisGame_;
	TetrisEntry tetrisEntry_;
	std::shared_ptr<ConsoleKeyboard> keyboard1_, keyboard2_;
	std::map<int, ConsoleGraphic> graphicPlayers_;	
};

#endif // CONSOLETETRIS_H
