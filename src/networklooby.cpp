#include "networklooby.h"

#include <mw/text.h>
#include <mw/color.h>
#include <mw/font.h>

#include <list>
#include <string>
#include <sstream>
#include <memory>
#include <functional>
#include <algorithm>

NetworkLooby::NetworkLooby(const mw::Font& font) : font_(font) {
	setPreferredSize(400, 400);
	ready_ = mw::Text("Ready", font);
	unready_ = mw::Text("Unready", font);
}

void NetworkLooby::draw(Uint32 deltaTime) {
	auto wp = getWindowMatrixPtr();
	wp->useShader();
	auto old = wp->getModel();

	gui::Dimension dim = getSize();
	
	static mw::Text id("Id", font_);
	wp->setModel(old * mw::getTranslateMatrix(0, dim.height_) * mw::getTranslateMatrix(0, -id.getCharacterSize() - 2));
	auto old2 = wp->getModel();
	id.draw();
	wp->setModel(old * mw::getTranslateMatrix(50, 0));
	static mw::Text nbr("Number Of Players", font_);
	nbr.draw();
	wp->setModel(wp->getModel() * mw::getTranslateMatrix(200, 0));
	static mw::Text ready("Ready to Start?", font_);
	ready.draw();
	old2 = old2 * mw::getTranslateMatrix(0, -(id.getCharacterSize() + 4));
	for (Connection& connection : ascList_) {
		wp->setModel(old2);
		connection.id_.draw();
		wp->setModel(wp->getModel() * mw::getTranslateMatrix(50, 0));
		connection.numberOfPlayers_.draw();
		wp->setModel(wp->getModel() * mw::getTranslateMatrix(200, 0));
		if (connection.boolReady_) {
			ready_.draw();
		} else {
			unready_.draw();
		}
	}
	wp->setModel(old);
}

void NetworkLooby::addConnection(int id, int nbrOfPlayers, bool boolReady) {
	ascList_.push_front(Connection(id, nbrOfPlayers, boolReady, font_));
}

void NetworkLooby::removeConnection(int id) {
	ascList_.remove_if([&](const Connection& c) {
		return c.intId_ == id;
	});
}

void NetworkLooby::setReady(int id, bool ready) {
	auto it = std::find_if(ascList_.begin(), ascList_.end(), [&](const Connection& c) {
		return c.intId_ == id;
	});

	if (it != ascList_.end()) {
		Connection& c = *it;
		if (c.boolReady_ != ready) {
			c.boolReady_ = ready;
		}
	}
}

void NetworkLooby::clear() {
	ascList_.clear();
}

NetworkLooby::Connection::Connection() {
}

NetworkLooby::Connection::Connection(int intId, int intNbrOfPlayers, bool boolReady, const mw::Font& font) {
	intId_ = intId;
	intNbrOfPlayers_ = intNbrOfPlayers;
	std::stringstream stream;
	stream << intId_;
	id_ = mw::Text(stream.str(), font);
	stream.str("");
	numberOfPlayers_ = mw::Text(stream.str(), font);
	stream << intNbrOfPlayers;
	numberOfPlayers_ = mw::Text(stream.str(), font);
	boolReady_ = boolReady;
}

