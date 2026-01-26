#ifndef GAME_STRATEGY_HPP
#define GAME_STRATEGY_HPP

enum class GameState {
    EXIT,
    QR_GAME,
    GOAL_GAME,
    RED_BALL_GAME
};

class GameStrategy {
public:
    virtual ~GameStrategy() {}
    virtual GameState run() = 0;
};

#endif // GAME_STRATEGY_HPP