// SentimentAnalysisDemo.cpp
// Analyzing the sentiment of a transcript.
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

int main()
{
   try {
      fs::path transcriptPath = 
          fs::path{".."} / "resources" / "transcript.txt";
      std::string transcript =
         deitel::openai::util::read_text_file(transcriptPath);

      std::println("ANALYZE SENTIMENT");
      std::string sentiment = createResponse(
         "gpt-5-mini",
         R"(You are a sentiment-analysis expert. Determine the provided
            transcript's sentiment. Explain your analysis.)",
         transcript);

      std::println("{}", sentiment);
      std::println("");
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
