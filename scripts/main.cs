using System;
using MeowDot;

public class Game {
    private float playerX = 100;
    private float playerY = 500;
    private float playerVY = 0;
    
    public void onStart() {
        Console.WriteLine("Game started!");
    }
    
    public void onUpdate(float dt) {
        // Jump
        if (Input.GetKeyDown(KeyCode.Space)) {
            if (playerY >= 500) {
                playerVY = -12;
            }
        }
        
        // Gravity
        playerVY += 25 * dt;
        playerY += playerVY * dt;
        
        // Ground collision
        if (playerY > 500) {
            playerY = 500;
            playerVY = 0;
        }
    }
    
    public void onDraw() {
        // Draw cat sprite
        Graphics.DrawSprite("cat.png", playerX, playerY, 60, 60);
    }
}
