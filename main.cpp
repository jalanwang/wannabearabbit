#include "WebcamManager.hpp"
#include "GameStrategy.hpp"
#include "QRgame.hpp"
#include "RedBallGame.hpp"
#include <iostream>
#include <memory>

int main() {
    WebcamManager webcam;
    if (!webcam.initialize()) {
        return -1;
    }

    std::unique_ptr<GameStrategy> currentGame;
    GameState currentState = GameState::QR_GAME; // 초기 게임: QR 게임

    while (currentState != GameState::EXIT) {
        switch (currentState) {
            case GameState::QR_GAME:
                currentGame = std::make_unique<QRGame>(webcam);
                break;
            case GameState::RED_BALL_GAME:
                currentGame = std::make_unique<RedBallGame>(webcam);
                break;
            default:
                currentState = GameState::EXIT;
                continue;
        }

        if (currentGame) {
            // 선택된 게임 실행 (게임이 종료되면 다음 상태 반환)
            currentState = currentGame->run();
        }
    }

    return 0;
}