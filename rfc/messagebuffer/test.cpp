#include <iostream>
#include "messagebuffer.h"
#include <stddef.h>

using namespace std;

phy::Message phyMsg;
mac::Message macMsg;
routing::Message routingMsg;

int main()
{
	cout << "phyMsg: " << sizeof(phyMsg) << endl;
	cout << "phyPayload: " << sizeof(phyMsg.payload) << endl;
	cout << "macMsg: " << sizeof(macMsg) << endl;
	cout << "macPayload: " << sizeof(macMsg.payload) << endl;
	cout << "routingMsg: " << sizeof(routingMsg) << endl;
	cout << "routingPayload: " << sizeof(routingMsg.payload) << endl;
	cout << "tt: " << offsetof(routing::Message,src) << endl;
	return 0;
}
