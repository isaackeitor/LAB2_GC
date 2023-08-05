#include <SDL2/SDL.h>
#include <vector>

// Define the size of the framebuffer
const int FRAMEBUFFER_WIDTH = 100;
const int FRAMEBUFFER_HEIGHT = 100;
const int FRAMEBUFFER_SIZE = FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// Define a Color struct to hold the RGB values of a pixel
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// Declare the framebuffer as a global variable
Color framebuffer[FRAMEBUFFER_SIZE];

// Declare a global clearColor of type Color
Color clearColor = {0,173,255,47}; // Initially set to black

// Declare a global currentColor of type Color
Color currentColor = {173,255,47,0}; // Initially set to white

// Declare a global game state
std::vector<std::vector<bool> > game_state(FRAMEBUFFER_WIDTH, std::vector<bool>(FRAMEBUFFER_HEIGHT, false));

// Function to clear the framebuffer with the clearColor
void clear() {
    for (int i = 0; i < FRAMEBUFFER_SIZE; i++) {
        framebuffer[i] = clearColor;
    }
}

// Function to set a specific pixel in the framebuffer to the currentColor
void point(int x, int y) {
    if (x >= 0 && x < FRAMEBUFFER_WIDTH && y >= 0 && y < FRAMEBUFFER_HEIGHT) {
        framebuffer[y * FRAMEBUFFER_WIDTH + x] = currentColor;
    }
}

void renderBuffer(SDL_Renderer* renderer) {
    // Create a texture
    SDL_Texture* texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        FRAMEBUFFER_WIDTH, 
        FRAMEBUFFER_HEIGHT
    );

    // Update the texture with the pixel data from the framebuffer
    SDL_UpdateTexture(
        texture, 
        NULL, 
        framebuffer, 
        FRAMEBUFFER_WIDTH * sizeof(Color)
    );

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Destroy the texture
    SDL_DestroyTexture(texture);
}

void update_game_state() {
    std::vector<std::vector<bool> > new_state = game_state;
    for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++) {
        for (int x = 0; x < FRAMEBUFFER_WIDTH; x++) {
            int n_neigh = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = (x + dx + FRAMEBUFFER_WIDTH) % FRAMEBUFFER_WIDTH;
                    int ny = (y + dy + FRAMEBUFFER_HEIGHT) % FRAMEBUFFER_HEIGHT;
                    n_neigh += game_state[nx][ny] ? 1 : 0;
                }
            }
            if (game_state[x][y] && (n_neigh < 2 || n_neigh > 3)) {
                new_state[x][y] = false;
            } else if (!game_state[x][y] && n_neigh == 3) {
                new_state[x][y] = true;
            }
        }
    }
    game_state = new_state;
}

void render(SDL_Renderer* renderer) {
    // Draw the game state
    for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++) {
        for (int x = 0; x < FRAMEBUFFER_WIDTH; x++) {
            if (game_state[x][y]) {
                point(x, y);
            }
        }
    }

    // Update the game state
    update_game_state();

    // Render the framebuffer to the screen
    renderBuffer(renderer);
}

void init_glider(int x, int y) {
    std::vector<std::vector<bool> > glider_pattern(3, std::vector<bool>(3, false));
    glider_pattern[1][0] = true;
    glider_pattern[2][1] = true;
    glider_pattern[0][2] = glider_pattern[1][2] = glider_pattern[2][2] = true;

    for (int dy = 0; dy < 3; dy++) {
        for (int dx = 0; dx < 3; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = glider_pattern[dx][dy];
        }
    }
}

void init_block(int x, int y) {
    std::vector<std::vector<bool> > block_pattern(2, std::vector<bool>(2, true));

    for (int dy = 0; dy < 2; dy++) {
        for (int dx = 0; dx < 2; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = block_pattern[dx][dy];
        }
    }
}

