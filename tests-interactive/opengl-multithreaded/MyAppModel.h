/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef MYAPPMODEL_H_
#define MYAPPMODEL_H_

#include <vector>

/**
 * The MyAppModel class stores and maintains all the application data.  In this
 * example of a rotating cube it contains vertices, normals, colors, and a
 * model matrix.
 */
class MyAppModel {
public:
	MyAppModel();
	~MyAppModel();

	float modelMatrix[16];
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> colors;
};

#endif

