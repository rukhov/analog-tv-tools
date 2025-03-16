import pytest
import sys

def pytest_addoption(parser):
    parser.addoption("--projectPath", action="store", default="")
    parser.addoption("--assetsPath", action="store", default="")
    parser.addoption("--executablesPath", action="store", default="")

def _executableSuffix():
    if sys.platform == 'win32':
        return '.exe'
    else:
        return ''

@pytest.fixture(scope="session")
def assetsPath(pytestconfig):
    return pytestconfig.getoption("assetsPath")

@pytest.fixture(scope="session")
def projectPath(pytestconfig):
    return pytestconfig.getoption("projectPath")

@pytest.fixture(scope="session")
def executablesPath(pytestconfig):
    return pytestconfig.getoption("executablesPath")

@pytest.fixture(scope="session")
def executableSuffix():
    return _executableSuffix()

@pytest.fixture(scope="session")
def decoderPath(pytestconfig):
    return pytestconfig.getoption("executablesPath") + '/analog-tv-tools-cli' + _executableSuffix()
