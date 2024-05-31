#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <list>
#include <queue>
#include <iostream>


using namespace sf;
using namespace std;


class Ring;

class Assessor
{
public:
    static bool ringInEclipse;
    static bool ringPerfect;
    static Ring* eclipsedRing;

};
bool Assessor::ringInEclipse = false;
bool Assessor::ringPerfect = false;
Ring* Assessor::eclipsedRing = nullptr;


class ColorPalette
{
public:
    static Color background;
    static Color shine;
    static Color gold;
    static Color player;
    static Color red;
};
Color ColorPalette::background = Color(11, 7, 18, 255);
Color ColorPalette::shine = Color(226, 215, 144, 255);  //224, 206, 110, 255
Color ColorPalette::gold = Color(183, 150, 52, 255);
Color ColorPalette::player = Color(11, 164, 72, 255);
Color ColorPalette::red = Color(224, 45, 57, 225);


static int playerScore = 0;


class Ring
{
    CircleShape myShape;

    bool eclipsed = false;
    bool perfect = false;
    bool lastCall = false;
    bool lapsed = false;

public:

    bool operator==(const Ring& other) const
    {
        return myShape.getScale().x == other.myShape.getScale().x;
    }

    bool operator!=(const Ring& other) const
    {
        return !(*this == other);
    }

    explicit Ring(float locX, float locY, float iniScale)    //constructor for shades
    {
        myShape = CircleShape(160);
        myShape.setOrigin(Vector2f(160, 160));
        myShape.setFillColor(Color::Transparent);
        myShape.setOutlineThickness(-10);
        myShape.setOutlineColor(ColorPalette::gold);
        myShape.setPosition(Vector2f(locX, locY));
        myShape.scale(iniScale, iniScale);
    }

    explicit Ring(float locX, float locY)                     //constructor for tracker
    {
        myShape = CircleShape(160);
        myShape.setOrigin(Vector2f(160, 160));
        myShape.setFillColor(Color::Transparent);
        myShape.setOutlineThickness(-5);
        myShape.setOutlineColor(ColorPalette::player);
        myShape.setPosition(Vector2f(locX, locY));
    }

    CircleShape GetDrawable()
    {
        return myShape;
    }

    bool ProgressScale(float increment)   //returns false if the ring is ready to be deleted
    {
        if (eclipsed)
        {
            if (perfect)
            {
                if (lastCall)
                {
                    if (lapsed)
                    {
                        if (myShape.getScale().x > 1.8) return false;
                    }
                    else
                    {
                        if (myShape.getScale().x > 1.1)
                        {
                            lapsed = true;
                            Assessor::ringInEclipse = false;
                            Assessor::eclipsedRing = nullptr;
                        }
                    }
                }
                else
                {
                    if (myShape.getScale().x > 1.05)
                    {
                        lastCall = true;                    //if (Assessor::ringPerfect == false) cout << "wut";
                        Assessor::ringPerfect = false;
                    }
                }
            }
            else
            {
                if (myShape.getScale().x > 0.96)
                {
                    perfect = true;                         //if (Assessor::ringPerfect == true) cout << "shling ";
                    Assessor::ringPerfect = true;
                }
            }
        }
        else
        {
            if (myShape.getScale().x > 0.9)
            {
                eclipsed = true;                            //if (Assessor::ringInEclipse == true) cout << "bump ";
                Assessor::ringInEclipse = true;
                Assessor::eclipsedRing = this;                                    
            }
        }

        myShape.scale(increment, increment);
        return true;
    }
};


class Star
{
    ConvexShape myShape;
    ConvexShape perfectShape;

public:

