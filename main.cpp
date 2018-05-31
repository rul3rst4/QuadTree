#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <iterator>
// QuadTree algorithm for collision detect.

static int width = 800;
static int height = 600;

class Point {
 public:
	Point(float x, float y) {
		this->x = x;
		this->y = y;
	}

	float getX() {
		return x;
	}
	float getY() {
		return y;
	}

 private:
	float x, y;
};

class Rectangle {
 public:
	Rectangle() {}
	Rectangle(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	void setPosition(int x, int y) {
		this->x = x;
		this->y = y;
	}

	void printCoordinates() {
		std::cout << "X: " << x << "\n"
		          << "Y: " << y << "\n"
		          << "W: " << w << "\n"
		          << "H: " << h << std::endl;
	}

	bool contains(Point *p) {
		return ((p->getX() >= this->x - this->w) && (p->getX() <= this->x + this->w) &&
		        (p->getY() >= this->y - this->h) && (p->getY() <= this->y + this->h));
	}

	bool intersects(Rectangle* range) {
		return (!(range->getX() - range->getW() > this->x + this->w ||
		          range->getX() + range->getW() < this->x - this->w ||
		          range->getY() - range->getH() > this->y + this->h ||
		          range->getY() + range->getH() < this->y - this->h));
	}

	float getX() {
		return x;
	}
	float getY() {
		return y;
	}
	float getW() {
		return w;
	}
	float getH() {
		return h;
	}

 private:
	float x, y, w, h;
};

class QuadTree {
 public:
	sf::RectangleShape rect;
	std::vector<sf::CircleShape*> PointShape;
	size_t cont;
	float foo;

 private:
	Rectangle *boundary;
	int capacity;
	std::vector<Point *> points;
	QuadTree *northwest;
	QuadTree *northeast;
	QuadTree *southeast;
	QuadTree *southwest;
	bool divided;

 public:
	QuadTree() {
		boundary = NULL;
		northwest = NULL;
		northeast = NULL;
		southeast = NULL;
		southwest = NULL;
		divided = false;
		cont = 0;
		std::cout << "Create" << std::endl;
	}
	QuadTree(Rectangle *boundary, int capacity) {
		std::cout << "Create" << std::endl;
		this->boundary = boundary;
		this->capacity = capacity;
		divided = false;

		rect.setSize(sf::Vector2f(boundary->getW() * 2, boundary->getH() * 2));
		rect.setOrigin((boundary->getW() * 2) / 2, (boundary->getH() * 2) / 2);
		rect.setPosition(boundary->getX(), boundary->getY());
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineThickness(1);
		rect.setOutlineColor(sf::Color::Black);
		cont = 0;

		boundary = NULL;
		northwest = NULL;
		northeast = NULL;
		southeast = NULL;
		southwest = NULL;
	}

	~QuadTree() {
		for(int i = 0; i < (int)PointShape.size(); i++) {
			delete PointShape[i];
		}
		PointShape.clear();

		points.clear();

		if(northwest != NULL)
			delete northwest;

		if(northeast != NULL)
			delete northeast;

		if(southeast != NULL)
			delete southeast;

		if(southwest != NULL)
			delete southwest;
	}

	void Query(Rectangle* range, std::vector<sf::CircleShape*>& found) {

		if(!this->boundary->intersects(range)) {
			return;
		} else {
			for(int i = 0; i < (int)PointShape.size(); i++) {
				if(range->contains(points[i])) {
					found.push_back(PointShape[i]);
				}
			}

			if(this->divided) {
				this->northwest->Query(range, found);

				this->northeast->Query(range, found);

				this->southwest->Query(range, found);

				this->southeast->Query(range, found);
			}
		}
	}

	void show1(sf::RenderWindow& window) {
		window.draw(rect);
		if(divided) {
			northeast->show1(window);
			northwest->show1(window);
			southeast->show1(window);
			southwest->show1(window);
		}
	}

