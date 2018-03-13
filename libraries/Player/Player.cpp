#include <Player.h>

Player::Player() {}

Player::Player(const int& id, const String& nickname) {
    this._playerID = id;
    this._nickname = nickname;    
}

int Player::getPlayerID() {
    return _playerID;
}

void Player::setPlayerID(int id) {
    this._playerID = id;
}

String Player::getNickname() {
    return _nickname;
}

void Player::setNickname(String nickname) {
    this._nickname = nickname;
}

int Player::getScore() {
    return _score;
}

void Player::setScore(int score) {
    this._score = score;
}