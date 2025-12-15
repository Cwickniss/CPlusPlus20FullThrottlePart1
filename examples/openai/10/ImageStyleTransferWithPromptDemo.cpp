// ImageStyleTransferWithPromptDemo.cpp
// Style transfer via the Images API edit capability.
#include <filesystem>
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;

void restyleWithImagesAPI(const std::string& model,
   const fs::path& imagePath, const fs::path& outputPath,
   const std::string& size, const std::string& stylePrompt) {
      
   deitel::openai::OpenAIClient client{};

   deitel::openai::ImageEditRequest params{
      .model = model,
      .image_path = imagePath.string(),
      .prompt = stylePrompt,
      .size = size
   };

   auto imagesResponse = client.images().edit(params);
   std::string b64 = 
      imagesResponse.at("data").at(0).at("b64_json").get<std::string>();

   deitel::openai::util::save_base64_to_file(b64, outputPath);
   std::println("Restyled image stored in:\n{}", outputPath.string());
}

int main() {
   try {
      std::println("IMAGE-STYLE-TRANSFER-VIA-PROMPT DEMO");
      
      std::string styleTransferPrompt = R"(
         Restyle the input photo into a vibrant swirling impasto 
         painting inspired by post-impressionist brushwork.  
    
         Medium/technique: thick acrylic paint with bold 
         palette-knife swipes and loaded brush strokes; swirling 
         arcs, rhythmic curves, comma-shaped dabs, and layered 
         ridges that give a tactile sheen (impasto).  
    
         Palette: luminous cobalt and ultramarine blues as the 
         dominant field; strong accents of golden yellow and amber; 
         secondary touches of teal and turquoise; minimal orange and 
         white highlights for contrast.  
    
         Composition: shallow depth, decorative and poster-flat; 
         energetic all-over brushwork that simplifies the subject 
         into flowing, abstracted shapes; swirls and curved strokes
         define contours and fur without precise detail.  
    
         Lighting & finish: very saturated, high contrast, minimal 
         shading; painterly, non-photorealistic.  
    
         Do/Don't: maintain subject recognizability by silhouette and 
         major proportions; no text; no signature; avoid fine line 
         drawing or photoreal textures.)";
         
      // paths
      auto resourcesPath = fs::path{".."} / "resources"; 
      auto imagePath = resourcesPath / "sunset.jpg";
      auto outputPath = resourcesPath / "outputs" / "styled_sunset.png";

      // restyle the image
      restyleWithImagesAPI("gpt-image-1", imagePath, outputPath,
         "1536x1024", styleTransferPrompt);
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

