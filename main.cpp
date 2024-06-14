#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <string>
#include <vector>


class Block : public sf::RectangleShape
{
public:
    float width, height;
    int remainingLives;
    std::string signature;
    Block(int remainingLives)
        :
          width(30.0f)
        , height(30.0f)
        , remainingLives(remainingLives)
        , sf::RectangleShape(sf::Vector2f(30.0f, 30.0f))
        , signature("I'm a block")
    {
        //setFillColor(sf::Color::Blue);
        if (remainingLives > 3 || remainingLives == 0)
        {
            this-> remainingLives = 4;
        }
        updateColor();
    }
    void destroy()
    {
        if (remainingLives > 0)
        {
            remainingLives = 0;
            updateColor();
        }

        if (remainingLives == 0)
        {
            std::cout << "Block destroyed!" << std::endl;
            setSize(sf::Vector2f(0, 0));
        }
    }
    void decreaseHP()
    {
        remainingLives--;
        std::cout << "Block hit, remaining block hp: " << remainingLives << "\n";
    }

    void updateColor()
    {
        switch (remainingLives)
        {
        case(1):
            setFillColor(sf::Color(234, 209, 80)); // Yellow
            break;
        case(2):
            setFillColor(sf::Color(249, 151, 89)); // Orange
            break;
        case(3):
            setFillColor(sf::Color(253, 118, 118)); // Red
            break;
        default:
            setFillColor(sf::Color(159, 63, 197)); // Purple
            break;
        }
    }
};

class Player : public sf::RectangleShape {
public:
    float playerSpeed, width, height;

    Player() 
        :
          width(150.0f)
        , height(10.0f)
        , playerSpeed(7.0f)
        , sf::RectangleShape(sf::Vector2f(150.0f, 10.0f))
    {
    }

    int getX()
    {
        return getPosition().x;
    }
    int getY()
    {
        return getPosition().y;
    }
    void moveLeft()
    {
        move(-playerSpeed, 0);
    }

    void moveRight()
    {
        move(playerSpeed, 0);
    }
    
    int whichPartWasTouched(int x)
    {
        int px = getX();
        if (x < px || x > px + width)
            return 0;
        else if( x >= px && x <= px+width/5)
            return 1;
        else if (x > px + width / 5 && x <= px + 2*(width / 5))
            return 2;
        else if (x > px + 2 * (width / 5) && x <= px + 3 * (width / 5))
            return 3;
        else if (x > px + 3 * (width / 5) && x <= px + 4 * (width / 5))
            return 4;
        else if (x > px + 4 * (width / 5) && x <= px + width )
            return 5;
        else
            return -1;
    }

 
private:

};

class Ball : public sf::CircleShape {
public:
    float speed, speedX, speedY;
    int radius;
    bool isFree;
    Ball()
        :
          speed(0.0f)
        , speedX(speed)
        , speedY(speed)
        , radius(10.0f)
        , isFree(false)
        , sf::CircleShape(10.0f)
    {
        setFillColor(sf::Color::Blue);
    }

    int getX()
    {
        return getPosition().x;
    }
    int getY()
    {
        return getPosition().y;
    }
    void setSpeedX(float newSpeed)
    {
        speedX = newSpeed;
    }

    void setSpeedY(float newSpeed)
    {
        speedY = newSpeed;
    }
    void setFreeState(bool state)
    {
        isFree = state;
    }
    void invertSpeedX()
    {
        speedX = -speedX;
    }

    void invertSpeedY()
    {
        speedY = -speedY;
    }

    void move()
    {
        setPosition(getPosition() + sf::Vector2f(speedX, speedY));
    }

    void move(sf::Vector2f point)
    {
        setPosition(point);
    }
    void moveByVector(sf::Vector2f vector)
    {
        setPosition(getPosition()+ vector);
    }


private:

};

