// TextTranslationDemo.cpp
// Translating text between languages.
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;
using deitel::openai::json;

std::string translate(const std::string& model,
   const std::string& textToTranslate, const std::string& language) {
      
   deitel::openai::OpenAIClient client{};

   deitel::openai::ResponsesRequest params{
      .model = model,
      .instructions = "Translate input to " + language,
      .input = textToTranslate
   };

   json response = client.responses().create(params);
   return deitel::openai::util::first_text_output(response);
}

int main()
{
   try {
      std::string english{
         "Today was a beautiful day. Tomorrow looks like bad weather."};
      std::println("ORIGINAL: {}\n", english);

      std::println("Translating English to Spanish...");
      std::string spanish{translate("gpt-5", english, "Spanish")};
      std::println("SPANISH: {}\n", spanish);

      std::println("Translating English to Japanese...");
      std::string japanese{translate("gpt-5", english, "Japanese")};
      std::println("JAPANESE: {}\n", japanese);

      std::println("Translating Spanish to English...");
      std::println("SPANISH TO ENGLISH: {}\n",
         translate("gpt-5", spanish, "English"));

      std::println("Translating Japanese to English...");
      std::println("JAPANESE TO ENGLISH: {}\n",
         translate("gpt-5", japanese, "English"));
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
