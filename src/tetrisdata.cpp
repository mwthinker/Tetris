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

	void from_json(const json& j, mw::Color& color) {
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
		{"bottomRow", block.getLowestStartRow()},
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

	try {
		highscoreRecord.level_ = j.at("level").get<int>();
	} catch (nlohmann::detail::out_of_range) {
		highscoreRecord.level_ = 0;
	}

	try {
		highscoreRecord.rows_ = j.at("rows").get<int>();
	} catch (nlohmann::detail::out_of_range) {
		highscoreRecord.rows_ = 0;
	}
}

void to_json(json& j, const HighscoreRecord& highscoreRecord) {
	j = json{{"name", highscoreRecord.name_},
	{"date", highscoreRecord.date_},
	{"points", highscoreRecord.points_},
	{"level", highscoreRecord.level_},
	{"rows", highscoreRecord.rows_}
	};
}

TetrisData::TetrisData() : textureAtlas_(2048, 2048, []() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}) {
	std::ifstream defaultStream("USE_APPLICATION_JSON");
	bool applicationJson;
	defaultStream >> applicationJson;
	const std::string APPLICATION_JSON = "tetris.json";
	if (applicationJson) {
		jsonPath_ = APPLICATION_JSON;
	} else {
		// Find default path to save/load file from.
		jsonPath_ = SDL_GetPrefPath("mwthinker", "MWetris2") + APPLICATION_JSON;
	}
	std::ifstream stream(jsonPath_);
	if (!stream.is_open()) {
		// Assume that the file does not exist, load file from application folder.
		stream = std::ifstream(APPLICATION_JSON);
	}
	stream >> jsonObject_;
}

