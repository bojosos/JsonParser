#include "interpreter.h"
#include <iostream>

int main()
{
  std::string line;
  Interpreter interpreter;

  std::vector<const char*> cmds = {"new",
                                   "create test member1 {'1,2,3': 69}",
                                   "create test member2 {'1,2,3': 69}",
                                   "create test member3 {'1,2,3': 69}",
                                   "create test2 member1 [33.4e-5]",
                                   "create test3 member1 [{'test': 33.4e-5}]",
                                   "move test/member1 test2",
                                   "edit test2/member1 \"asd\"",
                                   "edit test/member1 {\"hmm\": true}",
                                   "create test/member1 test {\"hmm\": false}",
                                   "remove test/member1",
                                   "savecompact tests/save.json",
                                   "savesearchcompact test tests/search.json"};
  for (auto* cmd : cmds)
  {
    try
    {
      interpreter.process(cmd);
    }
    catch (const std::exception& ex)
    {
      std::cerr << ex.what() << std::endl;
    }
  }
}