//#include "json.h"
#include "parse_requests.h"
#include "test_runner.h"
#include "tests.h"
#include "processor.h"

int main() {
  TestAll();
  RunProcessor();
	return 0;
}
