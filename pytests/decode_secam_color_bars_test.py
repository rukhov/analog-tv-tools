import os
from pathlib import Path
import tempfile
import re

def test_decode(projectPath, decoderPath):

    tmpFolder = Path( tempfile.gettempdir() )
    inputFile = Path( projectPath + '/test-data/cvbs-secam-colorbars-36000000Hz.i16')
    aviOutputFile = tmpFolder.joinpath(inputFile.stem).with_suffix(".avi")

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
            " --max-frames 2"
            )
    
    out = child.read()

    m = re.search(r'Total frame number: (\d+)', out)

    assert int(m[1]) == 2
