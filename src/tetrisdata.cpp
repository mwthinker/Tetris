#include "tetrisdata.h"
#include "square.h"

#include <json.hpp>

#include <fstream>
#include <sstream>

using nlohmann::json;

namespace {

	BlockType charToBlockType(char key) {
		switch (key) {
			case 'z':
				// Fall through.
			case 'Z':
				return BlockType::Z;
			case 'w':
				// Fall through.
			case 'W':
				return BlockType::WALL;
			case 't':
				// Fall through.
			case 'T':
				return BlockType::T;
			case 's':
				// Fall through.
			case 'S':
				return BlockType::S;
			case 'o':
				// Fall through.
			case 'O':
				return BlockType::O;
			case 'l':
				// Fall through.
			case 'L':
				return BlockType::L;
			case 'j':
				// Fall through.
			case 'J':
				return BlockType::J;
			case 'I':
				// Fall through.
			case 'i':
				return BlockType::I;
			default:
				return BlockType::EMPTY;
		}
	}

	std::string blockTypeToString(BlockType blocktype) {
		switch (blocktype) {
			case BlockType::Z:
				return "Z";
			case BlockType::WALL:
				return "W";
			case BlockType::T:
				return "T";
			case BlockType::S:
				return "S";
			case BlockType::O:
				return "O";
			case BlockType::L:
				return "L";
			case BlockType::J:
				return "J";
			case BlockType::I:
				return "I";
			case BlockType::EMPTY:
				return "E";
			default:
				return "E";
		}
		
	}

}

namespace mw { // Must use the namespace color origin from.

	void from_json(const json& j, Color<float>& color) {
		std::stringstream stream(j.get<std::string>());
		if (!(stream >> color.red_)) {
			throw std::runtime_error("Red value invalid");
		}
		if (!(stream >> color.green_)) {
			throw std::runtime_error("Green value invalid");
		}
		if (!(stream >> color.blue_)) {
			throw std::runtime_error("Blue value invalid");
		}
		if (!stream.eof()) {
			if (!(stream >> color.alpha_)) {
				throw std::runtime_error("Alpha value invalid");
			}
		} else {
			color.alpha_ = 1;
		}
	}

}

void from_json(const json& j, Ai& ai) {
	ai = Ai(j.at("name").get<std::string>(), j.at("valueFunction").get<std::string>());
}

void from_json(const json& j, BlockType& blockType) {
	blockType = charToBlockType(j.get<std::string>()[0]);
}

void to_json(json& j, const BlockType& blockType) {
	j = json(blockTypeToString(blockType));
}

void from_json(const json& j, Block& block) {
	block = Block(j.at("blockType").get<BlockType>(), j.at("bottomRow").get<int>(), j.at("leftColumn").get<int>(), j.at("currentRotation").get<int>());
}

void to_json(json& j, const Block& block) {
	j = json({
		{"bottomRow", block.getLowestRow()},
		{"blockType", block.getBlockType()},
		{"leftColumn", block.getStartColumn()},
		{"currentRotation", block.getCurrentRotation()}
	});
}

std::string convertBlockTypesToString(const std::vector<BlockType>& board) {
	std::stringstream stream;
	for (BlockType chr : board) {
		stream << blockTypeToString(chr);
	}
	return stream.str();
}

std::vector<BlockType> convertStringToBlockTypes(std::string str) {
	std::vector<BlockType> blocktypes_;
	for (char key : str) {
		blocktypes_.push_back(charToBlockType(key));
	}
	return blocktypes_;
}

void from_json(const json& j, HighscoreRecord& highscoreRecord) {
	highscoreRecord.name_ = j.at("name").get<std::string>();
	highscoreRecord.date_ = j.at("date").get<std::string>();
	highscoreRecord.points_ = j.at("points").get<int>();
}

void to_json(json& j, const HighscoreRecord& highscoreRecord) {
	j = json{{"name", highscoreRecord.name_},{"date", highscoreRecord.date_},{"points", highscoreRecord.points_}};
}

TetrisData::TetrisData() : textureAtlas_(2048, 2048, []() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}) {
	std::ifstream stream(JSON_PATH);
	stream >> jsonObject_;
}

void TetrisData::save() {
	std::ofstream stream(JSON_PATH);
	stream << jsonObject_.dump(1);
}

mw::Font TetrisData::loadFont(std::string file, unsigned int fontSize) {
	unsigned int size = fonts_.size();
	std::string key = file;
	key += fontSize;
	mw::Font& font = fonts_[key];

	// Font not found?
	if (fonts_.size() > size) {
		font = mw::Font(file, fontSize);
	}

	return font;
}