    Star()
    {
        myShape = ConvexShape(10);
        CircleShape stencil(50, 5);
        myShape.setPoint(0, stencil.getPoint(0));
        myShape.setPoint(2, stencil.getPoint(1));
        myShape.setPoint(4, stencil.getPoint(2));
        myShape.setPoint(6, stencil.getPoint(3));
        myShape.setPoint(8, stencil.getPoint(4));
        stencil = CircleShape(27, 5);
        Vector2f offset(23, 77);
        myShape.setPoint(5, Vector2f(stencil.getPoint(0).x, -stencil.getPoint(0).y) + offset);
        myShape.setPoint(3, Vector2f(stencil.getPoint(1).x, -stencil.getPoint(1).y) + offset);
        myShape.setPoint(1, Vector2f(stencil.getPoint(2).x, -stencil.getPoint(2).y) + offset);
        myShape.setPoint(9, Vector2f(stencil.getPoint(3).x, -stencil.getPoint(3).y) + offset);
        myShape.setPoint(7, Vector2f(stencil.getPoint(4).x, -stencil.getPoint(4).y) + offset);
        myShape.setOrigin(50, 50);
        myShape.setPosition(200, 200);
        myShape.setFillColor(Color::Transparent);

        perfectShape = ConvexShape(myShape);

        myShape.setOutlineThickness(-3);
        myShape.setOutlineColor(ColorPalette::gold);
    }

    void Shine(bool perfect)
    {
        myShape.setFillColor(ColorPalette::gold);
        if (perfect) perfectShape.setFillColor(ColorPalette::shine);
    }

    void DimShine(float extent)
    {
        if (myShape.getFillColor().a > 0)
        {
            myShape.setFillColor(myShape.getFillColor() - Color(0, 0, 0, roundf(250 * extent)));

            if (perfectShape.getFillColor().a > 0)
            {
                perfectShape.setFillColor(perfectShape.getFillColor() - Color(0, 0, 0, roundf(1000 * extent)));
            }
        }
    }

    void GetDrawn(RenderWindow& renderer)
    {
        renderer.draw(myShape);
        renderer.draw(perfectShape);
    }
};


class Bar
{
    RectangleShape scoreBack;
    RectangleShape redZone;
    RectangleShape scoreBar;

    queue<RectangleShape*> drawables;

    float score = 0;
    float reserve = 15;                      //negative points allowed before game over
    const int ringsTotal = 452;


public:

    Bar(float viewScale)
    {
        scoreBack = RectangleShape(Vector2f(350, 13));
        scoreBack.setOrigin(175, 6.5f);
        scoreBack.setFillColor(ColorPalette::background);
        scoreBack.setOutlineThickness(2);
        scoreBack.setOutlineColor(ColorPalette::shine - Color(0, 0, 0, 113));
        scoreBack.setPosition(viewScale / 2, 20);

        redZone = RectangleShape(Vector2f(350 * reserve / (ringsTotal + reserve), 13));
        redZone.setOrigin(redZone.getSize().x / 2, 6.5f);
        redZone.setFillColor(ColorPalette::red);
        redZone.setPosition(viewScale / 2, 20);

        scoreBar = RectangleShape(Vector2f( 350*reserve/(ringsTotal+reserve), 13));
        scoreBar.setOrigin(scoreBar.getSize().x / 2, 6.5f);
        scoreBar.setFillColor(ColorPalette::shine);
        scoreBar.setPosition(viewScale / 2, 20);

        RectangleShape* ptr1 = &scoreBack;
        drawables.emplace(ptr1);
        RectangleShape* ptr2 = &redZone;
        drawables.emplace(ptr2);
        RectangleShape* ptr3 = &scoreBar;
        drawables.emplace(ptr3);
    }

    bool AddPoints(float points)		    //returns whether the score remains above game over value
    {
        score += points;
        if (score < -reserve) return false;
        scoreBar.setScale(Vector2f((reserve + score) / reserve, 1));
        return true;
    }

    int GetScore()
    {
        return (int) roundf(score);
    }

    void GetDrawn(RenderWindow& renderer)
    {
        renderer.draw(scoreBack);
        renderer.draw(redZone);
        renderer.draw(scoreBar);
    }

};

