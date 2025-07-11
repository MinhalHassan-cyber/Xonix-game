#include <SFML/Graphics.hpp>
#include <time.h>
#include <cstdlib>
#include <string>
#include <fstream>
using namespace sf;
using namespace std;

const int M = 25;
const int N = 40;
const int ts = 18; // tile size

int grid[M][N] = {0};
int highScores[5] = {0};
string highScoreNames[5];

struct Enemy
{
  float x, y, dx, dy;

  Enemy()
  {
    x = (rand() % (N - 2) + 1) * ts;
    y = (rand() % (M - 2) + 1) * ts;
    dx = (rand() % 2 ? 1 : -1) * (1 + rand() % 3);
    dy = (rand() % 2 ? 1 : -1) * (1 + rand() % 3);
  }

  void move()
  {
    x += dx;
    if (grid[int(y) / ts][int(x) / ts] == 1)
    {
      dx = -dx;
      x += dx;
    }
    y += dy;
    if (grid[int(y) / ts][int(x) / ts] == 1)
    {
      dy = -dy;
      y += dy;
    }

    // Keep enemies within bounds
    if (x < ts)
      x = ts;
    if (x > (N - 2) * ts)
      x = (N - 2) * ts;
    if (y < ts)
      y = ts;
    if (y > (M - 2) * ts)
      y = (M - 2) * ts;
  }
};

struct Player
{
  int x, y;
  int dx, dy;
  Color color;
  bool isAlive;
  bool isBuilding;
  int score;
  int id; // 2 for player1, 3 for player2

  Player(int x, int y, Color c, int id) : x(x), y(y), dx(0), dy(0), color(c), isAlive(true),
                                          isBuilding(false), score(0), id(id) {}

  void reset(int startX, int startY)
  {
    x = startX;
    y = startY;
    dx = dy = 0;
    isAlive = true;
    isBuilding = false;
  }

  void updateControls(Keyboard::Key left, Keyboard::Key right,
                      Keyboard::Key up, Keyboard::Key down)
  {
    if (!isAlive)
      return;

    isBuilding = false;
    if (Keyboard::isKeyPressed(left))
    {
      dx = -1;
      dy = 0;
      isBuilding = true;
    }
    if (Keyboard::isKeyPressed(right))
    {
      dx = 1;
      dy = 0;
      isBuilding = true;
    }
    if (Keyboard::isKeyPressed(up))
    {
      dx = 0;
      dy = -1;
      isBuilding = true;
    }
    if (Keyboard::isKeyPressed(down))
    {
      dx = 0;
      dy = 1;
      isBuilding = true;
    }
  }

  void move()
  {
    if (!isAlive)
      return;
    x += dx;
    y += dy;

    // Boundary check
    if (x < 0)
      x = 0;
    if (x >= N)
      x = N - 1;
    if (y < 0)
      y = 0;
    if (y >= M)
      y = M - 1;
  }
};

void drop(int y, int x)
{
  if (y < 0 || y >= M || x < 0 || x >= N)
    return;
  if (grid[y][x] != 0)
    return;
  grid[y][x] = -1;
  drop(y - 1, x);
  drop(y + 1, x);
  drop(y, x - 1);
  drop(y, x + 1);
}

void loadHighScores()
{
  ifstream file("scoreboard.txt");
  if (file.is_open())
  {
    for (int i = 0; i < 5; i++)
    {
      file >> highScoreNames[i] >> highScores[i];
    }
    file.close();
  }
}

void saveHighScores()
{
  ofstream file("scoreboard.txt");
  if (file.is_open())
  {
    for (int i = 0; i < 5; i++)
    {
      file << highScoreNames[i] << " " << highScores[i] << endl;
    }
    file.close();
  }
}

void updateHighScores(const string &name, int score)
{
  for (int i = 0; i < 5; i++)
  {
    if (score > highScores[i])
    {
      for (int j = 4; j > i; j--)
      {
        highScores[j] = highScores[j - 1];
        highScoreNames[j] = highScoreNames[j - 1];
      }
      highScores[i] = score;
      highScoreNames[i] = name;
      saveHighScores();
      break;
    }
  }
}

