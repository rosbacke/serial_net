/*
 * ByteEther.cpp
 *
 *  Created on: 14 jul 2016
 *      Author: mikaelr
 */

#include "ByteEther.h"

ByteEther::ByteEther()
{
}

ByteEther::~ByteEther()
{
}

// Add a new client to be part of the network.
void ByteEther::addClient(ByteEtherIf::RxIf* client)
{
	m_clients.push_back(client);
}

// Send a byte to the ether.
void ByteEther::sendByte(gsl::byte myByte)
{
	m_txBuffer.push_back(myByte);
}


bool ByteEther::execute()
{
	decltype(m_txBuffer) tmp;
	tmp.swap(m_txBuffer);
	for (auto i : tmp)
	{
		for (auto j : m_clients)
		{
			j->newByte(i);
		}
	}
	return false;
}
