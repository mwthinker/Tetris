#ifndef NETWORKLOOBY_H
#define NETWORKLOOBY_H

#include "gamefont.h"

#include <gui/component.h>
#include <mw/text.h>
#include <mw/color.h>

#include <list>
#include <string>
#include <sstream>
#include <memory>
#include <functional>

class NetworkLooby : public gui::Component {
public:
	NetworkLooby();

	void draw(float deltaTime) override;
	void addConnection(int id, int nbrOfPlayers, bool boolReady);
	void removeConnection(int id);
	void setReady(int id, bool ready);
	void clear();

private:
	struct Connection {
		Connection();
		Connection(int intId, int intNbrOfPlayers, bool boolReady);

		int intId_;
		int intNbrOfPlayers_;
		bool boolReady_;
		mw::Text id_, numberOfPlayers_;
		static mw::Text ready, unready;
	};
	
	std::list<Connection> ascList_;
	mw::Color color_;
	int nextRecord_;
};

typedef std::shared_ptr<NetworkLooby> NetworkLoobyPtr;

NetworkLoobyPtr createNetworkLooby();

#endif // NETWORKLOOBY_H
