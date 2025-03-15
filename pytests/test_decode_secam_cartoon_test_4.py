import os
from pathlib import Path, PosixPath
import zipfile
import pytest

@pytest.fixture(scope="session")
def assetsPath(pytestconfig):
    return pytestconfig.getoption("assetsPath")

@pytest.fixture(scope="session")
def projectPath(pytestconfig):
    return pytestconfig.getoption("projectPath")

def test_decode(assetsPath, projectPath):

    decoderPath = assetsPath + '/bin/analog-tv-tools-cli'

    print("===========> PARAMS: projectPath: ", projectPath)

    try: 

        tmpFolder = Path('/tmp')
        inputFile = Path( projectPath + '/test-data/cartoon-test-video-4-36000000Hz.i16')
        aviOutputFile = tmpFolder.joinpath(inputFile.stem).with_suffix(".avi")
        #jpegOutputFileBase = tmpFolder.joinpath(inputFile.stem)

        tmpFolder.mkdir(0o777, True, True)

        child = os.popen(decoderPath + 
                " --input-type i16"
                " --input-data-type cvbs"
                " --output-type avi"
                " --standard secam"
                " --input-file " + str(inputFile) +
                " --output-file " + str(aviOutputFile) +
                " --sample-rate 36000000"
                " --dc-correction .0"
                " --amplification 1.333"
                " --max-frames 0"
                )
        
        out = child.read()
        
        print(out)

        index = out.find("Total frame number: 32")

        assert index > 0

    except Exception as err:
        print(f"Unexpected {err=}, {type(err)=}")
        assert False
        raise