mw::Sound TetrisData::loadSound(std::string file) {
	unsigned int size = sounds_.size();
	mw::Sound& sound = sounds_[file];

	// Sound not found?
	if (sounds_.size() > size) {
		sound = mw::Sound(file);
	}

	return sound;
}

mw::Music TetrisData::loadMusic(std::string file) {
	unsigned int size = musics_.size();
	mw::Music& music = musics_[file];

	// Music not found?
	if (musics_.size() > size) {
		music = mw::Music(file);
	}

	return music;
}

mw::Sprite TetrisData::loadSprite(std::string file) {
	return textureAtlas_.add(file, 1);;
}

mw::Sprite TetrisData::getSprite(BlockType blockType) {
	switch (blockType) {
		case BlockType::I:
			return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["squareI"].get<std::string>());
		case BlockType::J:
			return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["squareJ"].get<std::string>());
		case BlockType::L:
			return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["squareL"].get<std::string>());
		case BlockType::O:
			return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["squareO"].get<std::string>());
		case BlockType::S:
			return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["squareS"].get<std::string>());
		case BlockType::T:
			return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["squareT"].get<std::string>());
		case BlockType::Z:
			return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["squareZ"].get<std::string>());
	}
	return mw::Sprite();
}

mw::Sound TetrisData::getBlockCollisionSound() {
	return loadSound(jsonObject_["window"]["sounds"]["blockCollision"].get<std::string>());
}

mw::Sound TetrisData::getRowRemovedSound() {
	return loadSound(jsonObject_["window"]["sounds"]["rowRemoved"].get<std::string>());
}

mw::Sound TetrisData::getTetrisSound() {
	return loadSound(jsonObject_["window"]["sounds"]["tetris"].get<std::string>());
}

mw::Sound TetrisData::getButtonPushSound() {
	return loadSound(jsonObject_["window"]["sounds"]["buttonPush"].get<std::string>());
}

mw::Font TetrisData::getDefaultFont(int size) {
	return loadFont(jsonObject_["window"]["font"].get<std::string>(), size);
}

void TetrisData::bindTextureFromAtlas() const {
	textureAtlas_.getTexture().bindTexture();
}


Color TetrisData::getOuterSquareColor() {
	return jsonObject_["window"]["tetrisBoard"]["outerSquareColor"].get<Color>();
}

Color TetrisData::getInnerSquareColor() {
	return jsonObject_["window"]["tetrisBoard"]["innerSquareColor"].get<Color>();
}

Color TetrisData::getStartAreaColor() {
	return jsonObject_["window"]["tetrisBoard"]["startAreaColor"].get<Color>();
}

Color TetrisData::getPlayerAreaColor() {
	return jsonObject_["window"]["tetrisBoard"]["playerAreaColor"].get<Color>();
}

Color TetrisData::getBorderColor() {
	return jsonObject_["window"]["tetrisBoard"]["borderColor"].get<Color>();
}

mw::Sprite TetrisData::getBorderHorizontalSprite() {
	return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["borderHorizontal"].get<std::string>());
}

mw::Sprite TetrisData::getBorderVerticalSprite() {
	return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["borderVertical"].get<std::string>());
}

mw::Sprite TetrisData::getBorderLeftUpSprite() {
	return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["borderLeftUp"].get<std::string>());
}

mw::Sprite TetrisData::getBorderRightUpSprite() {
	return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["borderRightUp"].get<std::string>());
}

mw::Sprite TetrisData::getBorderDownRightSprite() {
	return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["borderDownRight"].get<std::string>());
}

mw::Sprite TetrisData::getBorderDownLeftSprite() {
	return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["borderDownLeft"].get<std::string>());
}

float TetrisData::getTetrisSquareSize() {
	return jsonObject_["window"]["tetrisBoard"]["squareSize"].get<float>();
}

float TetrisData::getTetrisBorderSize() {
	return jsonObject_["window"]["tetrisBoard"]["borderSize"].get<float>();
}

mw::Sprite TetrisData::getHalfCircleSprite() {
	return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["halfCircle"].get<std::string>());
}

mw::Sprite TetrisData::getLineSegmentSprite() {
	return loadSprite(jsonObject_["window"]["tetrisBoard"]["sprites"]["lineSegment"].get<std::string>());
}

int TetrisData::getWindowPositionX() {
	return jsonObject_["window"]["positionX"].get<int>();
}

int TetrisData::getWindowPositionY() {
	return jsonObject_["window"]["positionY"].get<int>();
}

void TetrisData::setWindowPositionX(int x) {
	jsonObject_["window"]["positionX"] = x;
}

void TetrisData::setWindowPositionY(int y) {
	jsonObject_["window"]["positionY"] = y;
}

int TetrisData::getWindowWidth() {
	return jsonObject_["window"]["width"].get<int>();
}

