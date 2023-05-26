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
int bombAmount = gridWidth * gridHight / 5, flagAmount = 0;
int openedTiles = 0;
bool bombOpen = false;


class Tile {
public:
	enum class TileState {
		open, flaged, closed
	};

	TileState tileState = Tile::TileState::closed;
	bool isBomb = false;
	int count = 0;

	sf::Sprite getSprite() {
		switch (tileState) {
			case Tile::TileState::open:
				return isBomb ? sprites[9] : sprites[count];
				break;
			case Tile::TileState::flaged:
				return sprites[11];
				break;
			case Tile::TileState::closed:
				return sprites[10];
				break;
		}
	}

	TileState toggleFlag() {
		switch (tileState) {
			case Tile::TileState::flaged:
				return tileState = TileState::closed;
				break;
			case Tile::TileState::closed:
				return tileState = TileState::flaged;
				break;
		}
	}
} grid[gridWidth][gridHight];

bool areCoordsValid(int x, int y) {
	return
		x >= 0 && x < gridWidth &&
		y >= 0 && y < gridHight;
}

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

void restart() {
	for (size_t i = 0; i < gridWidth; i++) {
		for (size_t j = 0; j < gridHight; j++) {
			grid[i][j].tileState = Tile::TileState::closed;
			grid[i][j].isBomb = false;
			grid[i][j].count = 0;
		}
	}

	firstPress = true;
	flagAmount = 0;
	openedTiles = 0;
	bombOpen = false;

	Clock.restart();
}

void openTile(int x, int y) {
	grid[x][y].tileState = Tile::TileState::open;
	openedTiles++;

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
	for (int i = 0; i < bombAmount; i++) {
		int xB = rand() % gridWidth, yB = rand() % gridHight;

		while (xB == x && yB == y || grid[xB][yB].isBomb == true) {
			xB = rand() % gridWidth;
			yB = rand() % gridHight;
		}

		grid[xB][yB].isBomb = true;

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

			if (firstPress) {
				generateBomb(x, y);
				firstPress = false;
				openTile(x, y);
				return;
			}

			if (event.key.code == sf::Mouse::Right)
				grid[x][y].toggleFlag() == Tile::TileState::flaged ? flagAmount++ : flagAmount--;

			if (event.key.code == sf::Mouse::Left && grid[x][y].tileState != Tile::TileState::flaged) {
				openTile(x, y);

				bombOpen = grid[x][y].isBomb;
			}
		}
	}
}

void gridPrintText(std::string text, sf::RenderWindow& window, float xCorrection, float yCorrection) {
	sf::Text stext(text, font, 24);
	stext.setPosition(xCorrection, yCorrection);
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
		gridPrintText("Time: " + gridTime(seconds), window, 0, gridHight * imageWidth);
	}

	sf::Sprite sprite = sprites[9];
	sprite.setPosition((gridWidth - 3) * imageWidth, gridHight * imageWidth);
	window.draw(sprite);
	gridPrintText(std::to_string(bombAmount - flagAmount), window, (gridWidth - 2) * imageWidth + 4, gridHight * imageWidth);

	window.display();
}

void gameOver() {
	int x, y;
	std::string path;

	if (openedTiles == gridWidth * gridHight - bombAmount) {
		path = "images/Nya.jpg";
		x = 1279;
		y = 719;
	} else if (bombOpen) {
		path = "images/BAH.png";
		x = 1644;
		y = 2089;
	} else {
		return;
	}

	sf::RenderWindow megumin(sf::VideoMode(x, y), "Megumin!");
	sf::Texture texture;

	texture.loadFromFile(path);

	sf::Sprite sprite;
	sprite.setTexture(texture);

	while (megumin.isOpen()) {
		sf::Event event;

		while (megumin.pollEvent(event)) {
			if (event.type == sf::Event::Closed || event.type == sf::Event::MouseButtonPressed) {
				megumin.close();
				restart();
			}
		}

		megumin.draw(sprite);
		megumin.display();
	}
}

int main() {
	setup();

	sf::RenderWindow window(sf::VideoMode(gridWidth * imageWidth, gridHight * imageWidth + 32), "Minesweeper!");

	while (window.isOpen()) {
		draw(window);
		gridEvent(window);
		gameOver();
	}
}
