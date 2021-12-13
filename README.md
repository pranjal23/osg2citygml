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

## Cite ##
If you use this code please cite it as the following from the arxiv publication (http://arxiv.org/abs/2109.14643)
@mastersthesis{swarup2016semiautomatic,
      title={Semi-automatic conversion from OSG to CityGML}, 
      author={Pranjal Swarup},
      year={2016},
      institution = {Nanyang Technological University}
}
