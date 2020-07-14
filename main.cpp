#include "json.h"
#include "parse_requests.h"
#include "test_runner.h"
#include "tests.h"
#include "processor.h"

int main() {
  TestAll();
  bool isJson = true;
  RunProcessor(isJson);
	return 0;
}