void init_blinker(int x, int y) {
    std::vector<bool> blinker_pattern(3, true);

    for (int dx = 0; dx < 3; dx++) {
        game_state[(x + dx) % FRAMEBUFFER_WIDTH][y] = blinker_pattern[dx];
    }
}

void init_toad(int x, int y) {
    std::vector<std::vector<bool> > toad_pattern(2, std::vector<bool>(4, false));
    toad_pattern[0][1] = toad_pattern[0][2] = toad_pattern[0][3] = toad_pattern[1][0] = toad_pattern[1][1] = toad_pattern[1][2] = true;

    for (int dy = 0; dy < 2; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = toad_pattern[dy][dx];
        }
    }
}

void init_tub(int x, int y) {
    std::vector<std::vector<bool> > tub_pattern(3, std::vector<bool>(3, false));
    tub_pattern[0][1] = tub_pattern[1][0] = tub_pattern[1][2] = tub_pattern[2][1] = true;

    for (int dy = 0; dy < 3; dy++) {
        for (int dx = 0; dx < 3; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = tub_pattern[dx][dy];
        }
    }
}

void init_pulsar(int x, int y) {
    std::vector<std::vector<bool> > pulsar_pattern(17, std::vector<bool>(17, false));
    for (int i = 0; i < 17; i++) {
        for (int j = 0; j < 17; j++) {
            if ((i == 2 || i == 7 || i == 9 || i == 14) && (j > 3 && j < 7 || j > 9 && j < 13)) {
                pulsar_pattern[i][j] = true;
            }
            if ((j == 2 || j == 7 || j == 9 || j == 14) && (i > 3 && i < 7 || i > 9 && i < 13)) {
                pulsar_pattern[i][j] = true;
            }
        }
    }

    for (int dy = 0; dy < 17; dy++) {
        for (int dx = 0; dx < 17; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = pulsar_pattern[dx][dy];
        }
    }
}

void init_middleweight_spaceship(int x, int y) {
    std::vector<std::vector<bool> > mws_pattern(5, std::vector<bool>(6, false));
    mws_pattern[0][1] = mws_pattern[0][4] = mws_pattern[1][0] = mws_pattern[1][5] = mws_pattern[2][5] = true;
    mws_pattern[3][0] = mws_pattern[3][5] = mws_pattern[4][2] = mws_pattern[4][3] = mws_pattern[4][4] = mws_pattern[4][5] = true;

    for (int dy = 0; dy < 5; dy++) {
        for (int dx = 0; dx < 6; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = mws_pattern[dx][dy];
        }
    }
}

void init_beacon(int x, int y) {
    std::vector<std::vector<bool> > beacon_pattern(4, std::vector<bool>(4, false));
    beacon_pattern[0][0] = beacon_pattern[0][1] = beacon_pattern[1][0] = beacon_pattern[2][3] = beacon_pattern[3][2] = beacon_pattern[3][3] = true;

    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = beacon_pattern[dx][dy];
        }
    }
}

void init_loaf(int x, int y) {
    std::vector<std::vector<bool> > loaf_pattern(4, std::vector<bool>(4, false));
    loaf_pattern[0][1] = loaf_pattern[0][2] = loaf_pattern[1][0] = loaf_pattern[1][3] = loaf_pattern[2][1] = loaf_pattern[2][3] = loaf_pattern[3][2] = true;

    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = loaf_pattern[dx][dy];
        }
    }
}

void init_boat(int x, int y) {
    std::vector<std::vector<bool> > boat_pattern(3, std::vector<bool>(3, false));
    boat_pattern[0][0] = boat_pattern[0][1] = boat_pattern[1][0] = boat_pattern[1][2] = boat_pattern[2][1] = true;

    for (int dy = 0; dy < 3; dy++) {
        for (int dx = 0; dx < 3; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = boat_pattern[dx][dy];
        }
    }
}

