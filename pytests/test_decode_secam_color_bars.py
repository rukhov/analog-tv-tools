import os
from pathlib import Path, PosixPath
import zipfile
import pytest

@pytest.fixture(scope="session")
def decoderPath(pytestconfig):
    return pytestconfig.getoption("decoderPath")

def test_decode(decoderPath):

    try: 

        tmpFolder = Path('/tmp')
        sourceDir = os.path.dirname(Path(__file__).resolve())
        inputFile = Path( sourceDir + '/../test-data/cvbs-secam-color-bars.f32')
        cvbsDataFile = inputFile
        aviOutputFile = tmpFolder.joinpath(inputFile.stem).with_suffix(".avi")
        jpegOutputFileBase = tmpFolder.joinpath(inputFile.stem)

        tmpFolder.mkdir(0o777, True, True)

        child = os.popen(decoderPath + 
                " --input-type raw"
                " --input-data-type cvbs"
                " --output-type avi"
                " --standard secam"
                " --input-file " + str(cvbsDataFile) +
                " --output-file " + str(aviOutputFile) +
                " --sample-rate 16000000"
                " --dc-correction .0"
                " --amplification 1.333"
                " --max-frames 0"
                )
        
        out = child.read()
        
        print(out)

        index = out.find("Total frame number:")

        assert index > 0

    except Exception as err:
        print(f"Unexpected {err=}, {type(err)=}")
        assert False
        raise