class Game {
public:
    bool ball_free;
    Game(sf::RenderWindow& gameWindow, int level)
        : window(gameWindow),
        level(level),
        player1(),
        circle(),
        has_colided_with_player(false),
        player_lost_point(false),
        ball_free(false),
        pScore(0), 
        remaining_lives(3),
        game_paused(false),
        game_finished(false)
    {
        player1.setPosition(windowWidth / 2 - player1.width/2, windowHeight - 30);
        circle.setPosition(player1.getX()+player1.width/2-circle.radius, player1.getY()-player1.height*2);
        updateTextScore(pScore);
        if (!font.loadFromFile("fonts/tech.ttf"))
        {
            std::cerr << "Could not load the font";
            exit(-1);
        }
        renderBlocks();
        drawRemainingLives(remaining_lives);
        textScore.setFont(font);
        endText.setFont(font);
        textScore.setCharacterSize(30);
        endText.setCharacterSize(50);
        textScore.setPosition(10, 10);
    }
    void updateTextScore(int score)
    {
        scoreString = "SCORE: " + std::to_string(pScore);
        textScore.setString(scoreString);
    }
    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) 
            {
                handleEvents(event);

            }
            if (!game_finished)
            {
                checkBlockCollision();
                update();
            }
            render();
        }
    }

