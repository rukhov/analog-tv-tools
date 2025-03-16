import os
from pathlib import Path
import tempfile
import re
import frame_checker as fc

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
    
    #check colors

    vc = fc.VideoColorChecker(str(aviOutputFile), 1) 
    assert vc.isOpen() == True

    tolerance = 20  # Tolerance value for color comparison

    assert vc.CheckAt( (210, 350) , (230, 230, 230) , tolerance) == True

    assert vc.CheckAt( (360, 350) , (230, 230, 0  ) , tolerance) == True

    assert vc.CheckAt( (500, 350) , (0,   230, 230) , tolerance) == True
    
    assert vc.CheckAt( (650, 350) , (0,   240, 0  ) , tolerance) == True

    assert vc.CheckAt( (800, 350) , (240, 0,   240) , tolerance) == True

    assert vc.CheckAt( (960, 350) , (240, 0,   0  ) , tolerance) == True

    assert vc.CheckAt( (1100,350) , (0,   0,   240) , tolerance) == True



    assert vc.CheckAt( (1100,850) , (0,   0,   0  ) , tolerance) == True

    assert vc.CheckAt( (600, 850) , (40,  0,   80 ) , tolerance) == True

    assert vc.CheckAt( (400, 850) , (230, 230, 230) , tolerance) == True

    assert vc.CheckAt( (220, 850) , (0,    30,  60) , tolerance) == True
