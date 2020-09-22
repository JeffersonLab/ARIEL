#include "art-workbook/GoodArtWorkflow/detail/tidbits.h"

#include <iostream>
#include <string>
#include <vector>

namespace {

  std::vector<std::string> const tidbits = {
    "Ludwig van Beethoven was deaf.\n"
    "Did you know that the French composer Gabriel Faure was also deaf?\n"
    "Have you ever heard of Faure?",
    //
    "Kyle majored in physics and piano performance.",
    //
    "'art' doesn't stand for anything.\n"
    "Kind of like the 'S' in Harry S Truman not standing for anything.",
    //
    "If one synchronized swimmer drowns, do they all drown?",
    //
    "Ever heard of a kakistocracy?  How about a plutocracy?\n"
    "Did you know \"mauve\" is pronounced like \"moh-ve\"?",
    //
    "Who won the Nobel prize in 1965? [Check the next event to get the answer.]",
    //
    "Feynman, Schwinger, and .... Tomonaga",
    //
    "\"A man, a plan, a canal, Panama\" is a palindrome.",
    //
    "Did you know that \"Kriegsgefangenschaft\" translated from German means \"captivity\" or \"prisoner-of-warness\"?",
    //
    "Wow, you made it all the way to event 10."
  };

}

namespace funny {

  void tidbit() {
    static std::size_t counter{};
    std::cout << "===================\n";
    std::cout << "TRIVIA\n\n" << tidbits.at(counter++) << "\n";
    std::cout << "===================\n";
    if ( counter == 10ul ) counter = 0ul;
  }

}
