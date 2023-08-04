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
Color clearColor = {0, 0, 0, 255}; // Initially set to black

// Declare a global currentColor of type Color
Color currentColor = {255, 255, 255, 255}; // Initially set to white

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
    // Clear the framebuffer
    clear();

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
    std::vector<bool> row1 = {false, true, false};
    std::vector<bool> row2 = {false, false, true};
    std::vector<bool> row3 = {true, true, true};
    std::vector<std::vector<bool> > glider_pattern = {row1, row2, row3};

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
    std::vector<bool> row1 = {false, true, false, false, true, false};
    std::vector<bool> row2 = {true, false, false, false, false, true};
    std::vector<bool> row3 = {true, false, false, false, false, true};
    std::vector<bool> row4 = {true, true, false, false, true, true};
    std::vector<bool> row5 = {false, false, true, true, true, true};
    std::vector<std::vector<bool> > mws_pattern = {row1, row2, row3, row4, row5};

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

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("life", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    bool running = true;
    SDL_Event event;

    init_glider(10, 10);
    init_block(30, 30);
    init_blinker(50, 50);
    init_toad(70, 70);
    init_tub(10, 40);
    init_pulsar(30, 60);
    init_middleweight_spaceship(50, 80);
    // init_beacon(70, 10);

    while (running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }

    // Call our render function
    render(renderer);

    // Present the frame buffer to the screen
    SDL_RenderPresent(renderer);

    // Delay to limit the frame rate
    SDL_Delay(1000 / 60);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