int menu(RenderWindow& renderer)
{
    RectangleShape background = RectangleShape(Vector2f(renderer.getSize().x, renderer.getSize().y));
    background.setFillColor(ColorPalette::background);
    RectangleShape startButton = RectangleShape(Vector2f(80, 30));
    startButton.setOrigin(40, 15);
    startButton.setFillColor(ColorPalette::shine);
    RectangleShape helpButton(startButton);
    RectangleShape quitButton = (startButton);

    startButton.setPosition(renderer.getSize().x / 2, renderer.getSize().y / 2.f);
    helpButton.setPosition(renderer.getSize().x / 2, startButton.getPosition().y + 40);
    quitButton.setPosition(renderer.getSize().x / 2, startButton.getPosition().y + 80);

    Font menuFont;
    if (!menuFont.loadFromFile("menu_font.ttf"))
    {
        cout << "no font for u :<";
    }

    Text impleHythm = Text(" imple  hythm", menuFont, 40);
    impleHythm.setOrigin(impleHythm.getLocalBounds().width / 2, 0);
    impleHythm.setPosition(renderer.getSize().x / 2, renderer.getSize().x / 3 - 80);
    
    Text sR = Text(impleHythm);
    sR.setString("S      R     ");

    impleHythm.setFillColor(ColorPalette::shine);
    sR.setFillColor(ColorPalette::player);

    Text playText = Text("play", menuFont, 20);
    playText.setOrigin(20, 15);
    playText.setFillColor(ColorPalette::background);
    Text quitText = Text(playText);
    quitText.setString("quit");
    Text helpText = Text(playText);
    helpText.setString("help");
    Text advice = Text("Press Space. Good Luck.", menuFont, 20);
    advice.setOrigin(0, 20);
    advice.setFillColor(Color::Transparent);

    Text scoreText;
    if (playerScore > 0)
    {
        scoreText = Text(to_string(playerScore), menuFont, 40);
        scoreText.setOrigin(scoreText.getLocalBounds().width / 2, 0);
        scoreText.setPosition(renderer.getSize().x / 2, renderer.getSize().x / 3);
        scoreText.setFillColor(ColorPalette::gold);
    }

    playText.setPosition(startButton.getPosition());
    quitText.setPosition(quitButton.getPosition());
    helpText.setPosition(helpButton.getPosition());
    advice.setPosition(3, renderer.getSize().y - 3);

    while (renderer.isOpen())
    {
        sf::Event event;
        while (renderer.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                renderer.close();
            }

            if (event.type == Event::MouseButtonPressed)
            {
                if (startButton.getLocalBounds().contains(Vector2f(event.mouseButton.x, event.mouseButton.y) + startButton.getOrigin() - startButton.getPosition()))
                {
                    renderer.clear();
                    return 1;
                }
                else if (helpButton.getLocalBounds().contains(Vector2f(event.mouseButton.x, event.mouseButton.y) + helpButton.getOrigin() - helpButton.getPosition())) advice.setFillColor(ColorPalette::gold);
                else if (quitButton.getLocalBounds().contains(Vector2f(event.mouseButton.x, event.mouseButton.y) + quitButton.getOrigin() - quitButton.getPosition())) return 0;
            }
        }

        renderer.clear();
        renderer.draw(background);
        renderer.draw(sR);
        renderer.draw(impleHythm);
        renderer.draw(startButton);
        renderer.draw(quitButton);
        renderer.draw(helpButton);
        renderer.draw(helpText);
        renderer.draw(quitText);
        renderer.draw(playText);
        if (playerScore > 0) renderer.draw(scoreText);
        if (advice.getFillColor().a > 0)
        {
            advice.setFillColor(advice.getFillColor() - Color(0, 0, 0, 1));
            renderer.draw(advice);
        }
        renderer.display();
    }
    return 0;
}