void TetrisData::save() {
	std::ofstream stream(jsonPath_);
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

mw::Font TetrisData::getDefaultFont(int size) {
	return loadFont(jsonObject_["window"]["font"].get<std::string>(), size);
}

void TetrisData::bindTextureFromAtlas() const {
	textureAtlas_.getTexture().bindTexture();
}

mw::Color TetrisData::getOuterSquareColor() const {
	return jsonObject_["window"]["tetrisBoard"]["outerSquareColor"].get<mw::Color>();
}

mw::Color TetrisData::getInnerSquareColor() const {
	return jsonObject_["window"]["tetrisBoard"]["innerSquareColor"].get<mw::Color>();
}

mw::Color TetrisData::getStartAreaColor() const {
	return jsonObject_["window"]["tetrisBoard"]["startAreaColor"].get<mw::Color>();
}

mw::Color TetrisData::getPlayerAreaColor() const {
	return jsonObject_["window"]["tetrisBoard"]["playerAreaColor"].get<mw::Color>();
}

mw::Color TetrisData::getBorderColor() const {
	return jsonObject_["window"]["tetrisBoard"]["borderColor"].get<mw::Color>();
}

float TetrisData::getTetrisSquareSize() const {
	return jsonObject_["window"]["tetrisBoard"]["squareSize"].get<float>();
}

float TetrisData::getTetrisBorderSize() const {
	return jsonObject_["window"]["tetrisBoard"]["borderSize"].get<float>();
}

bool TetrisData::isLimitFps() const {
	try {
		return jsonObject_.at("window").at("limitFps").get<bool>();
	} catch (nlohmann::detail::out_of_range) {
		return false;
	}
}

void TetrisData::setLimitFps(bool limited) {
	jsonObject_["window"]["limitFps"] = limited;
}

int TetrisData::getWindowPositionX() const {
	return jsonObject_["window"]["positionX"].get<int>();
}

int TetrisData::getWindowPositionY() const {
	return jsonObject_["window"]["positionY"].get<int>();
}

void TetrisData::setWindowPositionX(int x) {
	jsonObject_["window"]["positionX"] = x;
}

void TetrisData::setWindowPositionY(int y) {
	jsonObject_["window"]["positionY"] = y;
}

int TetrisData::getWindowWidth() const {
	return jsonObject_["window"]["width"].get<int>();
}

int TetrisData::getWindowHeight() const {
	return jsonObject_["window"]["height"].get<int>();
}

void TetrisData::setWindowWidth(int width) {
	jsonObject_["window"]["width"] = width;
}

void TetrisData::setWindowHeight(int height) {
	jsonObject_["window"]["height"] = height;
}

bool TetrisData::isWindowResizable() const {
	return jsonObject_["window"]["resizeable"].get<bool>();
}

void TetrisData::setWindowResizable(bool resizeable) {
	jsonObject_["window"]["resizeable"] = resizeable;
}

int TetrisData::getWindowMinWidth() const {
	return jsonObject_["window"]["minWidth"].get<int>();
}

int TetrisData::getWindowMinHeight() const {
	return jsonObject_["window"]["minHeight"].get<int>();
}

std::string TetrisData::getWindowIcon() const {
	return jsonObject_["window"]["icon"].get<std::string>();
}

bool TetrisData::isWindowBordered() const {
	return jsonObject_["window"]["border"].get<bool>();
}

void TetrisData::setWindowBordered(bool border) {
	jsonObject_["window"]["border"] = border;
}

bool TetrisData::isWindowPauseOnLostFocus() const {
	try {
		return jsonObject_.at("window").at("pauseOnLostFocus").get<bool>();
	} catch (nlohmann::detail::out_of_range) {
		return true;
	}
}

void TetrisData::setWindowPauseOnLostFocus(bool pauseOnFocus) {
	jsonObject_["window"]["pauseOnLostFocus"] = pauseOnFocus;
}

bool TetrisData::isWindowMaximized() const {
	return jsonObject_["window"]["maximized"].get<bool>();
}

void TetrisData::setWindowMaximized(bool maximized) {
	jsonObject_["window"]["maximized"] = maximized;
}

bool TetrisData::isWindowVsync() const {
	return jsonObject_["window"]["vsync"].get<bool>();
}

void TetrisData::setWindowVsync(bool activate) {
	jsonObject_["window"]["vsync"] = activate;
}

int TetrisData::getMultiSampleBuffers() const {
	return jsonObject_["window"]["multiSampleBuffers"];
}

int TetrisData::getMultiSampleSamples() const {
	return jsonObject_["window"]["multiSampleSamples"];
}

float TetrisData::getRowFadingTime() const {
	return jsonObject_["window"]["rowFadingTime"].get<float>();
}

void TetrisData::setRowFadingTime(float time) {
	jsonObject_["window"]["rowFadingTime"] = time;
}

float TetrisData::getRowMovingTime() const {
	return jsonObject_["window"]["rowMovingTime"].get<float>();
}

void TetrisData::setRowMovingTime(float time) {
	jsonObject_["window"]["rowMovingTime"] = time;
}

mw::Sprite TetrisData::getBackgroundSprite() {
	return loadSprite(jsonObject_["window"]["sprites"]["background"].get<std::string>());
}

std::string TetrisData::getAi1Name() const {
	return jsonObject_["ai1"].get<std::string>();
}
std::string TetrisData::getAi2Name() const {
	return jsonObject_["ai2"].get<std::string>();
}
std::string TetrisData::getAi3Name() const {
	return jsonObject_["ai3"].get<std::string>();
}
std::string TetrisData::getAi4Name() const {
	return jsonObject_["ai4"].get<std::string>();
}

void TetrisData::setAi1Name(std::string name) {
	jsonObject_["ai1"] = name;
}

void TetrisData::setAi2Name(std::string name) {
	jsonObject_["ai2"] = name;
}

void TetrisData::setAi3Name(std::string name) {
	jsonObject_["ai3"] = name;
}

void TetrisData::setAi4Name(std::string name) {
	jsonObject_["ai4"] = name;
}

std::vector<Ai> TetrisData::getAiVector() const {
	std::vector<Ai> ais;
	ais.push_back(Ai()); // Add default ai.
	ais.insert(ais.end(), jsonObject_["ais"].begin(), jsonObject_["ais"].end());
	return ais;
}

std::vector<HighscoreRecord> TetrisData::getHighscoreRecordVector() const {
	return std::vector<HighscoreRecord>(jsonObject_["highscore"].begin(), jsonObject_["highscore"].end());
}

void TetrisData::setHighscoreRecordVector(const std::vector<HighscoreRecord>& highscoreVector) {
	jsonObject_["highscore"].clear();
	for (const HighscoreRecord& record : highscoreVector) {
		jsonObject_["highscore"].push_back(record);
	}
}

int TetrisData::getActiveLocalGameRows() const {
	return jsonObject_["activeGames"]["localGame"]["rows"].get<int>();
}

int TetrisData::getActiveLocalGameColumns() const {
	return jsonObject_["activeGames"]["localGame"]["columns"].get<int>();
}

bool TetrisData::isFullscreenOnDoubleClick() const {
	return jsonObject_["window"]["fullscreenOnDoubleClick"].get<bool>();
}

void TetrisData::setFullscreenOnDoubleClick(bool activate) {
	jsonObject_["window"]["fullscreenOnDoubleClick"] = activate;
}

bool TetrisData::isMoveWindowByHoldingDownMouse() const {
	return jsonObject_["window"]["moveWindowByHoldingDownMouse"].get<bool>();
}

void TetrisData::setMoveWindowByHoldingDownMouse(bool activate) {
	jsonObject_["window"]["moveWindowByHoldingDownMouse"] = activate;
}

int TetrisData::getPort() const {
	return jsonObject_["window"]["port"].get<int>();
}

void TetrisData::setPort(int port) {
	jsonObject_["window"]["port"] = port;
}

int TetrisData::getTimeToConnectMS() const {
	return jsonObject_["window"]["timeToConnectMS"].get<int>();
}

std::string TetrisData::getIp() const {
	return jsonObject_["window"]["ip"].get<std::string>();
}

void TetrisData::setIp(std::string ip) {
	jsonObject_["window"]["ip"] = ip;
}

float TetrisData::getWindowBarHeight() const {
	return jsonObject_["window"]["bar"]["height"].get<float>();
}

mw::Color TetrisData::getWindowBarColor() const {
	return jsonObject_["window"]["bar"]["color"].get<mw::Color>();
}

mw::Sprite TetrisData::getCheckboxBoxSprite() {
	return loadSprite(jsonObject_["window"]["checkBox"]["boxImage"].get<std::string>());
}

mw::Sprite TetrisData::getCheckboxCheckSprite() {
	return loadSprite(jsonObject_["window"]["checkBox"]["checkImage"].get<std::string>());
}

mw::Color TetrisData::getCheckboxTextColor() const {
	return jsonObject_["window"]["checkBox"]["textColor"].get<mw::Color>();
}

mw::Color TetrisData::getCheckboxBackgroundColor() const {
	return jsonObject_["window"]["checkBox"]["backgroundColor"].get<mw::Color>();
}

mw::Color TetrisData::getCheckboxBoxColor() const {
	return jsonObject_["window"]["checkBox"]["boxColor"].get<mw::Color>();
}

mw::Color TetrisData::getChecboxCheckColor() const {
	return jsonObject_["window"]["checkBox"]["checkColor"].get<mw::Color>();
}

mw::Sprite TetrisData::getRadioButtonBoxSprite() {
	return loadSprite(jsonObject_["window"]["radioButton"]["boxImage"].get<std::string>());
}

mw::Sprite TetrisData::getRadioButtonCheckSprite() {
	return loadSprite(jsonObject_["window"]["radioButton"]["checkImage"].get<std::string>());
}

mw::Color TetrisData::getRadioButtonTextColor() const {
	return jsonObject_["window"]["radioButton"]["textColor"].get<mw::Color>();
}

mw::Color TetrisData::getRadioButtonBackgroundColor() const {
	return jsonObject_["window"]["radioButton"]["backgroundColor"].get<mw::Color>();
}

mw::Color TetrisData::getRadioButtonBoxColor() const {
	return jsonObject_["window"]["radioButton"]["boxColor"].get<mw::Color>();
}

mw::Color TetrisData::getRadioButtonCheckColor() const {
	return jsonObject_["window"]["radioButton"]["checkColor"].get<mw::Color>();
}

mw::Color TetrisData::getLabelTextColor() const {
	return jsonObject_["window"]["label"]["textColor"].get<mw::Color>();
}

mw::Color TetrisData::getLabelBackgroundColor() const {
	return jsonObject_["window"]["label"]["backgroundColor"].get<mw::Color>();
}

mw::Color TetrisData::getButtonFocusColor() const {
	return jsonObject_["window"]["button"]["focusColor"].get<mw::Color>();
}

mw::Color TetrisData::getButtonTextColor() const {
	return jsonObject_["window"]["button"]["textColor"].get<mw::Color>();
}

mw::Color TetrisData::getButtonHoverColor() const {
	return jsonObject_["window"]["button"]["hoverColor"].get<mw::Color>();
}

mw::Color TetrisData::getButtonPushColor() const {
	return jsonObject_["window"]["button"]["pushColor"].get<mw::Color>();
}

mw::Color TetrisData::getButtonBackgroundColor() const {
	return jsonObject_["window"]["button"]["backgroundColor"].get<mw::Color>();
}

mw::Color TetrisData::getButtonBorderColor() const {
	return jsonObject_["window"]["button"]["borderColor"].get<mw::Color>();
}

mw::Color TetrisData::getComboBoxFocusColor() const {
	return jsonObject_["window"]["comboBox"]["focusColor"].get<mw::Color>();
}

mw::Color TetrisData::getComboBoxTextColor() const {
	return jsonObject_["window"]["comboBox"]["textColor"].get<mw::Color>();
}

mw::Color TetrisData::getComboBoxSelectedTextColor() const {
	return jsonObject_["window"]["comboBox"]["selectedTextColor"].get<mw::Color>();
}

mw::Color TetrisData::getComboBoxSelectedBackgroundColor() const {
	return jsonObject_["window"]["comboBox"]["selectedBackgroundColor"].get<mw::Color>();
}

mw::Color TetrisData::getComboBoxBackgroundColor() const {
	return jsonObject_["window"]["comboBox"]["backgroundColor"].get<mw::Color>();
}

mw::Color TetrisData::getComboBoxBorderColor() const {
	return jsonObject_["window"]["comboBox"]["borderColor"].get<mw::Color>();
}

mw::Color TetrisData::getComboBoxShowDropDownColor() const {
	return jsonObject_["window"]["comboBox"]["showDropDownColor"].get<mw::Color>();
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

void TetrisData::setActiveLocalGame(int rows, int columns, const std::vector<PlayerData>& playerDataVector) {
	jsonObject_["activeGames"]["localGame"]["rows"] = rows;
	jsonObject_["activeGames"]["localGame"]["columns"] = columns;

	nlohmann::json playerJson = nlohmann::json::array();
	for (const PlayerData& data : playerDataVector) {
		nlohmann::json test = nlohmann::json(data.board_);

		playerJson.push_back({
			{"name", data.name_},
			{"lastPosition", data.lastPosition_},
			{"nextBlockType", data.next_},
			{"levelUpCounter", data.levelUpCounter_},
			{"ai", data.device_->isAi()},
			{"level", data.level_},
			{"points", data.points_},
			{"clearedRows", data.clearedRows_},
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
		playerData.lastPosition_ = player["lastPosition"].get<int>();
		playerData.next_ = player["nextBlockType"].get<BlockType>();
		playerData.levelUpCounter_ = player["levelUpCounter"].get<int>();
		playerData.level_ = player["level"].get<int>();
		playerData.points_ = player["points"].get<int>();
		playerData.clearedRows_ = player["clearedRows"].get<int>();
		playerData.current_ = player["currentBlock"].get<Block>();
		playerData.board_ = convertStringToBlockTypes(player["board"].get<std::string>());
		playerData.ai_ = player["ai"].get<bool>();
		playerData.deviceName_ = player["device"]["name"].get<std::string>();
		playerDataVector.push_back(playerData);
	}
	return playerDataVector;
}
