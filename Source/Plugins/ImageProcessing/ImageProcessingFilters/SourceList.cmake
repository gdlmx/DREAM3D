#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName ImageProcessingFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

#--------
# This macro must come first before we start adding any filters
START_FILTER_GROUP(${ImageProcessing_BINARY_DIR} "${_filterGroupName}" "ImageProcessing")


#---------
# List your public filters here
set(_PublicFilters
  #AlignSectionsPhaseCorrelation
  AutoThreshold
  ConvertArrayTo8BitImage
  DiscreteGaussianBlur
  GaussianBlur
  HoughCircles
  ImageCalculator
  ImageMath
#  ImageRegistration
  KMeans
  ManualThreshold
  MeanKernel
  MedianKernel
  MultiOtsuThreshold
  SobelEdge
  Watershed
  WriteImage
  ReadImage
  RGBToGray
  GrayToRGB
  FindMaxima
  BinaryWatershed
  DetermineStitching
)


#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_DREAM3D_FILTER(  "ImageProcessing" "ImageProcessing"
                        ${_filterGroupName} ${f}
                        ${ImageProcessing_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters

)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_DREAM3D_FILTER(  "ImageProcessing" "ImageProcessing"
                        ${_filterGroupName} ${f}
                        ${DREAM3DLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md FALSE)
endforeach()

ADD_DREAM3D_SUPPORT_HEADER(${ImageProcessing_SOURCE_DIR} ${_filterGroupName} ItkBridge.h)
ADD_DREAM3D_SUPPORT_HEADER(${ImageProcessing_SOURCE_DIR} ${_filterGroupName} TemplateUtilities.h)
ADD_DREAM3D_SUPPORT_HEADER(${ImageProcessing_SOURCE_DIR} ${_filterGroupName} ManualThresholdTemplate.h)
ADD_DREAM3D_SUPPORT_SOURCE(${ImageProcessing_SOURCE_DIR} ${_filterGroupName} ManualThresholdTemplate.cpp)

#---------------------
# This macro must come last after we are done adding all the filters and support files.
END_FILTER_GROUP(${ImageProcessing_BINARY_DIR} "${_filterGroupName}" "ImageProcessing")