private:
    void handleEvents(sf::Event event)
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (circle.isFree)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                player1.moveLeft();


            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                player1.moveRight();
        }
        else if (!circle.isFree && !game_finished)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                player1.moveLeft();
                circle.moveByVector(sf::Vector2f(-player1.playerSpeed, 0));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                player1.moveRight();
                circle.moveByVector(sf::Vector2f(player1.playerSpeed, 0));
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                if (circle.getX() > 0 && circle.getX() + 2* circle.radius < windowWidth)
                { 
                    circle.setSpeedY(4.5f);
                    circle.setFreeState(true);
                }
            }

        }
    }
    void drawRemainingLives(int remaining_lives)
    {
        
        for (size_t i = 0; i < remaining_lives; i++)
        {
            sf::CircleShape health_point;
            health_point.setRadius(10.0f);
            health_point.setFillColor(sf::Color::Red);
            float x_position = windowWidth - 100 - (i * 25.0f);
            health_point.setPosition(sf::Vector2f(x_position, 20.0));
            hp.push_back(health_point);
        }
    }

    void update() {
        cx = circle.getPosition().x;
        cy = circle.getPosition().y;
        px = player1.getPosition().x;
        py = player1.getPosition().y;

        player_lost_point = cy >= windowHeight + 2 * circle.radius && circle.isFree;
        if (player_lost_point) {
            resetPosition();
            pScore -= 100;
            remaining_lives -= 1;
            hp.pop_back();
            if (remaining_lives == 0) {
                
                finishGame(false);
            }
            return;
        }
        else if (circle.isFree) 
        {
            bounceBall();
            circle.move();
        }
        updateTextScore(pScore);
        

    }

    void renderBlocks()
    {
        int rowPixels;
        int blockLives;
        for (int i = 0; i < 5; i++)
        {
            //if (i == 3) blockLives = 2;
            //else blockLives = 1;
            rowPixels = 50 + 35 * i;
            for (int j = 0; j < 20; j++)
            {
                if (level == 1) blockLives = 1;
                else if (level == 2) blockLives = i % 2 + 1;
                else  blockLives = (i + j) % 4;
                Block block(blockLives);
                block.setPosition(50 + j * 35, rowPixels);
                blocks.push_back(block);
            }
        }
    }

    void resetPosition()
    {
        circle.setPosition(player1.getX() + player1.width / 2 - circle.radius, player1.getY() - player1.height * 2);
        circle.setSpeedX(0.0f);
        circle.setSpeedY(0.0f);

        circle.setFreeState(false);
    }
    void finishGame(bool playerWon)
    {
        circle.setPosition(windowWidth / 2, 0 - circle.radius * 3);
        //circle.setPosition(player1.getX() + player1.width / 2 - circle.radius, player1.getY() - player1.height * 2);
        circle.setSpeedX(0);
        circle.setSpeedY(0);
        if (playerWon)
        {
            endString = "YOU WON";
            endText.setString(endString);
        }
        else
        {
            endString = "YOU LOST";
            endText.setString(endString);
        }


        game_finished = true;
        sf::FloatRect textRect = endText.getLocalBounds();
        endText.setOrigin(textRect.width / 2.0f, textRect.height / 2.0f);
        endText.setPosition(windowWidth / 2.0f, windowHeight / 2.0f);
    }

    void render() {
        window.clear();
        for (auto& block : blocks)
        {
            block.updateColor();
            window.draw(block);
        }
        for (auto& healthPoint : hp)
        {
            window.draw(healthPoint);
        }
        window.draw(circle);
        window.draw(player1);
        window.draw(textScore);
        window.draw(endText);
        window.display();
    }

    void checkBlockCollision() {
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i].getGlobalBounds().intersects(circle.getGlobalBounds())) {
                blockHit(blocks[i], i);
                return;
            }
        }
    }

    void blockHit(Block& block, size_t index) {
        block.decreaseHP();
        sf::FloatRect ballBounds = circle.getGlobalBounds();
        sf::FloatRect blockBounds = block.getGlobalBounds();

        float overlapLeft = ballBounds.left + ballBounds.width - blockBounds.left;
        float overlapRight = blockBounds.left + blockBounds.width - ballBounds.left;
        float overlapTop = ballBounds.top + ballBounds.height - blockBounds.top;
        float overlapBottom = blockBounds.top + blockBounds.height - ballBounds.top;

        bool ballFromLeft = overlapLeft < overlapRight;
        bool ballFromTop = overlapTop < overlapBottom;

        float minOverlapX = ballFromLeft ? overlapLeft : overlapRight;
        float minOverlapY = ballFromTop ? overlapTop : overlapBottom;

        if (minOverlapX < minOverlapY) {
            circle.invertSpeedX();
        }
        else {
            circle.invertSpeedY();
        }
        if (block.remainingLives <= 0) {
            pScore += 10;
            block.destroy();
            blocks.erase(blocks.begin() + index);
            std::cout << "There are " << blocks.size() << " blocks in the game\n";
            checkIfGameFinished();
        }
    }

    void checkIfGameFinished()
    {
        if (blocks.size() <= 0)
        {
            game_finished = true;
            finishGame(true);

        }
        
    }
    void bounceFromPlayer()
    {
        float speedX, speedY;
        int touchedPart = player1.whichPartWasTouched(cx);
        switch (touchedPart)
        {
        case 1:
            speedY = -2.0f;
            speedX = -6.7f;
            break;
        case 2:
            speedY = -4.0f;
            speedX = -5.8f;
            break;
        case 3:
            speedY = -7.0f;
            speedX = 0.0f;
            break;
        case 4:
            speedY = -4.0f;
            speedX = 5.8f;
            break;
        case 5:
            speedY = -2.0f;
            speedX = 6.7f;
            break;
        default:
            speedY = circle.speedY;
            speedX = circle.speedX;
            break;
        }
            circle.setSpeedX(speedX);
            circle.setSpeedY(speedY);
        

    }

    void bounceBall() {
        has_colided_with_player = false;
        bool touching_player = cx <= px + player1.width
            && cx >= px
            && cy + circle.radius >= py
            && cy + circle.radius <= py + player1.height;

        bool touching_right_wall = cx + 2 * circle.radius > windowWidth;
        bool touching_left_wall = cx < 0;
        // Left - Right Bounce
        if ((touching_left_wall) || touching_right_wall) {
            circle.invertSpeedX();
        }

        // Top - Down bounce
        bool touching_top_wall = cy < 0;
        bool touching_low_wall = cy + 2 * circle.radius > windowHeight;
        if (touching_top_wall)
        {
            circle.invertSpeedY();
            has_colided_with_player = false;
        }
        if (touching_player && !has_colided_with_player) {
            bounceFromPlayer();
            pScore--;
        }
    }

    // Game constants and variables

    sf::RenderWindow& window;
    const int windowHeight = window.getSize().y;
    const int windowWidth = window.getSize().x;
    Player player1;
    int remaining_lives;
    Ball circle;
    int cx, cy, px, py;
    bool has_colided_with_player;
    bool player_lost_point;
    bool game_paused;
    bool game_finished;
    float ball_last_known_speed_x;
    float ball_last_known_speed_y;
    sf::Font font;
    sf::Text textScore, endText;
    int pScore;
    std::string scoreString, endString;
    std::vector<Block> blocks;
    std::vector<sf::CircleShape> hp;
    int level;
};

int main() {
    std::string levelDescriptions[3] = {"easy", "mid", "hard"};
    int level;
    do 
    {
        std::cout << "Choose level:\n[1]" << levelDescriptions[0] << "\n[2]"
            << levelDescriptions[1] << "\n[3]" << levelDescriptions[2] << "\n";
        std::cin >> level;
        std::cout << "Level chosen: " << levelDescriptions[level-1] << "\n";
    } while (level < 1 || level > 3);
 
    sf::RenderWindow window(sf::VideoMode(800, 700), "Brick Braker - Dominik Klisiewicz");
    window.setFramerateLimit(60);
    Game game(window, level);
    game.run();
    return 0;
}
