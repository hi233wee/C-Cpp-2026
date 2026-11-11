#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

inline sf::Font FONT;
inline constexpr char PATH[] = "simkai.ttf";

inline void load_font()
{
	if (!FONT.openFromFile(PATH))
	{
		std::cerr << "Failed to load font from " << PATH << std::endl;
	}
}

inline void draw_text(sf::RenderWindow& window, const char* text)
{
	sf::Text sf_text(FONT, text, 27);
	sf_text.setFillColor(sf::Color::Red);
	sf_text.setPosition(sf::Vector2f(7, 7));
	sf_text.setOutlineColor(sf::Color::Black);
	sf_text.setOutlineThickness(2);

	window.draw(sf_text);
}
