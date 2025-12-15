// NamedEntityRecognitionDemo.cpp
// Identifying named entities and obtaining them as structured outputs.
#include <filesystem>
#include <iostream>
#include <vector>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;
using deitel::openai::json;

struct NamedEntity {
   std::string text;
   std::string type;
};

struct NamedEntities {
   std::vector<NamedEntity> entities;
};

// Ask model to return JSON describing named entities.
NamedEntities createStructuredOutputResponse(const std::string& model,
   const std::string& instructions, const std::string& input) {
   deitel::openai::OpenAIClient client{};

   // Prompt the model to emit JSON only.
   std::string combinedInstructions = instructions + R"(
      Return ONLY a JSON object of the form:
      {
        "entities": [
           { 
             "text": "...", 
             "type": "..." 
           },
           ...
        ]
      })";

   deitel::openai::ResponsesRequest params{
      .model = model,
      .instructions = combinedInstructions,
      .input = input
   };

   json response = client.responses().create(params);
   std::string jsonText = 
      deitel::openai::util::first_text_output(response);

   json parsed = json::parse(jsonText);

   NamedEntities result;
   
   for (const auto& e : parsed.at("entities")) {
      NamedEntity ne{
         .text = e.at("text").get<std::string>(),
         .type = e.at("type").get<std::string>()
      };
      result.entities.push_back(std::move(ne));
   }

   return result;
}

int main() {
   try {
      auto textPath = fs::path{".."} / "resources" / "web.txt";
      auto text = deitel::openai::util::read_text_file(textPath);

      std::println("GETTING NAMED ENTITIES FOR:");
      std::println("{}", text);
      std::println("");

      NamedEntities result = createStructuredOutputResponse(
         "gpt-5-mini",
         R"(You are an expert in named entity recognition with the
            OntoNotes Named Entity Tag Set.)",
         text);

      std::println("NAMED ENTITIES:");
      for (const auto& e : result.entities) {
         std::println("- text: {}, type: {}", e.text, e.type);
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
