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
	glUseProgram();
	mw::Matrix44 model = getModelMatrix();
	//setGlModelMatrix(model);

	gui::Dimension dim = getSize();

	static mw::Text id("Id", font_);

	mw::translate2D(model, 0, dim.height_ - id.getCharacterSize() - 2);
	setGlModelU(model);
	/*
	auto old2 = wp->getModel();
	id.draw();
	//wp->setModel(old * mw::getTranslateMatrix44(50, 0));
	static mw::Text nbr("Number Of Players", font_);
	nbr.draw(50, 0);
	//wp->setModel(wp->getModel() * mw::getTranslateMatrix44(200, 0));
	static mw::Text ready("Ready to Start?", font_);
	ready.draw(250, 0);
	mw::translate2D(old2, 0, -(id.getCharacterSize() + 4));
	for (Connection& connection : ascList_) {
		model = old2;
		wp->setModel(model);
		connection.id_.draw();
		connection.numberOfPlayers_.draw(50, 0);
		if (connection.boolReady_) {
			ready_.draw(50 + 200, 0);
		} else {
			unready_.draw(50 + 200, 0);
		}
	}
	*/
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

