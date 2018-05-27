#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
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
 private:
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
    cont=0;
    foo=0.1;
  }
  QuadTree(Rectangle *boundary, int capacity) {
    this->boundary = boundary;
    this->capacity = capacity;
    divided = false;

    rect.setSize(sf::Vector2f(boundary->getW()*2,boundary->getH()*2));
    rect.setOrigin((boundary->getW()*2)/2,(boundary->getH()*2)/2);
    rect.setPosition(boundary->getX(), boundary->getY());
    rect.setFillColor(sf::Color::White);
    rect.setOutlineThickness(1);
    rect.setOutlineColor(sf::Color::Black);
    cont=0;
    foo=0.1;
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
    for(size_t i=0; i<(size_t)PointShape.size(); i++) {
      //PointShape[i]->move(cos(foo),sin(foo));
      window.draw(*PointShape[i]);
    }
    //foo=foo+0.1;
  }

  void subdivide() {
    Rectangle *ne = new Rectangle(boundary->getX() + boundary->getW() / 2,
                                  boundary->getY() - boundary->getH() / 2,
                                  boundary->getW() / 2, boundary->getH() / 2);
    this->northeast = new QuadTree(ne,capacity);

    Rectangle *nw = new Rectangle(boundary->getX() - boundary->getW() / 2,
                                  boundary->getY() - boundary->getH() / 2,
                                  boundary->getW() / 2, boundary->getH() / 2);
    this->northwest = new QuadTree(nw,capacity);

    Rectangle *se = new Rectangle(boundary->getX() + boundary->getW() / 2,
                                  boundary->getY() + boundary->getH() / 2,
                                  boundary->getW() / 2, boundary->getH() / 2);
    this->southeast = new QuadTree(se,capacity);

    Rectangle *sw = new Rectangle(boundary->getX() - boundary->getW() / 2,
                                  boundary->getY() + boundary->getH() / 2,
                                  boundary->getW() / 2, boundary->getH() / 2);
    this->southwest = new QuadTree(sw,capacity);

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
      PointShape[cont]->setRadius(1);
      PointShape[cont]->setOrigin(1,1);
      PointShape[cont]->setFillColor(sf::Color::Red);
      PointShape[cont]->setPosition(p->getX(),p->getY());
      cont++;
      return true;
    }
    if (!this->divided) {
      this->subdivide();
    }


    if(this->northeast->insertion(p)) return true;
    else if(this->northwest->insertion(p)) return true;
    else if(this->southeast->insertion(p)) return true;
    else if(this->southwest->insertion(p)) return true;

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

  /*for (size_t i = 0; i < 500; i++) {
    p.push_back(new Point(rand()%400, rand()%400));
    std::cout<<p[i]->getX()<<"\n"<<p[i]->getY()<<std::endl;
    qt->insertion(p[i]);
  }*/

  size_t i=0;

  while (window.isOpen()) {
    sf::Vector2i MouseTrack=sf::Mouse::getPosition(window);
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      /*if(event.type == sf::Event::MouseButtonPressed) {
        p.push_back(new Point(MouseTrack.x, MouseTrack.y));
        std::cout<<p[i]->getX()<<"\n"<<p[i]->getY()<<std::endl;
        qt->insertion(p[i]);

        i++;
      }*/
    }

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      p.push_back(new Point(MouseTrack.x, MouseTrack.y));
      std::cout<<p[i]->getX()<<"\n"<<p[i]->getY()<<std::endl;
      qt->insertion(p[i]);

      i++;
    }

    window.clear(sf::Color::White);
    qt->show1(window);
    qt->show2(window);
    window.display();
  }

  return 0;
}

