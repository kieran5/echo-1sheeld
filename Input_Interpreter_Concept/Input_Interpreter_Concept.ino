//proof of concept of a method to expand the list of words that can be used to control the robot. 
//will probably need adapting to be useful.
#include <ArduinoSTL.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;

class InputInterpreter {

  private:
    //arrays of chosen words are used rather than a thesaurus to avoid clashes such as 'go' in 'go backwards' being interpreted as a forward command

    string forwardWords[5] = { "forward", "advance", "proceed", "onward", "ahead" };
    string backwardWords[4] = { "backward", "back", "reverse", "retreat" };
    string stopWords[6] = { "stop", "cease", "hold", "no", "stay", "halt" };

  public:
    string Interpret(string input) {

      //create vector to hold words from incoming string
      vector<string> receivedWords;
      //populate received words vector
      istringstream iss(input);
      copy(istream_iterator<string>(iss),
           istream_iterator<string>(),
           back_inserter(receivedWords));
      //compare receivced words against command words
      //the first match will be returned regardless of the rest of the content
      for (int ii = 0; receivedWords.size(); ii++)
      {

        for (int i = 0; i < 5; i++)
        {
          if (receivedWords[ii] == forwardWords[i])
          {
            return "forward";
          }
        }

        for (int i = 0; i < 6; i++)
        {
          if (receivedWords[ii] == stopWords[i])
          {
            return "stop";
          }
        }

        for (int i = 0; i < 4; i++)
        {
          if (receivedWords[ii] == backwardWords[i])
          {
            return "backward";
          }
        }

        return "no match";

      }

    };
};
