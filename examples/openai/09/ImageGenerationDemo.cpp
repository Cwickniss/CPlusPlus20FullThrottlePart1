// ImageGenerationDemo.cpp
// Creating images from text prompts.
#include <filesystem>
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;

void createImage(const std::string& model, const std::string& prompt,
   const fs::path& path, const std::string& size = "auto") {
      
   deitel::openai::OpenAIClient client{};

   deitel::openai::ImagesGenerateRequest params{
      .model = model,
      .prompt = prompt,
      .size = size
   };

   auto imagesResponse = client.images().generate(params);

   const auto& image = imagesResponse.at("data").at(0);
   std::string b64 = image.at("b64_json").get<std::string>();

   deitel::openai::util::save_base64_to_file(b64, path);
   std::println("Image stored in:\n{}", path.string());
}

int main() {
   try {
      auto outputsPath = fs::path{".."} / "resources" / "outputs"; 
      
      std::println("IMAGE-GENERATION DEMO");
      std::string prompt = R"(
         Havanese dog as a Japanese anime character
         in neon colors against a black background)";
      std::println("Generating image for:\n{}", 
          strip_leading_whitespace(prompt));
      createImage("gpt-image-1", prompt, 
         outputsPath / "HavaneseAnime.png");

      prompt = "Havanese dog in the style of Vincent van Gogh";
      std::println("\nGenerating image for:\n{}", 
          strip_leading_whitespace(prompt));
      createImage("gpt-image-1", prompt, 
         outputsPath / "HavaneseVanGogh.png");

      prompt = "Havanese dog in the style of Leonardo da Vinci";
      std::println("\nGenerating image for:\n{}", 
          strip_leading_whitespace(prompt));
      createImage("gpt-image-1", prompt, 
         outputsPath / "HavaneseDaVinci.png");
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
