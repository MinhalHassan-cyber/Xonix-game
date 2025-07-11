# Xonix-game
This Xonix project is a feature-rich, modernized version of the classic arcade game, developed in C++ using SFML. The game includes Single and Two Player Modes, selectable from a well-designed Start Menu offering options such as "Start Game", "Select Level", and "View Scoreboard".Upon game completion, the End Menu displays the final scores, highlights new high scores, and provides options to restart, return to the main menu, or exit the game. The Difficulty Settings allow players to choose between Easy (2 enemies), Medium (4 enemies), Hard (6 enemies), or a Continuous Mode that adds two enemies every 20 seconds for a progressively challenging experience. A Movement Counter tracks and displays the number of tile-building actions a player performs. The game dynamically scales in challenge with a Timer that increases enemy speed every 20 seconds, and after 30 seconds, half the enemies shift to predefined geometric movement patterns like zig-zag and circular, each implemented in clearly labeled, modular functions. The Scoring System awards points for captured tiles, doubles points for capturing 10+ tiles, and scales to 4x after multiple occurrences. Power-ups that freeze enemies (and in two-player mode, the opponent too) are granted at scoring milestones and can be stored and activated strategically. A file-based Scoreboard maintains the top 5 high scores with timestamps, updating automatically when a new high score is achieved. In Two Player Mode, both players share the game board and timer, with distinct controls (Arrow keys for Player 1 and WASD for Player 2), individual scores and power-ups displayed on-screen. Players can interact and affect each other — collisions during tile construction lead to player elimination based on clearly defined rules. The game ends when both players are eliminated, declaring the winner based on score. This project demonstrates complex game logic, real-time state handling, adaptive difficulty, file management, and cooperative-competitive gameplay design.









Ask ChatGPT

