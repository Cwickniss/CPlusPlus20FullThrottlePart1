// TextToSpeechDemo.cpp
// Converting text to speech and saving as an audio file.
#include <iostream>
#include <filesystem>
#include <print>
#include <vector>
#include "openai/openai.hpp"

namespace fs = std::filesystem;

void textToSpeech(const std::string& text,
   const std::string& guidance, const std::string& voice, 
   const fs::path& path) {
      
   deitel::openai::OpenAIClient client{};

   deitel::openai::AudioSpeechRequest params{
      .model = "gpt-4o-mini-tts",
      .instructions = guidance,
      .input = text,
      .voice = voice
   };

   // Assuming SpeechAPI::create returns bytes.
   std::vector<unsigned char> audioBytes =
      client.audio().speech().create(params);

   deitel::openai::util::write_binary_file(path, audioBytes);
   std::println("Wrote audio to {}\n", path.string());
}

int main() {
   try {
      auto outputsPath = fs::path{".."} / "resources" / "outputs"; 

      std::println("TEXT-TO-SPEECH DEMO");
      std::println("Synthesizing happy English speech...");
      std::string english{
         "Today was a beautiful day. Tomorrow looks like bad weather."};
      textToSpeech(english, "Speak in a happy tone.", "ash",
         outputsPath / "english_happy.mp3");

      std::println("Synthesizing evil English speech...");
      textToSpeech(english, 
         "Speak as an evil supervillain and end with an evil laugh.", 
         "ash", outputsPath / "english_evil.mp3");

      std::println("Synthesizing Spanish speech...");
      std::string spanish{
         "Hoy fue un día hermoso. Mañana parece que habrá mal tiempo."};
      textToSpeech(spanish, "", "ash", outputsPath / "spanish.mp3");

      std::println("Synthesizing Japanese speech...");
      std::string japanese{  
         "今日はとてもいい天気でした。明日は天気が悪そうです。"};
      textToSpeech(japanese, "", "ash", outputsPath / "japanese.mp3");
   }
   catch (const std::exception& ex) {
      std::cerr << "Error: " << ex.what() << "\n";
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
