#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;

int main()
{
	RenderWindow window(VideoMode(600, 600), "Telegrammar");
	TcpSocket socket;

	Font font;
	font.loadFromFile("OpenSans-Regular.ttf");
	//Text info("Your IP: " + IpAddress::getLocalAddress().toString(), font, 30);
	Text info("Enter server's IP", font, 30);
	String s = "";
	String rs = "";
	String myName = "";
	Text myMessage("", font, 30);
	Text allMessages("", font, 30);
	int allMessagesLine = 0;

	info.setPosition(Vector2f(20, 400));
	myMessage.setPosition(Vector2f(20, 450));
	allMessages.setPosition(Vector2f(20, 20));
	allMessages.setString("");
	Packet packet;

	int state = 0;
	bool send = false;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}

			if (event.type == Event::KeyPressed)
			{
				if (s.getSize() > 0)
				{
					if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
					{
						send = true;
						myMessage.setString("");
					}
					else if (Keyboard::isKeyPressed(Keyboard::Key::BackSpace))
					{
						if (s[s.getSize() - 1] == '\n')
						{
							s.erase(s.getSize() - 2);
						}	
						else
						{
							s.erase(s.getSize() - 1);
						}
						myMessage.setString(s);
					}
				}
			}
			if (event.type == Event::TextEntered && !Keyboard::isKeyPressed(Keyboard::Key::BackSpace) && !Keyboard::isKeyPressed(Keyboard::Key::Enter))
			{
				if (s.getSize() < 98)
				{
					if (s.getSize() == 32 || s.getSize() == 65)
					{
						s += "\n";
					}
					s += event.text.unicode;
					myMessage.setString(s);
				}					
			}
		}

		if (send)
		{
			switch (state)
			{
			case 0:
				if (socket.connect(IpAddress(s), 2000, seconds(5)) == Socket::Done)
				{
					info.setString("Enter your name");
					socket.setBlocking(false);
					state++;
				}
				else
				{
					info.setString("Error! Please, check server's IP");
				}
				break;

			case 1:
				myName = s;
				info.setString(myName + ", enter a message");
				state++;
				break;

			case 2:
				if (s == "Disconnect me")
				{
					socket.disconnect();
					window.close();
					return 0;
					break;
				}
				packet.clear();
				s.replace("\n", "");
				s = myName + ": " + s;
				packet << s;
				socket.send(packet);
				break;
			}
			s = "";
		}

		send = false;

		if (socket.receive(packet) == Socket::Done && packet)
		{
			packet >> rs;

			int i = 32;
			while (i < rs.getSize())
			{
				rs.insert(i, "\n");
				i += 33;
				allMessagesLine++;
			}
			if (allMessagesLine < 8)
			{
				allMessages.setString(allMessages.getString() + rs + "\n");		
				allMessagesLine++;
			}
			else
			{
				int p = 0;
				int j = 0;
				while (j < i)
				{
					p = allMessages.getString().find("\n", p) + 1;
					j += 33;
				} 
				allMessages.setString(allMessages.getString().substring(p) + rs + "\n");
			}

			rs = "";
		}

		window.clear();
		window.draw(info);
		window.draw(myMessage);
		window.draw(allMessages);
		window.display();
	}

	return 0;
}