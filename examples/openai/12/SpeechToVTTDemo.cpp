// SpeechToVTTDemo.cpp
// Transcribing audio files to WebVTT caption files.
#include <filesystem>
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;

std::string speechToVTT(const fs::path& audioPath) {
   deitel::openai::OpenAIClient client{};

   deitel::openai::AudioTranscriptionRequest params{
      .model = "whisper-1",
      .file_path = audioPath.string(),
      .response_format = std::string{"vtt"}
   };

   return client.audio().transcriptions().create(params);
}

int main() {
   try {
      auto resourcesPath{fs::path{".."} / "resources"};
      auto audioPath{resourcesPath / "01_01.m4a"};
      auto outputPath{resourcesPath / "outputs" / "01_01.vtt"};

      std::println("Transcribing audio and creating VTT captions...");
      std::string vtt{speechToVTT(audioPath)};

      deitel::openai::util::write_text_file(outputPath, vtt);
      std::println("VTT stored in:\n{}", outputPath.string());
      std::println("{}", vtt);
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
