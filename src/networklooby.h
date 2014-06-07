#ifndef NETWORKLOOBY_H
#define NETWORKLOOBY_H

#include <gui/component.h>

#include <mw/text.h>
#include <mw/color.h>
#include <mw/font.h>

#include <list>
#include <string>
#include <sstream>
#include <functional>

class NetworkLooby : public gui::Component {
public:
	NetworkLooby(const mw::Font& font);

	void draw(Uint32 deltaTime) override;
	void addConnection(int id, int nbrOfPlayers, bool boolReady);
	void removeConnection(int id);
	void setReady(int id, bool ready);
	void clear();

private:
	struct Connection {
		Connection();
		Connection(int intId, int intNbrOfPlayers, bool boolReady, const mw::Font& font);

		int intId_;
		int intNbrOfPlayers_;
		bool boolReady_;
		mw::Text id_, numberOfPlayers_;
	};
	
	std::list<Connection> ascList_;
	mw::Color color_;
	int nextRecord_;
	mw::Font font_;
	mw::Text ready_, unready_;
};

#endif // NETWORKLOOBY_H
