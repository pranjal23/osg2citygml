# **OSG2CityGML** #


Code for my 2016 MSc Dissertation: Semi-automatic conversion from OSG to CityGML

A tool to add semantics to a 3D Mesh and convert it into CityGML format.

Dependency:
OpenSceneGraph (Added as submodule)
Qt5


## SETUP  ##
1. Git clone this repo
2. cd OpenSceneGraph
    git submodule update --init --recursive
3. cmake, then make install OpenSceneGraph remote included in the project
4. Qt5 Project - can be built using QT Creator or CMake from command line

The following work on Human-Computer interaction was derived from my Thesis:

Jayaraman et al. Interactive Labeling for Generation of CityGML Building Models from Meshes. InIntelligent Scene Modeling and Human-Computer Interaction 2021 (pp. 147-163). Springer, Cham.
https://link.springer.com/chapter/10.1007/978-3-030-71002-6_9
