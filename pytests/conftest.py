def pytest_addoption(parser):
    parser.addoption("--projectPath", action="store", default="")
    parser.addoption("--assetsPath", action="store", default="")
