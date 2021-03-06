Establish Matrix Phase {#establishshapetypes}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Initialization)

## Description ##
This Filter allows the user to assign a specific shape type to each phase of their synthetic structure. This filter should
be inserted **BEFORE** any packing filters are added. The filter is typically added just after the Initialize Synthetic Volume
filter is added. The various shape types are
+ Ellipsoid
+ Super Ellipsoid
+ Cube Octahedron
+ Cylinder

## Parameters ##
| Name             | Type | Notes |
|------------------|------|-------|
| Phase Types Array | UInt32 Array | Any array that has the correct number of tuples will work, i.e., the number of tuples is the same as the number of phases |


## Required Arrays ##

An Array that has the correct number of phases indentified. For example, one could use the "Crystal Structures" array if needed

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) |  | Intialize Synthetic Volume (SyntheticBuilding), Generate Ensemble Statistics (Statistics) |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | ShapeTypes | UInt32 array of the shape types. Each value in the array represents one of the enumerations above. |  |

## Authors ##

**Copyright:** 2014 Michael A. Groeber (AFRL),2014 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

