// Fig. 19.13: ModerationDemo.cpp
// Using the OpenAI Moderation API to check for offensive content.
#include <iostream>
#include <string>
#include <print>
#include "openai/openai.hpp"

using deitel::openai::json;


// Calls the Moderations API with the specified model and input text.
json checkText(const std::string& text) {
   deitel::openai::OpenAIClient client{};
      
   deitel::openai::ModerationRequest params{
      .model = "omni-moderation-latest",
      .input = text  
   };

   return client.moderations().create(params);
}

// Displays a summary of the moderation result for a single input string.
void displayModerationResults(const json& result) {
   const auto& firstResult = result.at("results").at(0);

   bool flagged = firstResult.at("flagged").get<bool>();

   if (!flagged) {
      std::println("Not flagged for offensive content.\n");
      return;
   }

   // If flagged, show every category and its score
   const auto& categories = firstResult.at("categories");
   const auto& categoryScores = firstResult.at("category_scores");

   std::println("FLAGGED FOR OFFENSIVE CONTENT");
   std::println("DETAILS:\n");

   for (auto it = categories.begin(); it != categories.end(); ++it) {
      const std::string name = it.key();
      bool value = it.value().get<bool>();

      // Score lookup (Java: moderation.getCategoryScores().get(name))
      double score = 0.0;
      if (categoryScores.contains(name) &&
          categoryScores.at(name).is_number()) {
         score = categoryScores.at(name).get<double>();
      }

      std::println("  {:<22}: {:<5}  score: {:.3f}",
         name, value ? "yes" : "no", score);
   }

   std::println("");
}

int main() {
   try {
      std::println("MODERATION DEMO");
      std::println(
         "Enter text to analyze, or type 'exit' (lowercase) to quit.\n");

      std::string inputText;

      while (true) {
         std::print("Enter text: ");
         if (!std::getline(std::cin, inputText)) {
            // EOF, just terminate
            break;
         }

         // Logic mirrors Java:
         // - lowercase "exit" terminates the program
         if (inputText == "exit") {
            break;
         }

         // - empty input is ignored (no API call), just reprompt
         if (inputText.empty()) {
            continue;
         }

         json result = checkText(inputText);
         displayModerationResults(result);
      }
   }
   catch (const std::exception& ex) {
      std::println(std::cerr, "Error: {}", ex.what());
      return 1;
   }
}


/**************************************************************************
 * (C) Copyright 1992-2025 by Deitel & Associates, Inc. and               *
 * Pearson Education, Inc. All Rights Reserved.                           *
 *                                                                        *
 * DISCLAIMER: The authors and publisher of this book have used their     *
 * best efforts in preparing the book. These efforts include the          *
 * development, research, and testing of the theories and programs        *
 * to determine their effectiveness. The authors and publisher make       *
 * no warranty of any kind, expressed or implied, with regard to these    *
 * programs or to the documentation contained in these books. The authors *
 * and publisher shall not be liable in any event for incidental or       *
 * consequential damages in connection with, or arising out of, the       *
 * furnishing, performance, or use of these programs.                     *
 *************************************************************************/
