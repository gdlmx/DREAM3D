DREAM3D Development ToDo List {#todo}
===============

This is the list of things to do. It is written with the Markdown style so it can be integrated with the other documentation pages.

## Show Stopping Bugs ##

## Documentation Issues ##


### Groeber Documentation
+ Missing Documenation File for IOFilters/ImportImageStack
+ write-up contributors section for website


### Jackson Priority ###
+ Usermanual::Import/Export/Favorite Pipelines
+ Usermanual::User Interface Introduction
+ Tutorial::Small IN100 for Docs/Website
+ Tutorial::Synthetic Generation for Docs/Website
+ Tutorial::Visualization Techniques for Docs/Website
+ Tutorial::Creating Ensemble Statistics for Docs/Website
+ Tutorial::Importing Orientation Data for Docs/Website
+ FileFormats::Xdmf files for Docs/Website
+ FileFormats::Native DREAM3D format
+ FileFormats::Other supported file types


### User Centric
+ Update/Create a user manual for the stats generator - last parts need pasting
+ Update documentation file structure now that users can specify array names

### Developer Centric ###
+ How to Incorporate documentation for plugins into the main help system
+ Add documentation on how to add new filter directly into DREAM3D
+ Add documentation on how to add new filter group directly into DREAM3D

### Ideas/Wish List ###
+ Add "help" button to each filter that would display the help for that specific filter
+ Comprehensive search of the help system (Possibly QAssistant based?)



## Still to Do ##
+ Add checks in MinSize and MinNeighbor filters to exit gracefully if minimum is larger than largest grain/num neighbors
+ Add in Xdmf wrapper for H5Ebsd so they can be easily viewed in ParaView
+ On the raw binary reader allow user to optionally over ride the origin and spacing values with the entries in the filter.
+ Create a Parameter Type that suggests Internal Array names but allows the user to type their own array name
+ Create filter to generate Abaqus input files, Albert Cerrone or Curt Bronkhorst to help with this
+ Write program to read in Pipeline Files on the command line
+ Writing NonConformal Surface Mesh does not take into account values whose sign is dependent on the winding of the triangle.
+ Sometimes dragging a filter from the filter list to the PipelineView area will cause a crash.
+ Filter that can execute any program on the users computer
+ Add in the HyperSlabArray class to use for some of the SurfaceMeshing Triangle/Node data
+ Fix the deformation statistics filter to adapt to new structure of transmission metrics
+ Add Filter to initialize the Ensemble Map based on a user defined array such as Phase Ids.
+ Integrate Lisa Chan's Twin Insertion Code - Have Code From Joe Tucker
+ Change Field to Region in all codes
+ Add/Modify filter to note if grain is a twin, parent or untwinned
+ Add ability to define twins of interest on merge twin filter
+ Add ability to append to pipeline folder in .dream3d file when starting a new pipeline with the read dream3d filter
+ Add ability to flag regions as 'defects' or a new phase in the fill bad data filter
+ Look into incorporating genetic algorithms in the packing algorithms of the synthetic builder
+ Include a filter to calculate 2-pt statistics
+ Look into using 2-pt statistics to advance the packing algorithms in the synthetic builder
+ Add a filter to calculate Kearn's number
+ Look into matching the Kearn's number when matching cystallography in the synthetic builder
+ Add a cleanup filter that considers the orientation (nearest) or kikuchi pattern of neighboring voxels when assigning bad voxels
+ Add GBCD calculation filters from G. Rohrer
+ Add a filter to calculate 'clustering' statistics on 'marked' features
+ Look into creating multiple maps at each level (i.e. parent and grain filed maps)
+ Modify ODF/PF generation in stats generator to better combine random baseline with user inputs (also degree/radian checkbox)
+ Add/Finish filter to calculate slice-slice misorientation (in terms of axis-angle in sample ref frame)
+ Add filter to calculate microtexture function or equivalent
+ Look into creating a reverse Saltykov/Sahagian filter to predict 3D grain size distribution from 2D data
+ Add filter to merge twins using surface mesh (5 parameter)
+ Add virtual sectioning filter
+ Add/Finish filter to 'standardize' euler angles within a grain
+ Look into creating a filter to color orientations using C. Shuh's color scale
+ Add mesh quantification filter (code from Tony)
+ Create filter to generate surface mesh from volume mesh
+ Add 3D linear intercept calculation filter (C. Hartley filter)
+ Design protocol to learn user's 'ontology'
+ Add filter to perform Tony's hot spot analysis
+ Add filter to combine far field data with near field data from HEDM
+ Remove all "under-the-hood" calls to other filters
+ Allow the user to specify the phase for minimum neighbors filter
+ Look at adding a mirror option to the rotate sample reference frame filter
+ Add the option for the user to set the name of a copied array (from field to cell)
+ Add filter to determine spherical correlation length from each grain centroid
+ Add mean filter in orientation space
+ Add mean filter on scalar array
+ Add median filter on scalar array
+ Work through all filters and change single for loops over totalPoints to triple loops over x-,y- and zPoints if the coordinates need to be calculated
+ Add splash Screen similar to ParaView where the various entities that have donated code or Funding are shown with their logos.
+ Fix the mPrime, F1, F1spt, F7 functions in Hex- and Ortho- Ops


##  Synthetic Building Filters ##
+ Add ability to read list of ellipsoids into packing algorithms
+ Look into bias near outside of box during synthetic building
+ Fix synthetic builder to note if volume is 2D and actually pack slices


##  IO Filters ##
+ Add filter to generate IPF Images for each slice of data, or a user defined range of slices and plane by defining the normal to the plane (X, Y or Z)
+ Provide better feedback on 3D .ctf files (No way to do this without major changes to codes)
+ Add filter to write out slices as images of any array
+ Add ability to store EBSPs and EDS spectra in h5ebsd file and data container
+ Add filter to write out 2D slices as .ang, .ctf or .mic files
+ Add filter to dump ensemble stats/info
+ Add filter to read .edf data
+ Read in more meta-data about each phase: Space Group, Phase Name, Material Name


##  Add to the Pipeline Gui ##
+ Drag-and-Drop a pipeline file onto the DREAM3D UI and have it load that file
+ When loading a Pipeline file detect any paths that are the wrong platform (C: on Unix) and offer to pick a new file
+ Add a 'Rename Favorite' menu
+ allow user to right-click on a favorite and "delete, rename, ..." the favorite
+ allow user to right-click on a filter and display help in a separate window


##  Feature Request ##
+ A filter that finds all the twins in the microstructure and assigns them
to another phase (phase 3), so that they can be visualised differently to the surrouding
grains (phase 2) and grains that are untwinned (phase 1).


##  Surface Meshing ##
+ bug in edges file is stopping the use in the smoothing filter
+ Add mesh decimation filter (surface or volume)


##  MFE Smoothing ##
+ add ability to stop the smoothing


##  Bruker EBSD System Support ##
+ BASE64 decoder
+ XML Parser for the SEM Image file
+ Create XML Parsing code for each file from the Bruker File Archive
+ Include Expat as part of the DREAM3D Compile?


## Items Completed ##
+ Fix html Help for "Standardize Euler Angles"
+ Fix html Help for "Erode Dilate Coordination Number"
+ Fix html for FindSlicetoSliceRotations.html
+ Fix html for FindSlipTransmissionMetrics.html
+ Fix html for AddBadData.html
+ Fix html for AddNoise.html
+ fix HTML for JumbleOrientations.html
+ Missing Documenation File for GenericFilters/LinkFieldMapToCellArray
+ Missing Documenation File for GenericFilters/RenameCellArray
+ Missing Documenation File for GenericFilters/RenameFieldArray
+ Missing Documenation File for StatisticsFilters/GenerateEnsembleStatistics
+ Missing Documenation File for StatisticsFilters/FindMisorientations
+ RawBinaryReader does not honor the Origin value
+ Design/Implement better system to write up all the documentation but be able to convert to PDF/HTML/etc
+ Tutorial on Surface Meshing for Docs/Website
+ Add filter to generate IPF colors for triangles in surface mesh
+ Fix reference frame button on import filter to not reset to default
+ Add node type calculation during quick mesh
+ Change the Slip Transmission Metrics filter to store the metrics as Neighborlist values to save space and time for calculation.
+ Fix data check in numerous filters that are omitting the crystal structures check and performing it in the execute
+ Fix rename cell and field array filters to update the data container during the preflight
+ Fix the link field map filter to return an error during preflight if array is not correct type
+ Add filter to rename array from cell map
+ Add filter to rename array from field map
+ Add Filter to initialize the Field Map based on a user defined cell array such as Grain Ids.
+ Move FindNeighbors filter to the statistics group
+ Add some optimizations into the PackPrimaryGrains by creating constants and removing triple nested vectors.
+ Add ability to write out goal statistics/attributes for grains during packing synthetic volumes
+ Look into deleting field arrays after packing in synthetic structures
+ Fix distributionOps functions to not fart when there is only 1 grain in a bin.
+ Output file similar to TSL OIM Analysis software. See example from Brad Fromm (INLFileWriter filter)
+ Fix TotalSurfaceAreas in MatchCrystallography
+ Fix phasetype sizecheck in GenerateEnsembleStatistics.
+ Check error causing crash in the find mdf filter
+ Hookup setting of PhaseTypes from GUI in GenerateEnsembleStatistics filter
+ Finish implementing writing of GenerateEnsembleStatistics filter'S GUI values to pref file
+ Update the H5EbsdVolumeReader to only read those arrays asked for by the user or by the QualityMetricFilters
+ Finish updating ReadH5Ebsd filter Gui for HKL and HEDM data sets
+ Add advanced button on import filter to allow any recommended transformation
+ Add in a "Search Box" to top of Filter list to filter out the list based on keywords
+ Combine the threshold_points and DetermineGoodVoxels into a single filter
+ Add ability to "add filter" to plugin maker
+ Work through all IO filters to have extensions in the save and open popup box
+ Fix issue with writing euler angles to binary vtk file
+ Fix stats generator to display new phase when new phase added
+ Add option to choose IPF coloring direction
+ Add ability to sort through filters by name
+ Add helper code to populate edge table in surface meshes
+ Update import filter to have default manufacturer transformation options
+ Add filter abilities to read .tif or other image types into array
+ Add surface mesh data container writer
+ Add framework to pass contents of data continer to filters for display on GUIs in real-time
+ Add single threshold filter for cells
+ Add multi parameter threshold filter for cells
+ Add single threshold filter for fields
+ Add multi parameter threshold filter for fields
+ Add ability to remove arrays out of maps
+ Fix the scaling of the axes in the stats generator when values are changed
+ Add import and reading capability for .mic files from HEDM data
+ Modify read dream3d filter to allow user to specify arrays to read
+ Add grain reference misorientation filter for c-axis
+ Add orientation jumble filter
+ Add filter to determine average c-axis orientation for fields
+ Add clear data filter to delete data from user defined subsets
+ Add filter to reconstruct prior beta grain during initial segmentation
+ Fix crashing bug in insert precipitates filter
+ Confirm functionality of orientation ops functions (FZ specifically)
+ Add quick mesh filter
+ Add option to align feature centroid filter to use reference slice
+ Add coordination number based erode/dilate filter
+ Add filter to create "noise" in orientations in synthetic structures
+ Add filter to insert "bad data" in synthetic structures (based on location)
+ Add filter to "identify sample" (operates on good voxels)
+ Rework deformation statistics filter
+ Add filter to calculate boundary transmission paraters
+ Create a raw binary reader
+ Move reference frame transformations to readh5ebsd filter by calling out to rotation filters
+ Generalize possible rotations in the reference rotation filters
+ Add ability to store parent id at the field level
+ Add filter to create a cell array from a field array
+ Add storage of misorientations as list at field level for each grain in find mdf filter
+ Add writing of neighborlist field level arrays to csv file
+ Add filter or option to write out misorientation values
+ Add option to write neighborlist data to csv field data file
+ Add subgroups for further organizing filters
+ Add filter to flatten color image to grayscale
+ Add filter to segment features by scalar tolerance value
+ Modify all stats filters to check for 2D in all directions
+ Break out all stats filters to just calculate stats
+ Update file type stats generator can read (.dream3d)
+ Users can not open .h5stats in StatsGenerator
+ Add ability to open a previously saved stats file back into the stats generator
+ Add filter to calculate only misorientations
+ Modify FindMDF filter to only calculate the MDF, not the actual misorientations
+ Add filter to calculate ensemble stats (fit distributions to arrays)
+ Fix calculation+storage of real stats for reading into synthetic builder


##  No Longer Wanted ##
+ Add structured grid writer
