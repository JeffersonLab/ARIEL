#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "art-workbook/GoodArtWorkflow/detail/tidbits.h"

#include <iostream>
#include <cstdio>
#include <cctype>

namespace fhicl {
  class ParameterSet;
}

namespace {

  bool not_valid_choice(std::string const& s, std::string& msg, bool & flag )
  {
    if ( s.size() != 1 ) {
      msg = "Uh oh.  You specified too many characters.  Try again.\n";
      return true;
    }
    if ( std::isdigit(s[0]) ) {
      msg = "What am I supposed to do with a number?  Try again.\n";
      return true;
    }
    if ( !std::isdigit(s[0]) && !std::isalpha(s[0]) ){
      msg = "Oh that's funny. ..... Try again. \n";
      return true;
    }
    if ( s[0] == 'N' || s[0] == 'n' ) {
      flag = false;
      return false;
    }
    if ( s[0] == 'Y' || s[0] == 'y' ) {
      flag = true;
      return false;
    }
    return true;
  }

  void selected(bool& flag)
  {
    std::string msg;
    std::string test;
    std::size_t counter{};
    do {
      if ( counter > 2 ) {
        std::cout <<
          "You are not good at following instructions.\n"
          "Trivia reading privilege revoked.\n";
        break;
      }
      else if ( counter != 0 ){
        std::cout << msg;
      }
      std::cout <<"Answer: 'Y' or 'N' ... \n\n";
      std::cin >> test;
      ++counter;
    } while ( not_valid_choice(test, msg, flag) );
  }

  class Producer : public art::EDProducer {
  public:

    Producer(fhicl::ParameterSet const&){}

    void beginJob() override;
    void produce(art::Event &) override;

  private:
    bool userSelect_ = false;

  };

  void Producer::beginJob()
  {
    std::string msg =
      "\nCongratulations.  You have successfully included\n"
      "the 'Producer' module as a producer in your trigger_paths.\n"
      "The deep, dark secret is that this is not a selector module.\n"
      "It is a producer module, that produces no 'art' product, but\n"
      "instead \"produces amusement\" for the user.\n"
      "I do not have any ice cream to give you, but you can now\n"
      "decide on an event-by-event basis if you want a piece of trivia\n"
      "printed out to the screen. Do you want this functionality?\n\n";
    std::cout << msg;
    selected( userSelect_ );
  }

  void Producer::produce( art::Event &e )
  {
    if ( userSelect_ ) {
      bool flag {false};
      std::cout << "\nTrivia for event: " << e.id() << "?\n\n";
      selected( flag );
      if ( flag ) funny::tidbit();
    }
  }

}

DEFINE_ART_MODULE(Producer)
