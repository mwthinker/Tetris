#ifndef CONNECTION_H
#define CONNECTION_H

#include "protocol.h"
#include "tetrisparameters.h"

class Connection {
public:
	virtual ~Connection() = default;
	
	virtual int getNbrAiPlayers() const = 0;

	virtual int getNbrHumanPlayers() const = 0;
};

#endif // CONNECTION_H
