#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <SFML/Audio.hpp>
#include <fstream>

#include <player.h>
#include <enemy.h>
#include <bullet.h>

// Setting class
class Setting : public sf::Sprite
{
private:
    bool check;

public:
    // Constructor
    Setting(sf::Texture &texture, bool checkval)
    {
        this->setTexture(texture);
        this->setScale(0.75, 0.75);
        check = checkval;
        setTextureBox();
    }
    // Getter for check
    bool getCheck()
    {
        return check;
    }
    // Setter for check
    void setCheck(bool checkval)
    {
        check = checkval;
        setTextureBox();
    }

private:
    // Set texture rect
    void setTextureBox()
    {
        if(check == true)
        {
            this->setTextureRect(sf::IntRect(0, 0, 68, 64));
        }
        else
        {
            this->setTextureRect(sf::IntRect(0, 68, 68, 64));
        }
    }
};

void makeEnemy(sf::Texture &enemy_texture, sf::Vector2i windowSize, int enemy_speed, Player &player, std::vector<Enemy> &enemies)
{
    Enemy enemy(enemy_texture, windowSize);

    // Set speed and random direction
    bool direction = std::rand() % 2;
    if(direction) enemy.setSpeed(enemy_speed);
    else enemy.setSpeed(-enemy_speed);

    enemy.setyLimit(player.getPlayerPosition().y);
    enemy.setVshift(52); // Change value to change difficulty // Ideal Values(factors of 416 - 16, 26, 32, 52, 104)
    enemies.emplace_back(enemy);
}