	void show2(sf::RenderWindow& window) {
		if(divided) {
			northeast->show2(window);
			northwest->show2(window);
			southeast->show2(window);
			southwest->show2(window);
		}
		for(size_t i = 0; i < (size_t)PointShape.size(); i++) {
			window.draw(*PointShape[i]);
		}
	}

	void subdivide() {
		Rectangle *ne = new Rectangle(boundary->getX() + boundary->getW() / 2,
		                              boundary->getY() - boundary->getH() / 2,
		                              boundary->getW() / 2, boundary->getH() / 2);
		this->northeast = new QuadTree(ne, capacity);

		Rectangle *nw = new Rectangle(boundary->getX() - boundary->getW() / 2,
		                              boundary->getY() - boundary->getH() / 2,
		                              boundary->getW() / 2, boundary->getH() / 2);
		this->northwest = new QuadTree(nw, capacity);

		Rectangle *se = new Rectangle(boundary->getX() + boundary->getW() / 2,
		                              boundary->getY() + boundary->getH() / 2,
		                              boundary->getW() / 2, boundary->getH() / 2);
		this->southeast = new QuadTree(se, capacity);

		Rectangle *sw = new Rectangle(boundary->getX() - boundary->getW() / 2,
		                              boundary->getY() + boundary->getH() / 2,
		                              boundary->getW() / 2, boundary->getH() / 2);
		this->southwest = new QuadTree(sw, capacity);

		this->divided = true;
	}

	bool insertion(Point *p) {
		//boundary->printCoordinates();
		if (!this->boundary->contains(p)) {
			return false;
		}
		if ((int)this->points.size() < capacity) {
			this->points.push_back(p);
			PointShape.push_back(new sf::CircleShape);
			PointShape[cont]->setRadius(3);
			PointShape[cont]->setOrigin(3, 3);
			PointShape[cont]->setFillColor(sf::Color::Red);
			PointShape[cont]->setPosition(p->getX(), p->getY());
			cont++;
			return true;
		}
		if (!this->divided) {
			this->subdivide();
		}


		if(this->northeast->insertion(p))
			return true;
		else if(this->northwest->insertion(p))
			return true;
		else if(this->southeast->insertion(p))
			return true;
		else if(this->southwest->insertion(p))
			return true;

		return false;
	}

};

int main() {
	srand(time(NULL));
	sf::RenderWindow window(sf::VideoMode(width, height), "QuadTree");
	window.setFramerateLimit(60);

	Rectangle *boundary = new Rectangle(200, 200, 200, 200);
	QuadTree *qt = new QuadTree(boundary, 4);

	std::vector <Point*> p;

	sf::RectangleShape rect;
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineColor(sf::Color(0, 220, 0));
	rect.setOutlineThickness(2);
	rect.setSize(sf::Vector2f(107 * 2, 75 * 2));
	rect.setOrigin((107 * 2) / 2, (75 * 2) / 2);
	rect.setPosition(200, height / 2);
	Rectangle* range = new Rectangle(200, height / 2, 107, 75);

	for (size_t i = 0; i < 100; i++) {
		p.push_back(new Point(rand() % 400, rand() % 400));
		//std::cout << p[i]->getX() << "\n" << p[i]->getY() << std::endl;
		qt->insertion(p[i]);
	}

	while (window.isOpen()) {
		sf::Vector2i MouseTrack = sf::Mouse::getPosition(window);
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear(sf::Color::White);

		rect.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
		range->setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

		std::vector<sf::CircleShape*> k;
		qt->Query(range, k);
		//std::cout << (int)k.size() << std::endl;

		for(int i = 0; i < (int)k.size(); i++) {
			k[i]->setFillColor(sf::Color(0, 0, 255));
		}

		qt->show1(window);
		qt->show2(window);
		window.draw(rect);

		for(int i = 0; i < (int)k.size(); i++) {
			k[i]->setFillColor(sf::Color(255, 0, 0));
		}
		window.display();
	}

	delete boundary;
	delete qt;
	delete range;

	for(int i = 0; i < (int)p.size(); i++) {
		delete p[i];
	}

	return 0;
}
