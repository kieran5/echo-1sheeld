#include <Player.h>



Player::Player() {}

Player::Player(const int& id, const String& nickname)
{
    _playerID = id;
    _nickname = nickname;    
}

void Player::setID(int id)
{
    _playerID = id;
}

int Player::getID()
{
    return _playerID;
}

void Player::setNickname(String nickname)
{
    _nickname = nickname;
}

String Player::getNickname()
{
    return _nickname;
}

void Player::setScore(int score)
{
    _score = score;
}

int Player::getScore()
{
    return _score;
}