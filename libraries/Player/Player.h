#ifndef Player_H
#define Player_H

#include <WString.h>

class Player
{
    public:
        Player();
        Player(const int&, const String&);
		int getPlayerID();
        void setPlayerID(int);
        void setNickname(String);
        String getNickname();
        void setScore(int);
        int getScore();
		bool isAlive();
		void die();
    private:
        int _playerID;
        String _nickname;
        int _score;
		bool _alive;

};
#endif