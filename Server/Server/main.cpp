#include <SFML/Network.hpp>

using namespace sf;

int main()
{
	std::vector<TcpSocket*> clients;
	clients.push_back(new TcpSocket);

	TcpListener listener;
	Packet packet;

	printf("IP: %s    Port: 2000", IpAddress::getLocalAddress().toString());

	listener.listen(2000);	
	while (true)
	{		
		if (listener.accept(*clients.back()) == Socket::Done)
		{
			printf("\nAccepted!");
			if (listener.isBlocking())
			{
				listener.setBlocking(false);
			}
			if (clients.back()->isBlocking())
			{
				clients.back()->setBlocking(false);
			}
			clients.push_back(new TcpSocket);
		}

		for (int i = 0; i < clients.size(); i++)
		{
			if (clients[i]->isBlocking())
			{
				clients[i]->setBlocking(false);
			}

			Socket::Status s = clients[i]->receive(packet);
			if (s == Socket::Done)
			{
				printf("\nReceived!");
				/////////////////////////////////////////////////////
				for (int j = 0; j < clients.size(); j++)
				{
					clients[j]->send(packet);
				}
				packet.clear();
			}	
			else if (s == Socket::Disconnected) 
			{
				printf("\nDisconnected!");				
				clients[i]->disconnect();
				std::vector<TcpSocket*>::iterator it = clients.begin() + i;
				clients.erase(it);
				packet.clear();
				i--;
			}
		}
	}
	clients.clear();
	return 0;
}