void write_highscore(int highscore)
{
    std::ofstream ofile("highscore.txt");
    if (!ofile.is_open()) std::cout << "File was not opened!" << std::endl;
    ofile << highscore;
    ofile.close(); // Close file
}

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");
    sf::Vector2i windowSize(window.getSize().x, window.getSize().y);
    // Set Icon
    sf::Image icon;
    if (!icon.loadFromFile("Textures/icon.png")) std::cerr << "Could not load icon." << std::endl;
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load Font
    sf::Font myfont;
    if(!myfont.loadFromFile("Font/arial.ttf")) std::cerr << "Could not load font." << std::endl;

    // Show FPS
    int window_fps = 60;
    window.setFramerateLimit(window_fps);
    sf::Text fps_text;
    fps_text.setString(std::to_string(window_fps));
    fps_text.setFillColor(sf::Color::Green);
    fps_text.setFont(myfont);
    fps_text.setCharacterSize(20);
    fps_text.setPosition(windowSize.x - fps_text.getGlobalBounds().width -5, 0); // -5 for better visibility

    // Set Background
    sf::Texture space_texture;
    if(!space_texture.loadFromFile("Textures/background.jpg")) std::cerr << "Could not load space texture." << std::endl;
    sf::Sprite space_background;
    space_background.setTexture(space_texture);
    space_background.setPosition(0, 0);

    // ------Score Text----
    int score = 0;
    sf::Text score_text;
    score_text.setString("Score: " + std::to_string(score));
    score_text.setStyle(sf::Text::Bold);
    score_text.setFillColor(sf::Color::Green);
    score_text.setOutlineColor(sf::Color::Black);
    score_text.setOutlineThickness(1);
    score_text.setFont(myfont);
    score_text.setCharacterSize(25);
    score_text.setPosition(0 +5, 0); // +5 for better visibility

    // Grey Overlay for Pause/Play Feature
    sf::Texture overlay_texture;
    if(!overlay_texture.loadFromFile("Textures/gray_overlay.jpg")) std::cerr << "Could not load overlay texture." << std::endl;
    sf::Sprite overlay;
    overlay.setTexture(overlay_texture);
    overlay.setPosition(0, 0);
    overlay.setColor(sf::Color(overlay.getColor().r, overlay.getColor().g, overlay.getColor().b, 200));

    // Play/Pause Feature
    bool pause_on = false;
    sf::Texture play_texture;
    if(!play_texture.loadFromFile("Textures/Buttons/play.png")) std::cerr << "Could not load play button texture." << std::endl;
    play_texture.setSmooth(true); // Smooth borders
    sf::Sprite play_button;
    play_button.setTexture(play_texture);
    play_button.setScale(0.25, 0.25); // 512 * 0.25 = 128
    play_button.setPosition(windowSize.x/2 -play_button.getGlobalBounds().width/2, windowSize.y/2 -play_button.getGlobalBounds().width/2);
    sf::Texture pause_texture;
    if(!pause_texture.loadFromFile("Textures/Buttons/pause.png")) std::cerr << "Could not load pause button texture." << std::endl;
    pause_texture.setSmooth(true); // Smooth borders
    sf::Sprite pause_button;
    pause_button.setTexture(pause_texture);
    pause_button.setScale(0.09765625, 0.09765625);  // 512 * 0.09765625 = 50
    pause_button.setPosition(windowSize.x/2 -pause_button.getGlobalBounds().width -5, 0 +2); // +2 and -5 for better visibility

    // Control Keys - [<- ->] or [A D]
    bool arrow_control = true;
    sf::Keyboard::Key left_key;
    sf::Keyboard::Key right_key;

    // Player/Spaceship
    sf::Texture player_texture;
    if(!player_texture.loadFromFile("Textures/player.png")) std::cerr << "Could not load spaceship/player texture." << std::endl;
    Player player(player_texture, windowSize);
    player.setSpeed(400);

    // Random Seed
    std::srand(std::time(NULL)); // For random spawn location of enemies
    // Enemy
    sf::Texture enemy_texture;
    if(!enemy_texture.loadFromFile("Textures/enemy.png")) std::cerr << "Could not load enemy texture." << std::endl;
    // Enemies
    int enemy_count = 8;
    int enemy_speed = 250; // Set enemy speed
    std::vector<Enemy> enemies;
    for(int i = 0; i < enemy_count; i++) // Initialize with fixed number of enemies
    {
        makeEnemy(enemy_texture, windowSize, enemy_speed, player, enemies);
    }

    // Bullet
    sf::Texture bullet_texture;
    if(!bullet_texture.loadFromFile("Textures/bullet.png")) std::cerr << "Could not load bullet texture." << std::endl;
    Bullet bullet(bullet_texture, player.getPlayerPosition());
    bullet.setSpeed(500); // Set bullet speed

    // Explosion
    sf::Texture explosion_texture;
    if(!explosion_texture.loadFromFile("Textures/explosion.png")) std::cerr << "Could not load explosion texture." << std::endl;
    sf::Sprite explosion_sprite;
    explosion_sprite.setTexture(explosion_texture);
    int spritedim = 192; // Dimension of one sprite in sprite sheet. 192x192
    explosion_sprite.setTextureRect(sf::IntRect(0, 0, spritedim, spritedim));
    explosion_sprite.setScale((float)1/3, (float)1/3);
    sf::Vector2f enemyposition;
    bool explosion = false;
    float total_time = 0;
    float switch_time = 0.05; // 20 fps. Will play all 10 frames of sprite sheet in 0.5 second.
    int totalrow = 2;
    int totalcolumn = 5;
    int currentrow = 1;
    int currentcolumn = 1;

    // Game Over
    bool gameover = false;
    sf::Texture gameover_texture;
    if(!gameover_texture.loadFromFile("Textures/gameover.png")) std::cerr << "Could not load gameover texture." << std::endl;
    sf::Sprite gameover_background;
    gameover_background.setTexture(gameover_texture);
    gameover_background.setPosition(0, 0);

    // Play Again
    sf::Text playagain_button;
    playagain_button.setString("PLAY AGAIN");
    playagain_button.setStyle(sf::Text::Bold | sf::Text::Underlined);
    playagain_button.setFillColor(sf::Color::Red);
    playagain_button.setOutlineColor(sf::Color::Black);
    playagain_button.setOutlineThickness(1.5);
    playagain_button.setFont(myfont);
    playagain_button.setCharacterSize(50);
    playagain_button.setPosition(windowSize.x/2 - playagain_button.getGlobalBounds().width/2, 500);

    // Start Menu
    sf::Texture title_texture;
    if(!title_texture.loadFromFile("Textures/title.png")) std::cerr << "Could not load title texture." << std::endl;
    sf::Sprite title;
    title.setTexture(title_texture);
    title.setPosition(0, 0);
    bool startmenu = true;
    // Start Button
    sf::Text start_button;
    start_button.setString("START");
    start_button.setStyle(sf::Text::Bold | sf::Text::Underlined);
    start_button.setFillColor(sf::Color::Yellow);
    start_button.setOutlineColor(sf::Color::Red);
    start_button.setOutlineThickness(2);
    start_button.setFont(myfont);
    start_button.setCharacterSize(50);
    start_button.setPosition(windowSize.x/4 - start_button.getGlobalBounds().width/2, 450);
    sf::Text start_help;
    start_help.setString("[Press ENTER]");
    start_help.setStyle(sf::Text::Bold);
    start_help.setFillColor(sf::Color::Yellow);
    start_help.setOutlineColor(sf::Color::Red);
    start_help.setOutlineThickness(2);
    start_help.setFont(myfont);
    start_help.setCharacterSize(25);
    start_help.setPosition(windowSize.x/4 - start_help.getGlobalBounds().width/2, 520);
    // Quit Button
    sf::Text quit_button;
    quit_button.setString("QUIT");
    quit_button.setStyle(sf::Text::Bold | sf::Text::Underlined);
    quit_button.setFillColor(sf::Color::Yellow);
    quit_button.setOutlineColor(sf::Color::Red);
    quit_button.setOutlineThickness(2);
    quit_button.setFont(myfont);
    quit_button.setCharacterSize(50);
    quit_button.setPosition(windowSize.x * (0.75) - quit_button.getGlobalBounds().width/2, 450);
    sf::Text quit_help;
    quit_help.setString("[Press Q]");
    quit_help.setStyle(sf::Text::Bold);
    quit_help.setFillColor(sf::Color::Yellow);
    quit_help.setOutlineColor(sf::Color::Red);
    quit_help.setOutlineThickness(2);
    quit_help.setFont(myfont);
    quit_help.setCharacterSize(25);
    quit_help.setPosition(windowSize.x * (0.75) - quit_help.getGlobalBounds().width/2, 520);

    // Exit Button (With sprite)
    sf::Texture exit_texture;
    if(!exit_texture.loadFromFile("Textures/Buttons/exit.png")) std::cerr << "Could not load exit texture." << std::endl;
    sf::Sprite exit_button;
    exit_button.setTexture(exit_texture);
    exit_button.setScale(1, 1);
    exit_button.setPosition(windowSize.x - exit_button.getGlobalBounds().width, windowSize.y - exit_button.getGlobalBounds().height);

    // Settings Menu
    bool settings = false;
    sf::Texture settings_texture;
    if(!settings_texture.loadFromFile("Textures/Buttons/settings.png")) std::cerr << "Could not load settings texture." << std::endl;
    sf::Sprite settings_button;
    settings_button.setTexture(settings_texture);
    settings_button.setScale(0.78125, 0.78125); // 0.78125 * 64 = 50
    settings_button.setPosition(windowSize.x/2 +5, 0 +2); // +2 and +5 for better visibility
    // Check box
    sf::Texture checkbox_texture;
    if(!checkbox_texture.loadFromFile("Textures/Buttons/checkbox.png")) std::cerr << "Could not load checkbox texture." << std::endl;
    // FPS Setting
    sf::Text fps_setting_text;
    fps_setting_text.setString("Show FPS");
    fps_setting_text.setStyle(sf::Text::Bold);
    fps_setting_text.setFillColor(sf::Color::Red);
    fps_setting_text.setOutlineColor(sf::Color::Black);
    fps_setting_text.setOutlineThickness(2);
    fps_setting_text.setFont(myfont);
    fps_setting_text.setCharacterSize(30);
    fps_setting_text.setPosition(windowSize.x * (0.25) - fps_setting_text.getGlobalBounds().width/2, 150);
    Setting fps_setting(checkbox_texture, true);
    fps_setting.setPosition(windowSize.x/2 -75, fps_setting_text.getPosition().y);
    // Sound Setting
    sf::Text sound_setting_text;
    sound_setting_text.setString("Sound On");
    sound_setting_text.setStyle(sf::Text::Bold);
    sound_setting_text.setFillColor(sf::Color::Red);
    sound_setting_text.setOutlineColor(sf::Color::Black);
    sound_setting_text.setOutlineThickness(2);
    sound_setting_text.setFont(myfont);
    sound_setting_text.setCharacterSize(30);
    sound_setting_text.setPosition(windowSize.x * (0.25) - sound_setting_text.getGlobalBounds().width/2, 250);
    Setting sound_setting(checkbox_texture, true);
    sound_setting.setPosition(windowSize.x/2 -75, sound_setting_text.getPosition().y);
    // Music Setting
    sf::Text music_setting_text;
    music_setting_text.setString("Music On");
    music_setting_text.setStyle(sf::Text::Bold);
    music_setting_text.setFillColor(sf::Color::Red);
    music_setting_text.setOutlineColor(sf::Color::Black);
    music_setting_text.setOutlineThickness(2);
    music_setting_text.setFont(myfont);
    music_setting_text.setCharacterSize(30);
    music_setting_text.setPosition(windowSize.x * (0.25) - music_setting_text.getGlobalBounds().width/2, 350);
    Setting music_setting(checkbox_texture, true);
    music_setting.setPosition(windowSize.x/2 -75, music_setting_text.getPosition().y);
    // Side Teleport Setting
    sf::Text teleport_setting_text;
    teleport_setting_text.setString("Side Teleport");
    teleport_setting_text.setStyle(sf::Text::Bold);
    teleport_setting_text.setFillColor(sf::Color::Red);
    teleport_setting_text.setOutlineColor(sf::Color::Black);
    teleport_setting_text.setOutlineThickness(2);
    teleport_setting_text.setFont(myfont);
    teleport_setting_text.setCharacterSize(30);
    teleport_setting_text.setPosition(windowSize.x * (0.25) - teleport_setting_text.getGlobalBounds().width/2, 450);
    Setting teleport_setting(checkbox_texture, false);
    teleport_setting.setPosition(windowSize.x/2 -75, teleport_setting_text.getPosition().y);
    // Help Box
    sf::Texture help_t;
    if(!help_t.loadFromFile("Textures/help.png")) std::cerr << "Could not load help texture." << std::endl;
    sf::Sprite helpbox;
    helpbox.setTexture(help_t);
    helpbox.setScale(0.55, 0.55);
    helpbox.setPosition(windowSize.x/2 , 60);

    // Sounds
    sf::SoundBuffer laser_buffer;
    if (!laser_buffer.loadFromFile("Textures/Sounds/laser.wav")) std::cerr << "Could not load laser buffer." << std::endl;
    sf::Sound laser_sound;
    laser_sound.setBuffer(laser_buffer);
    laser_sound.setVolume(25.0f);
    sf::SoundBuffer explosion_buffer;
    if (!explosion_buffer.loadFromFile("Textures/Sounds/explosion.wav")) std::cerr << "Could not load explosion buffer." << std::endl;
    sf::Sound explosion_sound;
    explosion_sound.setBuffer(explosion_buffer);
    explosion_sound.setVolume(25.0f);
    sf::Music music;
    if (!music.openFromFile("Textures/Sounds/music.ogg")) std::cerr << "Could not load music buffer." << std::endl;
    music.setLoop(true);
    music.setVolume(15.0f);
    music.play();
    sf::SoundBuffer levelup_buffer;
    if (!levelup_buffer.loadFromFile("Textures/Sounds/levelup.wav")) std::cerr << "Could not load levelup buffer." << std::endl;
    sf::Sound levelup_sound;
    levelup_sound.setBuffer(levelup_buffer);
    levelup_sound.setVolume(25.0f);
    sf::SoundBuffer gameover_buffer;
    if (!gameover_buffer.loadFromFile("Textures/Sounds/gameover.wav")) std::cerr << "Could not load gameover buffer." << std::endl;
    sf::Sound gameover_sound;
    gameover_sound.setBuffer(gameover_buffer);
    gameover_sound.setVolume(25.0f);

    // Difficulty Increment
    int maxlevel = 10; // Total 10 levels
    int currentlevel = 1; // Increase level every 500 points

    // Side Teleport - Continuous movement of player through sides of window
    bool side_teleport = false;

    // High Score File and Text
    std::ifstream ifile("highscore.txt");
    if (!ifile.is_open()) std::cout << "File was not opened!" << std::endl;
    int highscore = 0;
    ifile >> highscore;
    ifile.close(); // Close file
    sf::Text highscore_text;
    highscore_text.setString("Highscore: " + std::to_string(highscore));
    highscore_text.setStyle(sf::Text::Bold);
    highscore_text.setFillColor(sf::Color::Red);
    highscore_text.setOutlineColor(sf::Color::Black);
    highscore_text.setOutlineThickness(1);
    highscore_text.setFont(myfont);
    highscore_text.setCharacterSize(30);
    highscore_text.setPosition(windowSize.x/2 - highscore_text.getGlobalBounds().width/2, 50);

    // Clock
    sf::Clock clock;

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // -----Delta Time----
        sf::Time elapsed = clock.restart();

        // ------Show Score----
        score_text.setString("Score: " + std::to_string(score));

        // -----------Change Control Keys--------
        if(arrow_control) // If controls are set to arrows
        {
            left_key = sf::Keyboard::Left;
            right_key = sf::Keyboard::Right;
        }
        else
        {
            left_key = sf::Keyboard::A;
            right_key = sf::Keyboard::D;
        }
        player.setLeftkey(left_key);
        player.setRightkey(right_key);

        if(!pause_on && !gameover && !startmenu && !settings) // If game not paused and not over and not in start menu and not in settings menu
        {
            // ------------Player Movement-----------
            if(sf::Keyboard::isKeyPressed(left_key))
            {
                player.movePlayer(elapsed, left_key, windowSize, side_teleport); // Call move function in Player class
            }
            if(sf::Keyboard::isKeyPressed(right_key))
            {
                player.movePlayer(elapsed, right_key, windowSize, side_teleport); // Call move function in Player class
            }

            // -----------Enemy Movement--------------
            for(int i = 0; i < (int)enemies.size(); i++)
            {
                if(enemies[i].escape)// If escaped then ------------Game Over------------------
                {
                    gameover = true;
                    if(sound_setting.getCheck()) gameover_sound.play();
                    // Check if new highscore
                    if(score > highscore)
                    {
                        highscore = score;
                        highscore_text.setString("Highscore: " + std::to_string(highscore));
                    }
                }
                if(!enemies[i].alive) // Check if enemy is still shootable/alive. Enemy is not alive
                {
                    enemies.erase(enemies.begin() + i);
                }
                else
                {
                    enemies[i].animate(elapsed, windowSize); // Animate alive enemies
                }
            }
            // Enemy respawn
            if((int)enemies.size() != enemy_count)
            {
                for(int i = 0; i < enemy_count - (int)enemies.size(); i++)
                {
                    makeEnemy(enemy_texture, windowSize, enemy_speed, player, enemies);
                }
            }

            // -------------Bullet Movement-------------
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bullet.fired == false)
            {
                bullet.fired = true; // Bullet has been fired
                if(sound_setting.getCheck()) laser_sound.play();
            }
            // ----------Collision Detection-------------
            for(auto &enemy : enemies)
            {
                sf::FloatRect bulletbounds = bullet.getGbounds();
                if(enemy.isCollided(bulletbounds))
                {
                    enemy.alive = false;
//                    bullet.alive = false; // in case of rapid bullet implementation
                    bullet.fired = false;
                    score += 10; // 10 points for each collision
                    enemyposition = enemy.getEnemyPosition();
                    explosion = true;
                    if(sound_setting.getCheck()) explosion_sound.play();
                }
            }
            bullet.animate(elapsed, player.getPlayerPosition()); // Animate bullet after checking for collision. Otherwise bullet goes through enemy

            // ------------Difficulty Increment-----------
            if(score/500 == currentlevel+1 && currentlevel < maxlevel) // If score has reached points required for next level
            {
                currentlevel++;
                enemy_count += 2;
                enemy_speed += 25;
                if(sound_setting.getCheck()) levelup_sound.play();
            }

            // ----------Explosion Animation-------------
            if(explosion) // If explosion == true
            {
                // Set animation position
                explosion_sprite.setPosition(enemyposition.x, enemyposition.y); // Center animation wrt to enemy sprite

                total_time += elapsed.asSeconds();

                if(total_time >= switch_time)
                {
                    total_time -= switch_time; // Almost same as total_time = 0;. In the long run this will prevent errors caused by approximations.
                    explosion_sprite.setTextureRect(sf::IntRect((currentcolumn-1) * spritedim, (currentrow-1) * spritedim, spritedim, spritedim));
                    explosion_sprite.setScale((float)1/3, (float)1/3); // 192 / 3 = 64
                    currentcolumn++;
                    if(currentcolumn == totalcolumn && currentrow != totalrow) // After animating first row of sprite sheet
                    {
                        currentrow++;
                        currentcolumn = 1;
                    }
                    if(currentrow >= totalrow && currentcolumn >= totalcolumn) // If one animation is done
                    {
                        explosion = false;
                        currentcolumn = 1;
                        currentrow = 1;
                    }
                }
            }
        }

        // ------------Play Again---------------
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::R) and gameover)
        {
            gameover = false;
            score = 0;
            currentlevel = 1; // Reset difficulty level
            // Reset to default values
            enemy_count = 8;
            enemy_speed = 250;
            enemies.clear(); // Clear all enemies
            for(int i = 0; i < enemy_count; i++) // Initialize with fixed number of enemies
            {
                makeEnemy(enemy_texture, windowSize, enemy_speed, player, enemies);
            }
        }

        // ------------Quit - Press Q------------
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            write_highscore(highscore);
            return 0; // return 0 means program worked how it was supposed to.
        }
        // check all the window's events that were triggered since the last iteration of the loop

        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if(!gameover && !startmenu) // Only if game not over
            {
                // ----------Pause/Play - Press P and release---------
                if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P && !settings)
                {
                    pause_on = !pause_on; // Beautiful
                }
                // ----------Settings Menu---------
                if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::S && !pause_on)
                {
                    settings = !settings;
                }
                // --------Pause/Play - Click Pause/Play Button--------Exit Button----Settings Menu-----------
                if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) // If left mouse button is clicked
                {
                    sf::Vector2i mouseclick_pos = sf::Mouse::getPosition(window);
                    if(!pause_on && !settings) // If game not paused and not in settings menu
                    {
                        if(pause_button.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // If mouseclick is on pause button
                        {
                            pause_on = true;
                            settings = false; // Settings menu and pause menu should be mutually exclusive
                        }
                        // ----------Settings Button---------------
                        else if(settings_button.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // If settings button is clicked
                        {
                            settings = true;
                        }
                    }
                    else if(settings) // -----When Settings Menu is Open
                    {
                        if(exit_button.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // if mouselick is on exit button
                        {
                            write_highscore(highscore);
                            return 0;
                        }
                        else if(settings_button.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // If settings button is clicked
                        {
                            settings = false;
                        }
                        else if(fps_setting.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // if fps setting is clicked
                        {
                            fps_setting.setCheck(!fps_setting.getCheck());
                        }
                        else if(sound_setting.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // if sound settings is clicked
                        {
                            sound_setting.setCheck(!sound_setting.getCheck());
                        }
                        else if(music_setting.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // if music settings is clicked
                        {
                            if(music_setting.getCheck())
                            {
                                music_setting.setCheck(false);
                                music.stop();
                            }
                            else if(!music_setting.getCheck())
                            {
                                music_setting.setCheck(true);
                                music.play();
                            }
                        }
                        else if(teleport_setting.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // if teleport settings is clicked
                        {
                            side_teleport = !side_teleport;
                            teleport_setting.setCheck(side_teleport);
                        }
                    }
                    else if(pause_on)// If game is paused
                    {
                        if(play_button.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // If mouseclick is on play button
                        {
                            pause_on = false;
                        }
                        else if(exit_button.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // if mouselick is on exit button
                        {
                            write_highscore(highscore);
                            return 0;
                        }
                    }
                }
                // ----------Mute - Press M [For all sounds]------------
                if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::M)
                {
                    if(sound_setting.getCheck() != music_setting.getCheck())
                    {
                        sound_setting.setCheck(false);
                        music_setting.setCheck(false);
                    }
                    else
                    {
                        sound_setting.setCheck(!sound_setting.getCheck());
                        if(music_setting.getCheck())
                        {
                            music_setting.setCheck(false);
                            music.stop();
                        }
                        else if(!music_setting.getCheck())
                        {
                            music_setting.setCheck(true);
                            music.play();
                        }
                    }
                }
                // ------------Side Teleport - Press T------------------
                if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::T)
                {
                    side_teleport = !side_teleport;
                    teleport_setting.setCheck(side_teleport);
                }
            }
            else if(gameover)
            {
                // ------------Play Again---------------
                if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) // If left mouse button is clicked
                {
                    sf::Vector2i mouseclick_pos = sf::Mouse::getPosition(window);
                    if(playagain_button.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // If play again button is clicked
                    {
                        gameover = false;
                        score = 0;
                        currentlevel = 1; // Reset difficulty level
                        // Reset to default values
                        enemy_count = 8;
                        enemy_speed = 250;
                        enemies.clear(); // Clear all enemies
                        for(int i = 0; i < enemy_count; i++) // Initialize with fixed number of enemies
                        {
                            makeEnemy(enemy_texture, windowSize, enemy_speed, player, enemies);
                        }
                    }
                    else if(exit_button.getGlobalBounds().contains(mouseclick_pos.x, mouseclick_pos.y)) // if mouselick is on exit button
                    {
                        write_highscore(highscore);
                        return 0;
                    }
                }
            }

            // ------Mod for Score-----
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add) score += 100;

            // ---------Switch Control Keys----------
            if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::K)
            {
                arrow_control = !arrow_control;
            }

            // -----------Start Game------------------
            if(startmenu)
            {
                if(event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) startmenu = false;
                else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousepos = sf::Mouse::getPosition(window);
                    if(start_button.getGlobalBounds().contains(mousepos.x, mousepos.y)) // if start button is clicked
                    {
                        startmenu = false;
                    }
                    else if(quit_button.getGlobalBounds().contains(mousepos.x, mousepos.y)) // if quit button is clicked
                    {
                        write_highscore(highscore);
                        return 0;
                    }
                }
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything in Correct Order
        window.draw(space_background); // Draw space background first

        if(!startmenu) // If not on start menu
        {
            if(fps_setting.getCheck()) window.draw(fps_text); // Draw fps
            bullet.drawBullet(window); // Draw bullet
            player.drawPlayer(window); // Draw player/spacship
            for(auto &enemy : enemies) // Iterate through enemies vector
            {
                enemy.drawEnemy(window); // Draw Enemy
            }
            if(explosion) window.draw(explosion_sprite); // Draw/Animate explosion
            if(pause_on) // If game is paused
            {
                window.draw(overlay); // Draw overlay
                window.draw(play_button); // Draw play button
                window.draw(exit_button); // Draw exit button
            }
            else if(gameover) // If game over
            {
                window.draw(overlay); // Draw overlay
                window.draw(gameover_background); // Draw gameover sprite
                window.draw(playagain_button); // Draw play again prompt
                window.draw(exit_button); // Draw exit button
                window.draw(highscore_text); // Draw the high score
            }
            else if(settings) // if settings is true
            {
                window.draw(overlay);
                window.draw(exit_button);
                window.draw(settings_button);
                window.draw(fps_setting_text);
                window.draw(fps_setting);
                window.draw(sound_setting_text);
                window.draw(sound_setting);
                window.draw(music_setting_text);
                window.draw(music_setting);
                window.draw(teleport_setting_text);
                window.draw(teleport_setting);
                window.draw(helpbox);
            }
            else // If game is being played
            {
                window.draw(pause_button);
                window.draw(settings_button); // Draw settings button
            }
            window.draw(score_text); // Draw score
        }
        else
        {
            window.draw(title); // Draw the title
            window.draw(start_button); // Draw the start button
            window.draw(start_help);
            window.draw(quit_button); // Draw the quit button
            window.draw(quit_help);
        }

        // end the current frame
        window.display();
    }
    return 0;
}