int TetrisData::getWindowHeight() {
	return jsonObject_["window"]["height"].get<int>();
}

void TetrisData::setWindowWidth(int width) {
	jsonObject_["window"]["width"] = width;
}

void TetrisData::setWindowHeight(int height) {
	jsonObject_["window"]["height"] = height;
}

bool TetrisData::isWindowResizable() {
	return jsonObject_["window"]["resizeable"].get<bool>();
}

void TetrisData::setWindowResizable(bool resizeable) {
	jsonObject_["window"]["resizeable"] = resizeable;
}

int TetrisData::getWindowMinWidth() {
	return jsonObject_["window"]["minWidth"].get<int>();
}

int TetrisData::getWindowMinHeight() {
	return jsonObject_["window"]["minHeight"].get<int>();
}

std::string TetrisData::getWindowIcon() {
	return jsonObject_["window"]["icon"].get<std::string>();
}

bool TetrisData::isWindowBordered() {
	return jsonObject_["window"]["border"].get<bool>();
}

void TetrisData::setWindowBordered(bool border) {
	jsonObject_["window"]["border"] = border;
}

bool TetrisData::isWindowMaximized() {
	return jsonObject_["window"]["maximized"].get<bool>();
}

void TetrisData::setWindowMaximized(bool maximized) {
	jsonObject_["window"]["maximized"] = maximized;
}

bool TetrisData::isWindowVsync() {
	return jsonObject_["window"]["vsync"].get<bool>();
}

void TetrisData::setWindowVsync(bool activate) {
	jsonObject_["window"]["vsync"] = activate;
}

mw::Sprite TetrisData::getBackgroundSprite() {
	return loadSprite(jsonObject_["window"]["sprites"]["background"].get<std::string>());
}

std::vector<Ai> TetrisData::getAiVector() {
	return std::vector<Ai>(jsonObject_["players"]["ais"].begin(), jsonObject_["players"]["ais"].end());
}

std::vector<HighscoreRecord> TetrisData::getHighscoreRecordVector() {
	return std::vector<HighscoreRecord>(jsonObject_["highscore"].begin(), jsonObject_["highscore"].end());
}

void TetrisData::setHighscoreRecordVector(const std::vector<HighscoreRecord>& highscoreVector) {
	jsonObject_["highscore"].clear();
	for (const HighscoreRecord& record : highscoreVector) {
		jsonObject_["highscore"].push_back(record);
	}
}

int TetrisData::getActiveLocalGameRows() {
	return jsonObject_["activeGames"]["localGame"]["rows"].get<int>();
}

int TetrisData::getActiveLocalGameColumns() {
	return jsonObject_["activeGames"]["localGame"]["columns"].get<int>();
}

bool TetrisData::isFullscreenOnDoubleClick() {
	return jsonObject_["window"]["fullscreenOnDoubleClick"].get<bool>();
}

void TetrisData::setFullscreenOnDoubleClick(bool activate) {
	jsonObject_["window"]["fullscreenOnDoubleClick"] = activate;
}

bool TetrisData::isMoveWindowByHoldingDownMouse() {
	return jsonObject_["window"]["moveWindowByHoldingDownMouse"].get<bool>();
}

void TetrisData::setMoveWindowByHoldingDownMouse(bool activate) {
	jsonObject_["window"]["moveWindowByHoldingDownMouse"] = activate;
}

float TetrisData::getWindowBarHeight() {
	return jsonObject_["window"]["bar"]["height"].get<float>();
}

Color TetrisData::getWindowBarColor() {
	return jsonObject_["window"]["bar"]["color"].get<Color>();
}

mw::Sprite TetrisData::getCheckboxBoxSprite() {
	return loadSprite(jsonObject_["window"]["checkBox"]["boxImage"].get<std::string>());
}

mw::Sprite TetrisData::getCheckboxCheckSprite() {
	return loadSprite(jsonObject_["window"]["checkBox"]["checkImage"].get<std::string>());
}

Color TetrisData::getCheckboxTextColor() {
	return jsonObject_["window"]["checkBox"]["textColor"].get<Color>();
}

Color TetrisData::getCheckboxBackgroundColor() {
	return jsonObject_["window"]["checkBox"]["backgroundColor"].get<Color>();
}

Color TetrisData::getCheckboxBoxColor() {
	return jsonObject_["window"]["checkBox"]["boxColor"].get<Color>();
}

Color TetrisData::getChecboxCheckColor() {
	return jsonObject_["window"]["checkBox"]["checkColor"].get<Color>();
}

Color TetrisData::getLabelTextColor() {
	return jsonObject_["window"]["label"]["textColor"].get<Color>();
}

