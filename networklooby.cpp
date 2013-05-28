#include "networklooby.h"
#include "guiitem.h"
#include "gamefont.h"

#include <mw/text.h>
#include <mw/color.h>

#include <list>
#include <string>
#include <sstream>
#include <memory>
#include <functional>
#include <algorithm>

NetworkLooby::NetworkLooby() {
}

void NetworkLooby::draw() {
	for (Connection& connection : ascList_) {
		glPushMatrix();
		connection.id_.draw();
		glTranslated(50,0,0);
		connection.numberOfPlayers_.draw();
		glTranslated(150,0,0);
		connection.ready_.draw();
		glPopMatrix();
		glTranslated(0,fontDefault18->getCharacterSize()+2,0);
	}	
}

void NetworkLooby::addConnection(int id, int nbrOfPlayers) {
	ascList_.push_back(Connection(id,nbrOfPlayers));
}

void NetworkLooby::removeConnection(int id) {
	ascList_.remove_if([&](const Connection& c) {
		return c.intId_ == id;
	});
}

void NetworkLooby::setReady(int id, bool ready) {	
	auto it = std::find_if(ascList_.begin(),ascList_.end(),[&](const Connection& c) {
		return c.intId_ == id;
	});
	
	if (it != ascList_.end()) {
		Connection& c = *it;
		if (c.boolReady_ != ready) {
			c.boolReady_ = ready;
			if (ready) {
				c.ready_.setText("Ready");
			} else {
				c.ready_.setText("Unready");
			}
		}
	}
}

void NetworkLooby::clear() {
	ascList_.clear();
}

NetworkLooby::Connection::Connection() {
}

NetworkLooby::Connection::Connection(int intId, int intNbrOfPlayers) {
	intId_ = intId;
	intNbrOfPlayers_ = intNbrOfPlayers;
	std::stringstream stream;
	stream << intId_;
	id_ = mw::Text(stream.str(),fontDefault18);
	stream.str("");
	numberOfPlayers_ = mw::Text(stream.str(),fontDefault18);
	stream << intNbrOfPlayers;
	numberOfPlayers_ = mw::Text(stream.str(),fontDefault18);
	ready_ = mw::Text("Unready",fontDefault18);
	boolReady_ = false;
}

NetworkLoobyPtr createNetworkLooby() {
	return NetworkLoobyPtr(new  NetworkLooby);
}