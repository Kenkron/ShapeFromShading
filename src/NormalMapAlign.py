#!/usr/bin/env python
"""A simple script to try to align two sets of vectors"""

"""Usage:"""

"""    ./NormalMapAlign.py CorrectVectors ComputedVectors [NormalMap]"""

"""It is assumed that both sets of vectors can be rotated to """
"""align with each other.  This script is designed to test that """
"""by creating a rotation matrix to align the average of the vectors in each"""
"""data set, and applying this transformation to all members of the"""
"""second data set (and, optionally, a normal map)."""

import numpy
import scipy
from scipy import misc
import sys



def rotmat(vec1, vec2):
    """Returns a transformation matrix (as a numpy.array)"""
    """that will rotate unit vector vec1 onto unit vector vec2"""

    """vec1 and vec2 are both numpy.array objects"""
    v = numpy.cross(vec1,vec2)
    s = numpy.linalg.norm(v)
    c = vec1.dot(vec2)
    I = numpy.identity(3)
    Vx = numpy.array([[0, -v[2], v[1]],
                      [v[2], 0, -v[0]],
                      [-v[1], v[0], 0]])
    return I+Vx+Vx.dot(Vx)*(1-c)/(s*s)

def transformVectorMap(image, matrix):
    """Applies the given matrix to every vector in image. """
    """Image is an LxMxn numpy.array """
    """where n is the length of each vector"""
    for r in range(image.shape[0]):
        for c in range(image.shape[1]):
            image[r,c,:]=matrix.dot(image[r,c,:])

def main(correctDirsFilename, calculatedDirsFilename,
         calculatedNormalMap=None):
    correct = []
    with open(correctDirsFilename) as correctFile:
        for line in correctFile:
            correct.append(numpy.array([float(x) for x in line.split()]))
    calculated = []
    with open(calculatedDirsFilename) as calculatedFile:
        for line in calculatedFile:
            calculated.append(numpy.array([float(x) for x in line.split()]))
    diff = 0;
    for i in range(len(correct)):
        diff += numpy.linalg.norm(correct[i]-calculated[i])**2
    print("Original Diff: "+str(diff))
    avgcalc = numpy.zeros(3)
    for vec in calculated:
        avgcalc = avgcalc+vec
    avgcalc = avgcalc/numpy.linalg.norm(avgcalc)
    avgcorrect = numpy.zeros(3)
    for vec in correct:
        avgcorrect = avgcorrect+vec
    avgcorrect = avgcorrect/numpy.linalg.norm(avgcorrect)
    fix = rotmat(avgcalc,avgcorrect);
    print("Rotation Matrix:"+str(fix))
    diff = 0;
    for i in range(len(correct)):
        diff += numpy.linalg.norm(correct[i]-fix.dot(calculated[i]))**2
    print("Reoriented Diff: "+str(diff))
    if not calculatedNormalMap == None:
        normalMap = misc.imread(calculatedNormalMap)
        normalMap = normalMap/128.0-1
        transformVectorMap(normalMap, fix)
        normalMap = (normalMap+1)*128
        misc.imsave("fix.png", normalMap)
    
if __name__ == "__main__":
    normalmap = None
    if len(sys.argv) == 4:
        normalmap = sys.argv[3]
    if len(sys.argv) in range(3,5):
        main(sys.argv[1],sys.argv[2],normalmap);
