// ImageToImageStyleTransferDemo.cpp
// Image-to-Image style transfer via the Responses API.
#include <filesystem>
#include <format>
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;
using deitel::openai::json;

json inputImageFromPath(const fs::path& path) {
   std::string dataUrl =
      deitel::openai::util::make_data_url_from_file(path);

   // For the Responses API, image_url must be a STRING, not an object.
   return json{
      {"type", "input_image"},
      {"image_url", dataUrl}
   };
}

void restyleWithResponsesAPI(const std::string& model,
   const fs::path& toImagePath, const fs::path& fromImagePath,
   const fs::path& outputPath, const std::string& size,
   const std::string& moreInfo)
{
   deitel::openai::OpenAIClient client{};

   std::string prompt =
      "Apply the style of the second image to the first. "
      "Keep the subject's identity and layout. "
      "Generate the result at size " + size + ".";
   if (!moreInfo.empty()) {
      prompt += " Additional instructions: " + moreInfo + ".";
   }

   json text_content = {
      {"type", "input_text"},
      {"text", prompt}
   };

   json target_image = inputImageFromPath(toImagePath);
   json style_image  = inputImageFromPath(fromImagePath);

   json message = {
      {"role", "user"},
      {"content", json::array({text_content, target_image, style_image})}
   };

   json input = json::array({message});

   json tools = json::array({ json{{"type", "image_generation"}} });

   deitel::openai::ResponsesRequest params{
      .model = model,
      .input = input,
      .tools = tools
   };
   
   json response = client.responses().create(params);

   std::string b64 =
      deitel::openai::util::first_image_base64_output(response);

   deitel::openai::util::save_base64_to_file(b64, outputPath);
   std::println("Saved restyled image:\n{}", outputPath.string());
}

int main() {
   try {
      // Paths to the target, source and output images    
      auto resourcesPath = fs::path{".."} / "resources"; 
      auto toImagePath = resourcesPath / "sunset.jpg";
      auto fromImagePath = resourcesPath / "style1.jpg";
      auto outputPath = resourcesPath / "outputs" / 
         "styled_sunset_from_style1_image.png";

      // restyle the image
      restyleWithResponsesAPI("gpt-5-mini", toImagePath, fromImagePath,
         outputPath, "1536x1024", "");
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