int main()
{
    float viewScale = 400.f;

    RenderWindow gameView(VideoMode(viewScale, viewScale), "SimpleRhythm");
    gameView.setFramerateLimit(100);

    SoundBuffer audioPlayer;
    audioPlayer.loadFromFile("beat.FLAC");
    Sound beat;
    beat.setBuffer(audioPlayer);

    Music rave;
    if (!rave.openFromFile("crab_rave.FLAC")) {
        cout << "no rave :<";
    }
    rave.setVolume(50.f);

    RectangleShape background = RectangleShape(Vector2f(viewScale, viewScale));
    background.setFillColor(ColorPalette::background);
    Star tokenStar;
    Ring tracker{ viewScale / 2, viewScale / 2 };

    if (menu(gameView) == 0) gameView.close();

                       
    while (gameView.isOpen())               //ENTRY POINT FROM MENU. GAME START (RE)GENERATES HERE
    {
        bool initialized = false;

        Clock gameStartTimer;
        Clock frameDeltaTimer;
        float frameDeltaT = 0;

        std::list<Ring*> shades = list<Ring*>();
        Bar scoreBar = Bar(viewScale);

        queue<float> pattern;

#ifdef NDEBUG
        float oneBeatT = 0.47928;     //Release Value
        pattern.push(6.1 * oneBeatT);
#else
        float oneBeatT = 0.47928;       //Debug Value
        pattern.push(6.1 * oneBeatT);
        //for (int i = 350; i > 0; i--)pattern.push(oneBeatT);  //calibrator
#endif

        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);

        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);

        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);

        //drop

        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);

        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5 * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5 * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5 * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.75 * oneBeatT);


        //transition

        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(2.f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(1.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);

        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.75f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);

        //drop2
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5 * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5 * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5 * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.75 * oneBeatT);

        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.25f * oneBeatT);
        pattern.push(0.5f * oneBeatT);

        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(1.5 * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(1.5 * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(1.5 * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(oneBeatT);
        pattern.push(1.5 * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);
        pattern.push(0.5f * oneBeatT);

        pattern.push(20 * oneBeatT);

        float patternProgress = pattern.front();

        rave.play();
        rave.setPlayingOffset(seconds(25.f));

        initialized = true;
        while (initialized)
        {
            sf::Event event;
            while (gameView.pollEvent(event))
            {
                if (event.type == Event::Closed)
                {
                    rave.stop();
                    gameView.close();
                }

                if (event.type == Event::KeyPressed || event.type == Event::KeyReleased)
                {
                    if (Keyboard::isKeyPressed(Keyboard::Space))
                    {
                        beat.play();
                        if (Assessor::ringInEclipse)
                        {
                            tokenStar.Shine(Assessor::ringPerfect);
                            if (Assessor::ringPerfect)
                            {
                                scoreBar.AddPoints(1);
                                Assessor::ringPerfect = false;
                            }
                            else scoreBar.AddPoints(0.8f);
                            shades.remove(Assessor::eclipsedRing);
                            Assessor::eclipsedRing = nullptr;
                            Assessor::ringInEclipse = false;
                        }
                        else if (!scoreBar.AddPoints(-0.5f))
                        {
                            initialized = false;                           //requires cleanup
                        }
                    }
                }
            }

            if (!shades.empty())
            {
                Ring* binptr = nullptr;
                for (Ring* o : shades)
                {
                    if (!(*o).ProgressScale(1 + 1.6 * frameDeltaT))
                    {
                        binptr = o;
                        if (!scoreBar.AddPoints(-1))
                        {
                            initialized = false;                           //requires cleanup
                        }
                    }
                }
                if (binptr != nullptr) shades.remove(binptr);
                delete binptr;
            }

            tokenStar.DimShine(frameDeltaT);

            gameView.clear();
            gameView.draw(background);
            scoreBar.GetDrawn(gameView);
            for (Ring* o : shades)
            {
                gameView.draw((*o).GetDrawable());
            }
            gameView.draw(tracker.GetDrawable());
            tokenStar.GetDrawn(gameView);

            /*Time since last frame is read.
                 Computations sensitive to precision of this value are performed
              Frame time clock is reset. */
            frameDeltaT = frameDeltaTimer.getElapsedTime().asSeconds();
            patternProgress -= frameDeltaT;
            if (patternProgress < 0)
            {
                pattern.pop();
                if (!pattern.empty())
                {
                    Ring* ptr = new Ring(viewScale / 2, viewScale / 2, 0.2f);
                    shades.emplace_back(ptr);
                    patternProgress += pattern.front();              //retaining of remainder from imperfect beat  
                }                                                   //and adding to it may result in less variance
                else                                               //between beats (when average of imperfections
                {                                                 //is not factored in in the oneBeatT)
                    playerScore = scoreBar.GetScore();
                    initialized = false;
                }
            }
            frameDeltaTimer.restart();
            gameView.display();
        }
        rave.stop();
        if (menu(gameView) == 0) gameView.close();
        shades.clear();
    }
    rave.stop();
    return 0;
}
