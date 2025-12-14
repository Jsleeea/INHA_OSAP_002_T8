#include <gtest/gtest.h>

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// We compile the production file into this test translation unit so we can access
// AvlSet without needing a separate header.
// CMake defines AVLSET_NO_MAIN for this target, so the main() inside AVLSet.cpp
// is excluded.
#include "src/AVLSet.cpp"

namespace {

// The assignment-style APIs print results to std::cout and return void.
// These helpers capture stdout so we can assert on the printed output.
std::string CaptureStdout(const std::function<void()> &fn) {
  std::ostringstream buffer;
  std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
  try {
    fn();
  } catch (...) {
    std::cout.rdbuf(old);
    throw;
  }
  std::cout.rdbuf(old);
  return buffer.str();
}

// Convert output like "  20\n" -> "20" (first whitespace-delimited token).
std::string OneToken(const std::string &out) {
  istringstream iss(out);
  string tok;
  iss >> tok;
  return tok;
}

}  // namespace

TEST(AVLSet_Prepare, SizeEmpty) {
  AvlSet s;
  const string out = CaptureStdout([&] { s.Size(); });
  EXPECT_EQ("0", OneToken(out)) << "Size() output was: " << out;
}


