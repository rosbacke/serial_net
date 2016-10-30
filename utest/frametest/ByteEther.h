/*
 * ByteEther.h
 *
 *  Created on: 14 jul 2016
 *      Author: mikaelr
 */

#ifndef DOC_UTEST_FRAMETEST_BYTEETHER_H_
#define DOC_UTEST_FRAMETEST_BYTEETHER_H_

#include <vector>
#include <cstdint>

#include "interfaces/ByteEtherIf.h"
#include "interfaces/RuntimeIf.h"

/**
 * Responsible for distributing bytes between several clients.
 * Implement common medium for the case of several objects in one executable.
 */
class ByteEther : public ByteEtherIf, public RuntimeIf
{
public:
	ByteEther();
	~ByteEther();

	// Send out all stored bytes.
	// return true if more work remains.
	bool execute();

	// Add a new client to be part of the network.
	void addClient(ByteEtherIf::RxIf* client);

	// Send a byte to the ether.
	void sendByte(gsl::byte byte);

private:
	std::vector<ByteEtherIf::RxIf*> m_clients;

	// Currently queued up bytes to be sent.
	ByteVec m_txBuffer;
};

#endif /* DOC_UTEST_FRAMETEST_BYTEETHER_H_ */