Color TetrisData::getLabelBackgroundColor() {
	return jsonObject_["window"]["label"]["backgroundColor"].get<Color>();
}


Color TetrisData::getButtonFocusColor() {
	return jsonObject_["window"]["button"]["focusColor"].get<Color>();
}

Color TetrisData::getButtonTextColor() {
	return jsonObject_["window"]["button"]["textColor"].get<Color>();
}

Color TetrisData::getButtonHoverColor() {
	return jsonObject_["window"]["button"]["hoverColor"].get<Color>();
}

Color TetrisData::getButtonPushColor() {
	return jsonObject_["window"]["button"]["pushColor"].get<Color>();
}

Color TetrisData::getButtonBackgroundColor() {
	return jsonObject_["window"]["button"]["backgroundColor"].get<Color>();
}

Color TetrisData::getButtonBorderColor() {
	return jsonObject_["window"]["button"]["borderColor"].get<Color>();
}

Color TetrisData::getComboBoxFocusColor() {
	return jsonObject_["window"]["comboBox"]["focusColor"].get<Color>();
}

Color TetrisData::getComboBoxTextColor() {
	return jsonObject_["window"]["comboBox"]["textColor"].get<Color>();
}

Color TetrisData::getComboBoxSelectedTextColor() {
	return jsonObject_["window"]["comboBox"]["selectedTextColor"].get<Color>();
}

Color TetrisData::getComboBoxSelectedBackgroundColor() {
	return jsonObject_["window"]["comboBox"]["selectedBackgroundColor"].get<Color>();
}

Color TetrisData::getComboBoxBackgroundColor() {
	return jsonObject_["window"]["comboBox"]["backgroundColor"].get<Color>();
}

Color TetrisData::getComboBoxBorderColor() {
	return jsonObject_["window"]["comboBox"]["borderColor"].get<Color>();
}

Color TetrisData::getComboBoxShowDropDownColor() {
	return jsonObject_["window"]["comboBox"]["showDropDownColor"].get<Color>();
}

mw::Sprite TetrisData::getComboBoxShowDropDownSprite() {
	return loadSprite(jsonObject_["window"]["comboBox"]["showDropDownSprite"].get<std::string>());
}

mw::Sprite TetrisData::getHumanSprite() {
	return loadSprite(jsonObject_["window"]["sprites"]["human"].get<std::string>());
}

mw::Sprite TetrisData::getComputerSprite() {
	return loadSprite(jsonObject_["window"]["sprites"]["computer"].get<std::string>());
}

mw::Sprite TetrisData::getCrossSprite() {
	return loadSprite(jsonObject_["window"]["sprites"]["cross"].get<std::string>());
}

mw::Sprite TetrisData::getZoomSprite() {
	return loadSprite(jsonObject_["window"]["sprites"]["zoom"].get<std::string>());
}

void TetrisData::setActiveLocalGame(int rows, int columns, std::vector<PlayerData> playerDataVector) {
	jsonObject_["activeGames"]["localGame"]["rows"] = rows;
	jsonObject_["activeGames"]["localGame"]["columns"] = columns;
	
	nlohmann::json playerJson = nlohmann::json::array();
	for (PlayerData& data : playerDataVector) {
		nlohmann::json test = nlohmann::json(data.board_);

		playerJson.push_back({
			{"name", data.name_},
			{"nextBlockType", data.next_},
			{"levelUpCounter", data.levelUpCounter_},
			{"ai", data.device_->isAi()},
			{"level", data.level_},
			{"points", data.points_},
			{"currentBlock", data.current_},
			{"board", convertBlockTypesToString(data.board_)},
			{"device",{
				{"name", data.device_->getName()},
				{"ai", data.device_->isAi()},
			}}
		});
	}
	jsonObject_["activeGames"]["localGame"]["players"] = playerJson;
}

std::vector<PlayerData> TetrisData::getActiveLocalGamePlayers() {
	nlohmann::json players = jsonObject_["activeGames"]["localGame"]["players"];

	std::vector<PlayerData> playerDataVector;
	for (nlohmann::json& player : players) {
		PlayerData playerData;
		playerData.name_ = player["name"].get<std::string>();
		playerData.next_ = player["nextBlockType"].get<BlockType>();
		playerData.levelUpCounter_ = player["levelUpCounter"].get<int>();
		playerData.level_ = player["level"].get<int>();
		playerData.points_ = player["points"].get<int>();
		playerData.current_ = player["currentBlock"].get<Block>();
		playerData.board_ = convertStringToBlockTypes(player["board"].get<std::string>());
		playerData.ai_ = player["ai"].get<bool>();
		playerData.deviceName_ = player["device"]["name"].get<std::string>();
		playerDataVector.push_back(playerData);
	}
	return playerDataVector;
}
