#ifndef GAME_STRATEGY_HPP
#define GAME_STRATEGY_HPP

#include <opencv2/opencv.hpp>

enum class GameState {
    EXIT,
    QR_GAME,
    FLAG_GAME    
};

class GameStrategy {
public:
    virtual ~GameStrategy() {}
    virtual GameState process(cv::Mat& frame, int key) = 0;
};

#endif // GAME_STRATEGY_HPP