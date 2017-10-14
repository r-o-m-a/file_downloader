#include "inc/Cols.h"

Cols::Cols()
{
	this->add(this->nameCol);
	this->add(this->progressCol);
	this->add(this->statusCol);
	this->add(this->urlCol);
	this->add(this->destinationCol);
	this->add(this->protocolCol);
}
