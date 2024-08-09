## Adding Tests

To add a new test:

1. Duplicate `UnitTests\TestTemplate.cpp` and rename it appropriately.
2. Ensure the new test file is located in the `UnitTests` directory.
3. Update `tests\CMakeLists.txt` by adding the path of your new test file to the `SOURCES` list.
4. Build `Puma` and run the tests using `./build/bin/Puma-tests`.