// TextSummarizationDemo.cpp
// Summarizing a transcript as an abstract paragraph and key points.
#include <filesystem>
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;
using deitel::openai::json; 

// Perform a Responses API request and return the first text output.
std::string createResponse(const std::string& model,
   const std::string& instructions, const std::string& input) {
   deitel::openai::OpenAIClient client{};

   // configure the request parameters
   deitel::openai::ResponsesRequest params{
      .model = model,
      .instructions = instructions,
      .input = input
   };

   json response = client.responses().create(params);
   return deitel::openai::util::first_text_output(response); // to_string(response);
}

int main() {
   try {
      fs::path transcriptPath = 
         fs::path{".."} / "resources" / "transcript.txt";
      std::string transcript =
         deitel::openai::util::read_text_file(transcriptPath);

      // Summary abstract
      std::println("CREATE A SUMMARY ABSTRACT OF A TRANSCRIPT");
      std::string summaryAbstract = createResponse(
         "gpt-5-mini",
         R"(Given a C++ technical presentation's transcript, create a
            summary abstract paragraph. Use straightforward sentences.
            Spell language features and method names correctly.
            Do not refer to the speaker.)",
         transcript);
      std::println("{}", summaryAbstract);
      std::println("");

      // Key points
      std::println("EXTRACT KEY POINTS FROM A TRANSCRIPT");
      std::string keyPoints = createResponse(
         "gpt-5-mini",
         R"(Given a C++ technical presentation's transcript,
            return a numbered list of the top 5 key points.)",
         transcript);
      std::println("{}", keyPoints);
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