void resetGame(int &enemyCount, bool continuousMode)
{
  for (int i = 0; i < M; i++)
  {
    for (int j = 0; j < N; j++)
    {
      grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;
    }
  }
  if (!continuousMode)
  {
    enemyCount = (enemyCount < 2) ? 2 : (enemyCount > 6) ? 6
                                                         : enemyCount;
  }
}

void drawMenu(RenderWindow &window, Font &font, const string &title,
              const string options[], int numOptions, int selected)
{
  Text titleText(title, font, 40);
  titleText.setPosition(150, 30);
  titleText.setFillColor(Color::Cyan);

  window.clear(Color::Black);
  window.draw(titleText);

  for (int i = 0; i < numOptions; i++)
  {
    Text option(options[i], font, 30);
    option.setPosition(180, 120 + i * 60);
    option.setFillColor(i == selected ? Color::Yellow : Color::White);
    window.draw(option);
  }

  window.display();
}

int showMenu(RenderWindow &window, const string &title, const string options[], int numOptions)
{
  Font font;
  if (!font.loadFromFile("arial.ttf"))
    return 0;

  int selected = 0;
  while (window.isOpen())
  {
    Event e;
    while (window.pollEvent(e))
    {
      if (e.type == Event::Closed)
        return -1;
      if (e.type == Event::KeyPressed)
      {
        if (e.key.code == Keyboard::Up)
          selected = (selected - 1 + numOptions) % numOptions;
        if (e.key.code == Keyboard::Down)
          selected = (selected + 1) % numOptions;
        if (e.key.code == Keyboard::Enter)
          return selected;
        if (e.key.code == Keyboard::Escape)
          return -1;
      }
    }
    drawMenu(window, font, title, options, numOptions, selected);
  }
  return -1;
}