void init_ship(int x, int y) {
    std::vector<std::vector<bool> > ship_pattern(3, std::vector<bool>(3, false));
    ship_pattern[0][0] = ship_pattern[0][2] = ship_pattern[1][0] = ship_pattern[1][1] = ship_pattern[2][1] = true;

    for (int dy = 0; dy < 3; dy++) {
        for (int dx = 0; dx < 3; dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = ship_pattern[dx][dy];
        }
    }
}

void init_gosper_glider_gun(int x, int y) {
    std::vector<std::vector<bool> > gosper_glider_gun_pattern(9, std::vector<bool>(36, false));

    // Assign the values for the Gosper Glider Gun pattern
    gosper_glider_gun_pattern[1][5] = gosper_glider_gun_pattern[1][6] = true;
    gosper_glider_gun_pattern[2][5] = gosper_glider_gun_pattern[2][6] = true;
    gosper_glider_gun_pattern[3][3] = gosper_glider_gun_pattern[3][4] = true;
    gosper_glider_gun_pattern[4][2] = gosper_glider_gun_pattern[4][8] = true;
    gosper_glider_gun_pattern[5][1] = gosper_glider_gun_pattern[5][2] = gosper_glider_gun_pattern[5][7] = gosper_glider_gun_pattern[5][8] = gosper_glider_gun_pattern[5][9] = true;
    gosper_glider_gun_pattern[6][2] = gosper_glider_gun_pattern[6][8] = true;
    gosper_glider_gun_pattern[7][3] = gosper_glider_gun_pattern[7][4] = true;
    gosper_glider_gun_pattern[8][5] = gosper_glider_gun_pattern[8][6] = true;

    for (int dy = 0; dy < gosper_glider_gun_pattern.size(); dy++) {
        for (int dx = 0; dx < gosper_glider_gun_pattern[dy].size(); dx++) {
            game_state[(x + dx) % FRAMEBUFFER_WIDTH][(y + dy) % FRAMEBUFFER_HEIGHT] = gosper_glider_gun_pattern[dy][dx];
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Conway’s Game Of Life", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    bool running = true;
    SDL_Event event;

    // Initialize multiple patterns at different locations
    for (int i = 0; i < FRAMEBUFFER_WIDTH; i += 10) {
        for (int j = 0; j < FRAMEBUFFER_HEIGHT; j += 10) {
            int pattern = rand() % 10;
            switch (pattern) {
                case 0:
                    init_glider(i, j);
                    break;
                case 1:
                    init_block(i, j);
                    break;
                case 2:
                    init_blinker(i, j);
                    break;
                case 3:
                    init_toad(i, j);
                    break;
                case 4:
                    init_tub(i, j);
                    break;
                case 5:
                    init_pulsar(i, j);
                    break;
                case 6:
                    init_beacon(i, j);
                    break;
                case 7:
                    init_loaf(i, j);
                    break;
                case 8:
                    init_boat(i, j);
                    break;
                case 9:
                    init_ship(i, j);
                    break;
            }
        }
    }

    int frame_count = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        
        clear();

        // Every 100 frames, reintroduce a random pattern at a random location
        if (frame_count % 100 == 0) {
            int x = rand() % FRAMEBUFFER_WIDTH;
            int y = rand() % FRAMEBUFFER_HEIGHT;
            int pattern = rand() % 10;
            switch (pattern) {
                case 0:
                    init_glider(x, y);
                    break;
                case 1:
                    init_block(x, y);
                    break;
                case 2:
                    init_blinker(x, y);
                    break;
                case 3:
                    init_toad(x, y);
                    break;
                case 4:
                    init_tub(x, y);
                    break;
                case 5:
                    init_pulsar(x, y);
                    break;
                case 6:
                    init_beacon(x, y);
                    break;
                case 7:
                    init_loaf(x, y);
                    break;
                case 8:
                    init_boat(x, y);
                    break;
                case 9:
                    init_ship(x, y);
                    break;
            }
        }

        // Call our render function
        render(renderer);

        // Present the frame buffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(1000 / 60);

        frame_count++;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
