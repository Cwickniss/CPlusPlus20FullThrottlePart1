// DescribeImageDemo.cpp
// Getting accessibility descriptions of images...,
#include <filesystem>
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;
using deitel::openai::json;

// Perform a Responses request for an image description.
// Uses a single data: URL image as input.
std::string describeImage(const std::string& model,
   const std::string& prompt, const fs::path& imagePath) {
   deitel::openai::OpenAIClient client{};

   // Build a data: URL for the image.
   std::string dataUrl =
      deitel::openai::util::make_data_url_from_file(imagePath);

   // Responses "input" for vision: single user message with image + prompt.
   // NOTE: For the Responses API, image_url must be a STRING,
   // not an object { "url": ..., "detail": ... }.
   json image_content = json::object({
      {"type", "input_image"},
      {"image_url", dataUrl}
   });

   json text_content = json::object({
      {"type", "input_text"},
      {"text", prompt}
   });

   json message = json::object({
      {"role", "user"},
      {"content", json::array({text_content, image_content})}
   });

   json input = json::array({message});

   deitel::openai::ResponsesRequest params{
      .model = model,
      .instructions = 
         R"(You are an expert at creating accessible image descriptions
            per the World Wide Web Consortium's Web Content
            Accessibility Guidelines (WCAG). Given an image, provide
            alt text and a detailed description for people who are
            blind or have low vision. Identify objects accurately.)",
      .input = input
   };

   json response = client.responses().create(params);
   return deitel::openai::util::first_text_output(response);
}

int main() {
   try {
      auto resoursesPath = fs::path{".."} / "resources";

      // Aruba sunset photo
      std::println("ACCESSIBLE DESCRIPTION OF A PHOTO");
      std::string photoDesc = describeImage(
         "gpt-5-mini",
         "Provide an accessible description of this Aruba sunset photo.",
         resoursesPath / "sunset.jpg");
      std::println("{}", photoDesc);
      std::println("");

      // UML diagram
      std::println("ACCESSIBLE DESCRIPTION OF A DIAGRAM");
      std::string diagramDesc = describeImage(
         "gpt-5-mini",
         "Provide an accessible description of this UML activity diagram.",
         resoursesPath / "ForLoop.png");
      std::println("{}", diagramDesc);
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