int main()
{
  srand(time(0));
  RenderWindow window(VideoMode(N * ts, M * ts), "Xonix Game");
  window.setFramerateLimit(60);

  Texture t1, t2, t3;
  t1.loadFromFile("images/tiles.png");
  t2.loadFromFile("images/gameover.png");
  t3.loadFromFile("images/enemy.png");

  Sprite sTile(t1), sGameover(t2), sEnemy(t3);
  sGameover.setPosition(100, 100);
  sEnemy.setOrigin(20, 20);

  loadHighScores();

  // Game state variables
  bool twoPlayerMode = false;
  int difficulty = 1; // 0=easy, 1=medium, 2=hard, 3=continuous
  int enemyCount = 4;
  bool continuousMode = false;
  float gameTime = 0;
  float speedTimer = 0;
  float continuousTimer = 0;
  bool gameOver = false;

  // Main game loop
  while (window.isOpen())
  {
    // Main menu
    string mainMenuOptions[] = {"1. Single Player", "2. Two Players", "3. Select Level", "4. Scoreboard", "5. Exit"};
    int menuChoice = showMenu(window, "XONIX GAME", mainMenuOptions, 5);

    if (menuChoice == -1 || menuChoice == 4)
      break; // Exit
    if (menuChoice == 3)
    { // Scoreboard
      string scoreOptions[6];
      scoreOptions[0] = "HIGH SCORES";
      for (int i = 0; i < 5; i++)
      {
        scoreOptions[i + 1] = to_string(i + 1) + ". " + highScoreNames[i] + " - " + to_string(highScores[i]);
      }
      showMenu(window, "SCOREBOARD", scoreOptions, 6);
      continue;
    }

    twoPlayerMode = (menuChoice == 1);

    if (menuChoice == 2)
    { // Select level
      string levelOptions[] = {"1. Easy (2 enemies)", "2. Medium (4 enemies)",
                               "3. Hard (6 enemies)", "4. Continuous Mode"};
      int levelChoice = showMenu(window, "SELECT LEVEL", levelOptions, 4);

      if (levelChoice == -1)
        continue;

      difficulty = levelChoice;
      switch (difficulty)
      {
      case 0:
        enemyCount = 2;
        continuousMode = false;
        break;
      case 1:
        enemyCount = 4;
        continuousMode = false;
        break;
      case 2:
        enemyCount = 6;
        continuousMode = false;
        break;
      case 3:
        enemyCount = 2;
        continuousMode = true;
        break;
      }
    }

    // Initialize game
    Player player1(10, 0, Color::Blue, 2);
    Player player2(N - 10, M - 1, Color::Red, 3);

    Enemy enemies[10]; // Max 10 enemies
    resetGame(enemyCount, continuousMode);

    Clock gameClock;
    float moveTimer = 0;
    const float moveDelay = 0.07f;
    bool running = true;

    while (running && window.isOpen())
    {
      float deltaTime = gameClock.restart().asSeconds();

      Event e;
      while (window.pollEvent(e))
      {
        if (e.type == Event::Closed)
        {
          window.close();
          return 0;
        }
        if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)
        {
          running = false;
        }
      }

      // Update timers
      gameTime += deltaTime;
      moveTimer += deltaTime;
      if (continuousMode)
        continuousTimer += deltaTime;

      // Increase enemy speed every 20 seconds
      if (speedTimer > 20.0f)
      {
        speedTimer = 0;
        for (int i = 0; i < enemyCount; i++)
        {
          enemies[i].dx *= 1.2f;
          enemies[i].dy *= 1.2f;
        }
      }
      else
      {
        speedTimer += deltaTime;
      }

      // Add enemies in continuous mode
      if (continuousMode && continuousTimer > 20.0f && enemyCount < 10)
      {
        continuousTimer = 0;
        enemyCount += 2;
        enemies[enemyCount - 2] = Enemy();
        enemies[enemyCount - 1] = Enemy();
      }

      // Update players
      player1.updateControls(Keyboard::Left, Keyboard::Right, Keyboard::Up, Keyboard::Down);
      if (twoPlayerMode)
        player2.updateControls(Keyboard::A, Keyboard::D, Keyboard::W, Keyboard::S);

      // Move players at fixed intervals
      if (moveTimer > moveDelay)
      {
        player1.move();
        if (twoPlayerMode)
          player2.move();
        moveTimer = 0;

        // Check player collisions with grid
        if (player1.isAlive)
        {
          // Collision with enemy trail
          if (grid[player1.y][player1.x] == 3)
            player1.isAlive = false;
          // Building trail
          if (player1.isBuilding && grid[player1.y][player1.x] == 0)
            grid[player1.y][player1.x] = player1.id;
        }

        if (twoPlayerMode && player2.isAlive)
        {
          // Collision with either player's trail
          if (grid[player2.y][player2.x] == 3 || grid[player2.y][player2.x] == 2)
            player2.isAlive = false;
          // Building trail
          if (player2.isBuilding && grid[player2.y][player2.x] == 0)
            grid[player2.y][player2.x] = player2.id;
        }

        // Check if players returned to border while building
        if (player1.isAlive && player1.isBuilding && grid[player1.y][player1.x] == 1)
        {
          player1.isBuilding = false;

          // Flood fill from enemies
          for (int i = 0; i < enemyCount; i++)
          {
            drop(enemies[i].y / ts, enemies[i].x / ts);
          }

          // Calculate captured area and score
          int captured = 0;
          for (int i = 0; i < M; i++)
          {
            for (int j = 0; j < N; j++)
            {
              if (grid[i][j] == -1)
              {
                grid[i][j] = 0;
              }
              else if (grid[i][j] == player1.id)
              {
                grid[i][j] = 1;
                captured++;
              }
              else if (twoPlayerMode && grid[i][j] == player2.id)
              {
                grid[i][j] = 1;
                captured++;
              }
            }
          }

          // Update score
          if (captured > 0)
          {
            player1.score += captured;
            if (twoPlayerMode)
              player2.score += captured;
          }
        }

        // Repeat for player 2 if in two-player mode
        if (twoPlayerMode && player2.isAlive && player2.isBuilding && grid[player2.y][player2.x] == 1)
        {
          player2.isBuilding = false;

          for (int i = 0; i < enemyCount; i++)
          {
            drop(enemies[i].y / ts, enemies[i].x / ts);
          }

          int captured = 0;
          for (int i = 0; i < M; i++)
          {
            for (int j = 0; j < N; j++)
            {
              if (grid[i][j] == -1)
              {
                grid[i][j] = 0;
              }
              else if (grid[i][j] == player1.id)
              {
                grid[i][j] = 1;
                captured++;
              }
              else if (grid[i][j] == player2.id)
              {
                grid[i][j] = 1;
                captured++;
              }
            }
          }

          if (captured > 0)
          {
            player1.score += captured;
            player2.score += captured;
          }
        }
      }

      // Move enemies
      for (int i = 0; i < enemyCount; i++)
      {
        enemies[i].move();

        // Check enemy collisions with players
        if (player1.isAlive &&
            abs(enemies[i].x / ts - player1.x) < 1 &&
            abs(enemies[i].y / ts - player1.y) < 1)
        {
          player1.isAlive = false;
        }
        if (twoPlayerMode && player2.isAlive &&
            abs(enemies[i].x / ts - player2.x) < 1 &&
            abs(enemies[i].y / ts - player2.y) < 1)
        {
          player2.isAlive = false;
        }
      }

      // Check game over conditions
      if ((!player1.isAlive && !twoPlayerMode) ||
          (twoPlayerMode && !player1.isAlive && !player2.isAlive))
      {
        gameOver = true;

        // Show end menu
        string endOptions[] = {"1. Restart", "2. Main Menu", "3. Exit"};
        string endTitle;
        if (twoPlayerMode)
        {
          endTitle = "P1: " + to_string(player1.score) + "  P2: " + to_string(player2.score);
          updateHighScores("Player1", player1.score);
          updateHighScores("Player2", player2.score);
        }
        else
        {
          endTitle = "Score: " + to_string(player1.score);
          updateHighScores("Player", player1.score);
        }

        int endChoice = showMenu(window, endTitle, endOptions, 3);

        if (endChoice == 0)
        { // Restart
          resetGame(enemyCount, continuousMode);
          player1.reset(10, 0);
          if (twoPlayerMode)
            player2.reset(N - 10, M - 1);
          gameOver = false;
          gameTime = 0;
          speedTimer = 0;
          continuousTimer = 0;
          continue;
        }
        else if (endChoice == 1)
        { // Main menu
          break;
        }
        else
        { // Exit
          window.close();
          return 0;
        }
      }

      // Drawing
      window.clear();

      // Draw grid
      for (int i = 0; i < M; i++)
      {
        for (int j = 0; j < N; j++)
        {
          if (grid[i][j] == 0)
            continue;

          if (grid[i][j] == 1)
            sTile.setTextureRect(IntRect(0, 0, ts, ts));
          else if (grid[i][j] == 2)
            sTile.setTextureRect(IntRect(54, 0, ts, ts)); // P1 trail
          else if (grid[i][j] == 3)
            sTile.setTextureRect(IntRect(108, 0, ts, ts)); // P2 trail

          sTile.setPosition(j * ts, i * ts);
          window.draw(sTile);
        }
      }

      // Draw players
      if (player1.isAlive)
      {
        RectangleShape playerRect(Vector2f(ts, ts));
        playerRect.setFillColor(player1.color);
        playerRect.setPosition(player1.x * ts, player1.y * ts);
        window.draw(playerRect);
      }

      if (twoPlayerMode && player2.isAlive)
      {
        RectangleShape playerRect(Vector2f(ts, ts));
        playerRect.setFillColor(player2.color);
        playerRect.setPosition(player2.x * ts, player2.y * ts);
        window.draw(playerRect);
      }

      // Draw enemies
      sEnemy.rotate(10);
      for (int i = 0; i < enemyCount; i++)
      {
        sEnemy.setPosition(enemies[i].x, enemies[i].y);
        window.draw(sEnemy);
      }

      // Draw UI
      Font font;
      if (font.loadFromFile("arial.ttf"))
      {
        // Player 1 info
        Text p1Info("P1: " + to_string(player1.score), font, 20);
        p1Info.setPosition(10, 10);
        p1Info.setFillColor(Color::Blue);
        window.draw(p1Info);

        // Player 2 info (if two player)
        if (twoPlayerMode)
        {
          Text p2Info("P2: " + to_string(player2.score), font, 20);
          p2Info.setPosition(N * ts - 100, 10);
          p2Info.setFillColor(Color::Red);
          window.draw(p2Info);
        }

        // Game time and enemies
        Text timeText("Time: " + to_string(int(gameTime)) + " Enemies: " + to_string(enemyCount), font, 20);
        timeText.setPosition(N * ts / 2 - 100, 10);
        timeText.setFillColor(Color::White);
        window.draw(timeText);
      }

      if (gameOver)
      {
        window.draw(sGameover);
      }

      window.display();
    }
  }

  return 0;
}
