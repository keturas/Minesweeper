#include <SFML/Graphics.hpp>
#include <time.h>

const int gridWidth = 10, gridHight = 10, imageWidth = 32;
const int spritesAmount = 12;

sf::Texture texture;
sf::Font font;
sf::Sprite sprites[spritesAmount];
sf::Clock Clock;

int seconds = 0;
bool firstPress = true;

class Tile {
private:
public:
	enum class TileState {
		open, flaged, closed
	};

	TileState tileState = Tile::TileState::closed;
	bool isMine = false;
	int count = 0;

	sf::Sprite getSprite() {
		switch (tileState) {
			case Tile::TileState::open:
				return isMine ? sprites[9] : sprites[count];
				break;
			case Tile::TileState::flaged:
				return sprites[11];
				break;
			case Tile::TileState::closed:
				return sprites[10];
				break;
		}
	}

	void toggleFlag() {
		switch (tileState) {
			case Tile::TileState::flaged:
				tileState = TileState::closed;
				break;
			case Tile::TileState::closed:
				tileState = TileState::flaged;
				break;
		}
	}
} grid[gridWidth][gridHight];

void setup() {
	srand(time(0));

	Clock.restart();

	texture.loadFromFile("images/Tiles.png");
	font.loadFromFile("CyrilicOld.TTF");

	for (size_t i = 0; i < spritesAmount; i++) {
		sprites[i].setTexture(texture);
		sprites[i].setTextureRect(sf::IntRect(i * imageWidth, 0, imageWidth, imageWidth));
	}
}

bool areCoordsValid(int x, int y) {
	return
		x >= 0 && x < gridWidth &&
		y >= 0 && y < gridHight;
}

void openTile(int x, int y) {
	grid[x][y].tileState = Tile::TileState::open;

	if (grid[x][y].count != 0)
		return;

	for (int dx : {-1, 0, 1}) {
		for (int dy : {-1, 0, 1}) {
			int x2 = x + dx, y2 = y + dy;

			if (!areCoordsValid(x2, y2))
				continue;

			if (grid[x2][y2].tileState == Tile::TileState::closed) {
				openTile(x2, y2);;
			}
		}
	}
}

void generateBomb(int x, int y) {
	for (int i = 0; i < gridWidth * gridHight / 5; i++) {
		int xB = rand() % gridWidth, yB = rand() % gridHight;

		while (xB == x && yB == y || grid[xB][yB].isMine == true) {
			xB = rand() % gridWidth;
			yB = rand() % gridHight;
		}

		grid[xB][yB].isMine = true;

		for (int dx : {-1, 0, 1}) {
			for (int dy : {-1, 0, 1}) {
				int x2 = xB + dx, y2 = yB + dy;

				if (areCoordsValid(x2, y2)) {
					grid[x2][y2].count++;
				}
			}
		}
	}
}

void Megumin() {
	sf::RenderWindow megumin(sf::VideoMode(1644, 2089), "Megumin!");
	sf::Texture texture;
	texture.loadFromFile("images/BAH.png");
	sf::Sprite sprite;
	sprite.setTexture(texture);

	while (megumin.isOpen()) {
		sf::Event event;

		while (megumin.pollEvent(event))
			if (event.type == sf::Event::Closed)
				megumin.close();

		megumin.draw(sprite);
		megumin.display();
	}
}

void gridEvent(sf::RenderWindow& window) {
	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();

		if (event.type == sf::Event::MouseButtonPressed) {
			sf::Vector2i pos = sf::Mouse::getPosition(window);
			int x = pos.x / 32;
			int y = pos.y / 32;

			if (!areCoordsValid(x, y) || grid[x][y].tileState == Tile::TileState::open)
				return;

			if (event.key.code == sf::Mouse::Right)
				grid[x][y].toggleFlag();

			if (event.key.code == sf::Mouse::Left && grid[x][y].tileState != Tile::TileState::flaged) {
				if (firstPress) {
					generateBomb(x, y);
					firstPress = false;
				}

				openTile(x, y);

				if (grid[x][y].isMine == true) {
					Megumin();					
				}
			}
		}
	}
}

void gridPrintText(std::string text, sf::RenderWindow& window, float xÑorrection, float yÑorrection) {
	sf::Text stext(text, font, 24);
	stext.setPosition(xÑorrection, yÑorrection);
	stext.setFillColor(sf::Color::Black);
	window.draw(stext);
}

std::string gridTime(int seconds) {
	std::string result;
	int iminutes = seconds / 60;
	int iseconds = seconds % 60;
	std::string sminutes = std::to_string(iminutes);
	std::string sseconds = std::to_string(iseconds);

	if (iminutes < 10)
		sminutes.insert(sminutes.begin(), '0');

	if (iseconds < 10)
		sseconds.insert(sseconds.begin(), '0');

	result += sminutes + ":" + sseconds;

	return result;
}

void draw(sf::RenderWindow& window) {
	window.clear(sf::Color::White);

	for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHight; j++) {
			sf::Sprite sprite = grid[i][j].getSprite();
			sprite.setPosition(i * imageWidth, j * imageWidth);
			window.draw(sprite);
		}
	}

	if (seconds != Clock.getElapsedTime().asSeconds()) {
		seconds = static_cast<int>(Clock.getElapsedTime().asSeconds());
		gridPrintText("Time: " + gridTime(seconds), window, 0, gridHight * 32.f);
	}

	window.display();
}

int main() {
	setup();

	sf::RenderWindow window(sf::VideoMode(gridWidth * imageWidth, gridHight * imageWidth + 32), "Minesweeper!");

	while (window.isOpen()) {
		draw(window);
		gridEvent(window);
	}
}
