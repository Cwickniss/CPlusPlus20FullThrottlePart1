// CodeGenerationDemo.cpp
// Generating code from a text description.
#include <filesystem>
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;
using deitel::openai::json;

std::string createResponse(const std::string& model,
   const std::string& instructions, const std::string& input) {
   deitel::openai::OpenAIClient client{};

   deitel::openai::ResponsesRequest params{
      .model = model,
      .instructions = instructions,
      .input = input
   };

   json response = client.responses().create(params);
   return deitel::openai::util::first_text_output(response);
}

int main() {
   try {
      std::println("CODE GENERATION DEMO");

      std::string code = createResponse("gpt-5",
         "You are an expert C++23 programmer.",
         R"(Create a C++23 program that simulates rolling a die 
            600,000,000 times using all available CPU cores. Avoid loops. 
            Summarize the frequencies and nicely format them right-aligned
            under the column heads "Face" and "Frequency". Return only the
            code with no markdown formatting. Use three-space indents and 
            a maximum code line length of 74 characters.)");

      std::println("{}", code);
      deitel::openai::util::write_text_file(
         fs::path{".."} / "resources" / "outputs" / "RollDie.cpp", code);
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

