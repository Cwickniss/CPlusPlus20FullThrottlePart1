// SpeechToTextDemo.cpp
// Transcribing audio files to text.
#include <filesystem>
#include <iostream>
#include <print>
#include "openai/openai.hpp"

namespace fs = std::filesystem;

// Perform an Audio Transcriptions API request and return transcript text.
std::string speechToText(
   const std::string& model, const fs::path& audioPath) {
      
   deitel::openai::OpenAIClient client{};

   deitel::openai::AudioTranscriptionRequest params{
      .model = model,
      .file_path  = audioPath.string(),
      .response_format = std::string{"text"}
   };

   // Assuming your Audio API returns raw string for `create_raw`
   // or json for `create`. Here we use a raw text helper.
   std::string transcript = 
      client.audio().transcriptions().create(params);

   return transcript;
}

int main() {
   try {
      auto audioPath = fs::path{".."} / "resources" / "02_00.m4a";

      if (!fs::exists(audioPath)) {
          throw std::runtime_error(
              "Audio file does not exist at: " + audioPath.string());
      }
      if (fs::file_size(audioPath) == 0) {
          throw std::runtime_error(
              "Audio file is empty: " + audioPath.string());
      }

      std::println("Waiting for Transcription...");
      std::string transcript =
         speechToText("gpt-4o-transcribe", audioPath);

      std::println("TRANSCRIPT:");
      std::println("{}", transcript);
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
