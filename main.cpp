#include <array>
#include <SFML/Graphics.hpp>
#include <time.h>

using namespace sf;
using namespace std;

struct Tile {
	bool open = false;
	bool mine = false;
	bool flag = false;
	int count = 0;
};

const int gridWidth = 10, gridHight = 10, imageWidth = 32;
Tile grid[gridWidth][gridHight];
bool firstPress = true;

void openTile(int x, int y) {
	for (int dx : {-1, 0, 1}) {
		for (int dy : {-1, 0, 1}) {
			int x2 = x + dx, y2 = y + dy;

			if (!(x2 >= 0 && x2 < gridWidth && y2 >= 0 && y2 < gridHight))
				continue;

			grid[x][y].open = true;

			if (grid[x][y].count == 0 && grid[x2][y2].mine == false &&
				grid[x2][y2].open == false && grid[x2][y2].flag == false) {
				openTile(x2, y2);;
			}
		}
	}
}

void generateBomb(int x, int y) {
	for (int i = 0; i < gridWidth * gridHight / 5; i++) {
		int xB = rand() % gridWidth, yB = rand() % gridHight;

		while (xB == x && yB == y || grid[xB][yB].mine == true) {
			xB = rand() % gridWidth;
			yB = rand() % gridHight;
		}

		grid[xB][yB].mine = true;

		for (int dx : {-1, 0, 1}) {
			for (int dy : {-1, 0, 1}) {
				int x2 = xB + dx, y2 = yB + dy;

				if (x2 >= 0 && x2 < gridWidth && y2 >= 0 && y2 < gridHight) {
					grid[x2][y2].count++;
				}
			}
		}
	}
}

void gridEvent(RenderWindow& window) {
	Event event;

	while (window.pollEvent(event)) {
		if (event.type == Event::Closed)
			window.close();

		if (event.type == Event::MouseButtonPressed) {
			Vector2i pos = Mouse::getPosition(window);
			int x = pos.x / 32;
			int y = pos.y / 32;

			if (x >= 0 && x < gridWidth && y >= 0 && y < gridHight) {
				if (event.key.code == Mouse::Right && grid[x][y].open == false)
					grid[x][y].flag = !grid[x][y].flag;

				if (event.key.code == Mouse::Left && grid[x][y].flag == false) {
					if (firstPress) {
						generateBomb(x, y);
						firstPress = false;
					}

					openTile(x, y);

					if (grid[x][y].mine == true) {
						RenderWindow Megumin(VideoMode(1644, 2089), "Megumin!");
						Texture texture;
						texture.loadFromFile("images/BAH.png");
						Sprite sprite;
						sprite.setTexture(texture);

						while (Megumin.isOpen()) {
							Event event;

							while (Megumin.pollEvent(event))
								if (event.type == Event::Closed)
									Megumin.close();

							Megumin.draw(sprite);
							Megumin.display();
						}
					}
				}
			}
			// ≈сли координаты не попадают на игровое поле, а игра закончена, то провер€ем нажата ли кнопка ресет
			//else {
			//	if (game_over && event.key.code == Mouse::Left) {
			//		if (x >= 4 && x <= 5 && y == field_out.at(0).size()) {
			//			window.close();
			//			main();
			//		}
			//	}
			//}
		}
	}
}

void gridPrintText(string text, Font& font, RenderWindow& window, float x_correction, float y_correction) {
	Text stext(text, font, 24);
	stext.setPosition(x_correction, y_correction);
	stext.setFillColor(Color::Black);
	window.draw(stext);
}

string gridTime(int seconds) {
	string result;
	int iminutes = seconds / 60;
	int iseconds = seconds % 60;
	string sminutes = to_string(iminutes);
	string sseconds = to_string(iseconds);

	if (iminutes < 10)
		sminutes.insert(sminutes.begin(), '0');

	if (iseconds < 10)
		sseconds.insert(sseconds.begin(), '0');

	result += sminutes + ":" + sseconds;

	return result;
}

int main() {
	srand(time(0));

	RenderWindow window(VideoMode(gridWidth * imageWidth, gridHight * imageWidth + 32), "Minesweeper!");

	Texture texture;
	texture.loadFromFile("images/Tiles.png");

	Font font;
	font.loadFromFile("CyrilicOld.TTF");

	array <Sprite, 12> sprites;
	for (size_t i = 0; i < sprites.size(); i++) {
		sprites.at(i).setTexture(texture);
		sprites.at(i).setTextureRect(IntRect(i * imageWidth, 0, imageWidth, imageWidth));
	}

	Clock clock;
	clock.restart();
	int seconds = 0;

	while (window.isOpen()) {
		window.clear(Color::White);

		for (int i = 0; i < gridWidth; i++) {
			for (int j = 0; j < gridHight; j++) {
				if (grid[i][j].flag) {
					sprites[11].setPosition(i * imageWidth, j * imageWidth);
					window.draw(sprites[11]);
					continue;
				}

				if (!grid[i][j].open) {
					sprites[10].setPosition(i * imageWidth, j * imageWidth);
					window.draw(sprites[10]);
					continue;
				}

				if (grid[i][j].mine) {
					sprites[9].setPosition(i * imageWidth, j * imageWidth);
					window.draw(sprites[9]);
					continue;
				}

				sprites[grid[i][j].count].setPosition(i * imageWidth, j * imageWidth);
				window.draw(sprites[grid[i][j].count]);
			}
		}

		if (seconds != clock.getElapsedTime().asSeconds()) {
			seconds = static_cast<int>(clock.getElapsedTime().asSeconds());
			gridPrintText("Time: " + gridTime(seconds), font, window, 0, gridHight * 32.f);
		}

		gridEvent(window);

		window.display();
	}
}
