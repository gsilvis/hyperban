#!/usr/bin/python

import sympy
import mpmath

mpmath.mp.prec = 95
mpmath.mp.pretty = True

def normalize(a):
    return a/a[3]

mink_ident = sympy.matrices.Matrix(
  [[1,0,0,0],
   [0,1,0,0],
   [0,0,1,0],
   [0,0,0,-1]])

def mink_inner_product(a, b):
    return (a.transpose() * mink_ident * b) [0,0]

def outer_product(a, b):
    return a * b.transpose()

def reflect(a):
    denom = mink_inner_product(a, a)
    result = outer_product(a, a)
    result = result * mink_ident
    result = result * 2 / denom
    return sympy.matrices.eye(4) - result

origin = sympy.matrices.Matrix([0,0,0,1])

def midpoint(a, b):
    return a*(sympy.sqrt(mink_inner_product(b, b) * mink_inner_product(a, b))) + b*(sympy.sqrt(mink_inner_product(a, a) * mink_inner_product(a, b)))

def midpoint_zero(a):
    return midpoint(a, origin)

def translate(a, b):
    return reflect(midpoint(a, b)) * reflect(a)

#  Takes a number of RADIANS
def rotate(theta):
    return sympy.matrices.Matrix(
      [[sympy.cos(theta), -sympy.sin(theta), 0, 0],
       [sympy.sin(theta), sympy.cos(theta), 0, 0],
       [0, 0, 1, 0],
       [0, 0, 0, 1]])

def matratsimp(mat):
    return sympy.matrices.Matrix(mat.rows, mat.cols, lambda i, j: sympy.ratsimp(mat[i, j]))

#print "myrotate"
myrotate = rotate(2 * sympy.pi / 5)
#print myrotate
#print

#print "m"
m = sympy.Symbol("m")
#print m
#print

#print "mynorthvertex"
mynorthvertex = sympy.matrices.Matrix([0, m, 0, 1])
#print mynorthvertex
#print

#print "mywestvertex"
mywestvertex = sympy.matrices.Matrix([-m, 0, 0, 1])
#print mywestvertex
#print

#print "myeastvertex"
myeastvertex = sympy.matrices.Matrix([m, 0, 0, 1])
#print myeastvertex
#print

#print "translatesouth"
translatesouth = translate(mynorthvertex, origin)
#print translatesouth
#print

#print "translatenorth"
translatenorth = translate(origin, mynorthvertex)
#print translatenorth
#print

#print "mytransformation"
mytransformation = translatenorth * myrotate * translatesouth
mytransformation = matratsimp(mytransformation)
#print mytransformation
#print

#print "myresult"
myresult = normalize(mytransformation * mywestvertex)
myresult = matratsimp(myresult)
#print myresult
#print

#print "myerror"
myerror = myresult - myeastvertex
myerror = myerror[1]
#print myerror
myerror = sympy.simplify(myerror)
#print myerror
#print
k = mpmath.findroot((lambda xi: myerror.subs(m, xi)),
        (0.02, 0.9), solver=mpmath.calculus.optimization.Bisection)
print "Result:", k

