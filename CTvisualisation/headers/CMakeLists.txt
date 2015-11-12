SET(DICOM_VISUALISATION_HEADER_FILES glErrorCheck_inline.h global.h histogramDrawFunc.h histogramKeyboardFunc.h histogramMouseFunc.h importDicomFile.h menu.h openGLStatus.h rescaleVolume.h segementationMethods.h viewSegmentedData.h)

add_library(HEADER_FILES ${DICOM_VISUALISATION_HEADER_FILES})
